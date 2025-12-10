#ifndef DEEZERWINDOW_H
#define DEEZERWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QTimer>
#include <QJsonObject>
#include <memory>

class DiscordRPC;

/**
 * @brief Main window with embedded Deezer web interface
 * 
 * Loads Deezer web interface and monitors playback state via JavaScript injection
 */
class DeezerWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construct DeezerWindow
     * @param discordRpc Discord RPC instance for activity updates
     * @param parent Parent widget
     */
    explicit DeezerWindow(DiscordRPC* discordRpc, QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~DeezerWindow();

    /**
     * @brief Show the window
     */
    void showWindow();

    /**
     * @brief Hide the window
     */
    void hideWindow();

protected:
    /**
     * @brief Handle close event (close to tray)
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief Handle resize event (save window size)
     */
    void resizeEvent(QResizeEvent* event) override;

signals:
    /**
     * @brief Emitted when window should quit completely
     */
    void quitRequested();

private slots:
    /**
     * @brief Called when page finishes loading
     */
    void onLoadFinished(bool success);

    /**
     * @brief Periodic update of activity
     */
    void onUpdateActivity();

    /**
     * @brief Handle JavaScript console messages
     */
    void onConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level,
                         const QString& message,
                         int lineNumber,
                         const QString& sourceId);

private:
    /**
     * @brief Initialize the web view
     */
    void initWebView();

    /**
     * @brief Inject JavaScript to monitor playback
     */
    void injectJavaScript();

    /**
     * @brief Wait for player to be ready
     */
    void waitForPlayer();

    /**
     * @brief Get current track information from JavaScript
     */
    void getCurrentTrackInfo();

    /**
     * @brief Update Discord activity based on track info
     */
    void updateDiscordActivity(const QJsonObject& trackInfo);

    /**
     * @brief Setup mutation observers in JavaScript
     */
    void setupObservers();

    /**
     * @brief Update tooltip based on current track
     */
    void updateTooltip(const QJsonObject& trackInfo);

    /**
     * @brief Get status text for Discord based on config
     */
    QString getStatusDetails(const QJsonObject& trackInfo);
    QString getStatusState(const QJsonObject& trackInfo);

    QWebEngineView* m_webView;
    DiscordRPC* m_discordRpc;
    QTimer* m_updateTimer;
    QTimer* m_callbackTimer;
    
    QJsonObject m_currentTrack;
    bool m_playerReady;
    
    static constexpr const char* USER_AGENT = 
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
        "(KHTML, like Gecko) Chrome/138.0.0.0 Safari/537.36";
};

#endif // DEEZERWINDOW_H
