#include "DeezerWindow.h"
#include "Config.h"
#include "DiscordRPC.h"
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QMessageBox>

DeezerWindow::DeezerWindow(DiscordRPC* discordRpc, QWidget* parent)
    : QMainWindow(parent)
    , m_webView(new QWebEngineView(this))
    , m_discordRpc(discordRpc)
    , m_updateTimer(new QTimer(this))
    , m_callbackTimer(new QTimer(this))
    , m_playerReady(false)
{
    // Restore window size from config
    int width = Config::instance().getWindowWidth();
    int height = Config::instance().getWindowHeight();
    resize(width, height);

    if (width == 1920 && height == 1080) {
        showMaximized();
    }

    setWindowTitle("Deezer Discord RPC");
    setWindowIcon(QIcon(":/icons/app.png"));

    initWebView();

    // Set up timers
    connect(m_updateTimer, &QTimer::timeout, this, &DeezerWindow::onUpdateActivity);
    m_updateTimer->start(5000); // Update every 5 seconds

    // Discord callback timer
    connect(m_callbackTimer, &QTimer::timeout, [this]() {
        if (m_discordRpc) {
            m_discordRpc->runCallbacks();
        }
    });
    m_callbackTimer->start(1000); // Run callbacks every second
}

DeezerWindow::~DeezerWindow() {
    // Save window size
    if (!isMaximized()) {
        Config::instance().setWindowWidth(width());
        Config::instance().setWindowHeight(height());
    }
}

void DeezerWindow::initWebView() {
    setCentralWidget(m_webView);

    // Configure web engine settings
    QWebEngineProfile* profile = m_webView->page()->profile();
    profile->setHttpUserAgent(USER_AGENT);
    
    QWebEngineSettings* settings = m_webView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
    settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);

    // Connect signals
    connect(m_webView, &QWebEngineView::loadFinished, this, &DeezerWindow::onLoadFinished);
    
    // Handle console messages for debugging
    connect(m_webView->page(), &QWebEnginePage::javaScriptConsoleMessage,
            this, &DeezerWindow::onConsoleMessage);

    // Handle new window requests (OAuth popups)
    m_webView->page()->setUrlRequestInterceptor(nullptr); // TODO: Implement ad-blocking

    // Load Deezer
    QUrl url("https://account.deezer.com/login/");
    m_webView->load(url);
}

void DeezerWindow::onLoadFinished(bool success) {
    if (success) {
        qDebug() << "Page loaded successfully";
        // Wait a bit for JavaScript to initialize
        QTimer::singleShot(2000, this, &DeezerWindow::waitForPlayer);
    } else {
        qWarning() << "Page failed to load";
    }
}

void DeezerWindow::waitForPlayer() {
    // Check if player is ready
    QString checkPlayerScript = R"(
        (function() {
            var element = document.querySelector('[data-testid="item_title"]');
            return element !== null;
        })();
    )";

    m_webView->page()->runJavaScript(checkPlayerScript, [this](const QVariant& result) {
        if (result.toBool()) {
            qDebug() << "Player is ready";
            m_playerReady = true;
            setupObservers();
        } else {
            // Try again after 500ms
            QTimer::singleShot(500, this, &DeezerWindow::waitForPlayer);
        }
    });
}

void DeezerWindow::setupObservers() {
    // Inject JavaScript to monitor playback changes
    QString observerScript = R"(
        (function() {
            // Track title observer
            var titleElement = document.querySelector('.marquee-content > [data-testid="item_title"]');
            if (titleElement) {
                var trackObserver = new MutationObserver(function() {
                    console.log('Track changed');
                });
                trackObserver.observe(titleElement, { 
                    childList: true, 
                    subtree: true, 
                    characterData: true 
                });
            }

            // Play/pause button observer
            var playButton = document.querySelector('.chakra-button__group > button[data-testid^="play_button_"]');
            if (playButton) {
                var playObserver = new MutationObserver(function() {
                    console.log('Play state changed');
                });
                playObserver.observe(playButton, { 
                    attributes: true, 
                    childList: false, 
                    subtree: false 
                });
            }

            return true;
        })();
    )";

    m_webView->page()->runJavaScript(observerScript, [](const QVariant& result) {
        qDebug() << "Observers setup:" << result.toBool();
    });
}

void DeezerWindow::onUpdateActivity() {
    if (!m_playerReady) {
        return;
    }

    getCurrentTrackInfo();
}

void DeezerWindow::getCurrentTrackInfo() {
    // JavaScript to extract current track information
    QString extractScript = R"(
        (function() {
            if (typeof dzPlayer === 'undefined') {
                return JSON.stringify({error: 'Player not ready'});
            }

            var currentSong = dzPlayer.getCurrentSong();
            var albumId = currentSong?.ALB_ID;
            var trackId = dzPlayer.getSongId() || dzPlayer.getRadioId();
            var playerType = dzPlayer.getPlayerType();
            var mediaType = dzPlayer.getMediaType();
            var trackName = dzPlayer.getSongTitle() + (currentSong?.VERSION ? ' ' + currentSong?.VERSION : '');
            var albumName = dzPlayer.getAlbumTitle();
            var artists = currentSong?.ARTISTS?.map(function(art) { return art.ART_NAME; })?.join(', ') || 
                         dzPlayer.getArtistName();
            var firstArtistId = currentSong?.ART_ID;
            var playing = dzPlayer.isPlaying();
            var songTime = Math.floor(dzPlayer.getDuration() * 1000);
            var timeLeft = Math.floor(dzPlayer.getRemainingTime() * 1000);
            var cover = dzPlayer.getCover();
            
            var coverType = 'misc';
            if (mediaType === 'song') coverType = 'cover';
            if (mediaType === 'episode') coverType = 'talk';
            
            var coverUrl = 'https://e-cdns-images.dzcdn.net/images/' + coverType + '/' + cover + '/256x256-000000-80-0-0.jpg';

            return JSON.stringify({
                albumId: albumId,
                trackId: trackId,
                mediaType: mediaType,
                playerType: playerType,
                trackName: trackName,
                albumName: albumName,
                artists: artists,
                playing: playing,
                songTime: songTime,
                timeLeft: timeLeft,
                coverUrl: coverUrl,
                firstArtistId: firstArtistId
            });
        })();
    )";

    m_webView->page()->runJavaScript(extractScript, [this](const QVariant& result) {
        QString jsonString = result.toString();
        
        QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
        if (doc.isNull() || !doc.isObject()) {
            qWarning() << "Failed to parse track info:" << jsonString;
            return;
        }

        QJsonObject trackInfo = doc.object();
        
        // Check if there's an error
        if (trackInfo.contains("error")) {
            return;
        }

        // Update activity if track changed or play state changed
        bool shouldUpdate = false;
        
        if (m_currentTrack.isEmpty()) {
            shouldUpdate = true;
        } else {
            if (m_currentTrack["trackName"].toString() != trackInfo["trackName"].toString() ||
                m_currentTrack["playing"].toBool() != trackInfo["playing"].toBool()) {
                shouldUpdate = true;
            }
        }

        if (shouldUpdate) {
            m_currentTrack = trackInfo;
            updateDiscordActivity(trackInfo);
            updateTooltip(trackInfo);
        }
    });
}

void DeezerWindow::updateDiscordActivity(const QJsonObject& trackInfo) {
    if (!m_discordRpc) {
        return;
    }

    bool playing = trackInfo["playing"].toBool();
    
    if (!playing) {
        m_discordRpc->clearActivity();
        qDebug() << "Cleared activity (not playing)";
        return;
    }

    QString details = getStatusDetails(trackInfo);
    QString state = getStatusState(trackInfo);
    QString albumCover = trackInfo["coverUrl"].toString();
    QString albumName = trackInfo["albumName"].toString();
    
    // Calculate timestamps
    qint64 songTime = trackInfo["songTime"].toVariant().toLongLong();
    qint64 timeLeft = trackInfo["timeLeft"].toVariant().toLongLong();
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    qint64 startTimestamp = now - (songTime - timeLeft);
    qint64 endTimestamp = now + timeLeft;

    // Button
    QString trackId = trackInfo["trackId"].toString();
    QString mediaType = trackInfo["mediaType"].toString();
    QString buttonLabel = "Play on Deezer";
    QString buttonUrl;
    
    if (mediaType == "song") {
        buttonUrl = QString("https://www.deezer.com/track/%1").arg(trackId);
    } else if (mediaType == "episode") {
        buttonUrl = QString("https://www.deezer.com/episode/%1").arg(trackId);
    }

    m_discordRpc->updatePresence(
        details,
        state,
        albumCover,
        albumName,
        startTimestamp / 1000,  // Convert to seconds
        endTimestamp / 1000,
        buttonLabel,
        buttonUrl
    );

    qDebug() << "Updated activity:" << details << "|" << state;
}

QString DeezerWindow::getStatusDetails(const QJsonObject& trackInfo) {
    QString statusName = Config::instance().getStatusName();
    QString trackName = trackInfo["trackName"].toString();
    QString artists = trackInfo["artists"].toString();

    if (statusName == "artists_and_title") {
        return QString("%1 - %2").arg(artists, trackName);
    } else if (statusName == "title_and_artists") {
        return QString("%1 - %2").arg(trackName, artists);
    } else {
        return trackName;
    }
}

QString DeezerWindow::getStatusState(const QJsonObject& trackInfo) {
    QString statusName = Config::instance().getStatusName();
    QString artists = trackInfo["artists"].toString();

    if (statusName == "artists_and_title" || statusName == "title_and_artists") {
        return QString(); // No state when combining in details
    } else {
        return artists;
    }
}

void DeezerWindow::updateTooltip(const QJsonObject& trackInfo) {
    QString tooltipText = Config::instance().getTooltipText();
    QString tooltip;

    if (tooltipText == "app_name") {
        tooltip = "Deezer Discord RPC";
    } else if (tooltipText == "app_version") {
        tooltip = "Version 1.3.7";
    } else if (tooltipText == "app_name_and_version") {
        tooltip = "Deezer Discord RPC version 1.3.7";
    } else if (tooltipText == "artists_and_title") {
        tooltip = QString("%1 - %2")
            .arg(trackInfo["artists"].toString())
            .arg(trackInfo["trackName"].toString());
    } else if (tooltipText == "title_and_artists") {
        tooltip = QString("%1 - %2")
            .arg(trackInfo["trackName"].toString())
            .arg(trackInfo["artists"].toString());
    }

    // Note: To update tray tooltip, we would need a reference to TrayIcon
    // This would be handled by connecting signals in main.cpp
}

void DeezerWindow::onConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                                   const QString& message,
                                   int lineNumber,
                                   const QString& sourceId) {
    Q_UNUSED(level)
    Q_UNUSED(lineNumber)
    Q_UNUSED(sourceId)
    
    // Log interesting console messages
    if (message.contains("Track changed") || message.contains("Play state changed")) {
        qDebug() << "Console:" << message;
        // Trigger immediate update
        QTimer::singleShot(100, this, &DeezerWindow::onUpdateActivity);
    }
}

void DeezerWindow::closeEvent(QCloseEvent* event) {
    bool dontCloseToTray = Config::instance().getDontCloseToTray();
    
    if (dontCloseToTray) {
        emit quitRequested();
        event->accept();
    } else {
        // Hide to tray
        event->ignore();
        hide();
    }
}

void DeezerWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    
    if (!isMaximized()) {
        Config::instance().setWindowWidth(width());
        Config::instance().setWindowHeight(height());
    }
}

void DeezerWindow::showWindow() {
    show();
    raise();
    activateWindow();
}

void DeezerWindow::hideWindow() {
    hide();
}
