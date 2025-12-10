#include "TrayIcon.h"
#include "Config.h"
#include "DiscordRPC.h"
#include <QApplication>
#include <QMessageBox>
#include <QActionGroup>
#include <QDebug>

TrayIcon::TrayIcon(DiscordRPC* discordRpc, QObject* parent)
    : QSystemTrayIcon(parent)
    , m_discordRpc(discordRpc)
{
    // Set tray icon
    setIcon(QIcon(":/icons/tray.png"));
    setToolTip("Deezer Discord RPC");

    createMenu();

    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::onTrayIconActivated);

    show();
}

void TrayIcon::createMenu() {
    m_menu = new QMenu();

    // Show window action
    m_showWindowAction = m_menu->addAction("Deezer Discord RPC");
    connect(m_showWindowAction, &QAction::triggered, this, &TrayIcon::onShowWindowAction);

    // Version action (disabled)
    m_versionAction = m_menu->addAction("Version: 1.3.7");
    m_versionAction->setEnabled(false);

    // Check for updates
    m_checkForUpdatesAction = m_menu->addAction("Check for updates");
    connect(m_checkForUpdatesAction, &QAction::triggered, this, &TrayIcon::onCheckForUpdatesAction);

    m_menu->addSeparator();

    // Status name submenu
    m_statusNameMenu = m_menu->addMenu("Status name");
    QActionGroup* statusNameGroup = new QActionGroup(m_statusNameMenu);
    statusNameGroup->setExclusive(true);

    auto currentStatusName = Config::instance().getStatusName();

    m_statusAppNameAction = m_statusNameMenu->addAction("Deezer");
    m_statusAppNameAction->setCheckable(true);
    m_statusAppNameAction->setData("app_name");
    m_statusAppNameAction->setChecked(currentStatusName == "app_name");
    statusNameGroup->addAction(m_statusAppNameAction);

    m_statusSongTitleAction = m_statusNameMenu->addAction("Song title");
    m_statusSongTitleAction->setCheckable(true);
    m_statusSongTitleAction->setData("song_title");
    m_statusSongTitleAction->setChecked(currentStatusName == "song_title");
    statusNameGroup->addAction(m_statusSongTitleAction);

    m_statusArtistsSongAction = m_statusNameMenu->addAction("Artists song");
    m_statusArtistsSongAction->setCheckable(true);
    m_statusArtistsSongAction->setData("artists_song");
    m_statusArtistsSongAction->setChecked(currentStatusName == "artists_song");
    statusNameGroup->addAction(m_statusArtistsSongAction);

    m_statusArtistsAndTitleAction = m_statusNameMenu->addAction("Artists song - Song title");
    m_statusArtistsAndTitleAction->setCheckable(true);
    m_statusArtistsAndTitleAction->setData("artists_and_title");
    m_statusArtistsAndTitleAction->setChecked(currentStatusName == "artists_and_title");
    statusNameGroup->addAction(m_statusArtistsAndTitleAction);

    m_statusTitleAndArtistsAction = m_statusNameMenu->addAction("Song title - Artists song");
    m_statusTitleAndArtistsAction->setCheckable(true);
    m_statusTitleAndArtistsAction->setData("title_and_artists");
    m_statusTitleAndArtistsAction->setChecked(currentStatusName == "title_and_artists");
    statusNameGroup->addAction(m_statusTitleAndArtistsAction);

    connect(statusNameGroup, &QActionGroup::triggered, this, &TrayIcon::onStatusNameAction);

    // Tooltip text submenu
    m_tooltipTextMenu = m_menu->addMenu("Tooltip text");
    QActionGroup* tooltipTextGroup = new QActionGroup(m_tooltipTextMenu);
    tooltipTextGroup->setExclusive(true);

    auto currentTooltipText = Config::instance().getTooltipText();

    m_tooltipAppNameAction = m_tooltipTextMenu->addAction("App name");
    m_tooltipAppNameAction->setCheckable(true);
    m_tooltipAppNameAction->setData("app_name");
    m_tooltipAppNameAction->setChecked(currentTooltipText == "app_name");
    tooltipTextGroup->addAction(m_tooltipAppNameAction);

    m_tooltipAppVersionAction = m_tooltipTextMenu->addAction("App version");
    m_tooltipAppVersionAction->setCheckable(true);
    m_tooltipAppVersionAction->setData("app_version");
    m_tooltipAppVersionAction->setChecked(currentTooltipText == "app_version");
    tooltipTextGroup->addAction(m_tooltipAppVersionAction);

    m_tooltipAppNameAndVersionAction = m_tooltipTextMenu->addAction("App name and version");
    m_tooltipAppNameAndVersionAction->setCheckable(true);
    m_tooltipAppNameAndVersionAction->setData("app_name_and_version");
    m_tooltipAppNameAndVersionAction->setChecked(currentTooltipText == "app_name_and_version");
    tooltipTextGroup->addAction(m_tooltipAppNameAndVersionAction);

    m_tooltipArtistsAndTitleAction = m_tooltipTextMenu->addAction("Artists song - Song title");
    m_tooltipArtistsAndTitleAction->setCheckable(true);
    m_tooltipArtistsAndTitleAction->setData("artists_and_title");
    m_tooltipArtistsAndTitleAction->setChecked(currentTooltipText == "artists_and_title");
    tooltipTextGroup->addAction(m_tooltipArtistsAndTitleAction);

    m_tooltipTitleAndArtistsAction = m_tooltipTextMenu->addAction("Song title - Artists song");
    m_tooltipTitleAndArtistsAction->setCheckable(true);
    m_tooltipTitleAndArtistsAction->setData("title_and_artists");
    m_tooltipTitleAndArtistsAction->setChecked(currentTooltipText == "title_and_artists");
    tooltipTextGroup->addAction(m_tooltipTitleAndArtistsAction);

    connect(tooltipTextGroup, &QActionGroup::triggered, this, &TrayIcon::onTooltipTextAction);

    // Don't close to tray
    m_dontCloseToTrayAction = m_menu->addAction("Don't close to tray");
    m_dontCloseToTrayAction->setCheckable(true);
    m_dontCloseToTrayAction->setChecked(Config::instance().getDontCloseToTray());
    connect(m_dontCloseToTrayAction, &QAction::triggered, this, &TrayIcon::onDontCloseToTrayAction);

    // Reconnect RPC
    m_reconnectAction = m_menu->addAction("Reconnect RPC");
    connect(m_reconnectAction, &QAction::triggered, this, &TrayIcon::onReconnectAction);

    m_menu->addSeparator();

    // About
    m_aboutAction = m_menu->addAction("About");
    connect(m_aboutAction, &QAction::triggered, this, &TrayIcon::onAboutAction);

    // Quit
    m_quitAction = m_menu->addAction("Quit");
    connect(m_quitAction, &QAction::triggered, this, &TrayIcon::onQuitAction);

    setContextMenu(m_menu);
}

void TrayIcon::updateTooltip(const QString& text) {
    setToolTip(text);
}

void TrayIcon::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        emit showWindowRequested();
    }
}

void TrayIcon::onShowWindowAction() {
    emit showWindowRequested();
}

void TrayIcon::onStatusNameAction() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString statusName = action->data().toString();
        Config::instance().setStatusName(statusName);
        emit statusNameChanged(statusName);
        qDebug() << "Status name changed to:" << statusName;
    }
}

void TrayIcon::onTooltipTextAction() {
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString tooltipText = action->data().toString();
        Config::instance().setTooltipText(tooltipText);
        emit tooltipTextChanged(tooltipText);
        qDebug() << "Tooltip text changed to:" << tooltipText;
    }
}

void TrayIcon::onDontCloseToTrayAction() {
    bool checked = m_dontCloseToTrayAction->isChecked();
    Config::instance().setDontCloseToTray(checked);
    emit dontCloseToTrayChanged(checked);
    qDebug() << "Don't close to tray changed to:" << checked;
}

void TrayIcon::onReconnectAction() {
    emit reconnectRequested();
    if (m_discordRpc) {
        m_discordRpc->disconnect();
        m_discordRpc->connect();
        qDebug() << "Reconnecting Discord RPC...";
    }
}

void TrayIcon::onCheckForUpdatesAction() {
    emit checkForUpdatesRequested();
    QMessageBox::information(nullptr, "Check for Updates", 
        "Please check for updates at:\nhttps://github.com/CuteTenshii/deezer-discord-rpc/releases");
}

void TrayIcon::onAboutAction() {
    QMessageBox::about(nullptr, "About Deezer Discord RPC",
        "<h3>Deezer Discord RPC</h3>"
        "<p>Version: 1.3.7 (C++ Implementation)</p>"
        "<p>A Discord Rich Presence client for Deezer</p>"
        "<p>License: MIT</p>"
        "<p><a href='https://github.com/CuteTenshii/deezer-discord-rpc'>GitHub Repository</a></p>");
}

void TrayIcon::onQuitAction() {
    emit quitRequested();
}
