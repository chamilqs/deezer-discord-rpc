#ifndef TRAYICON_H
#define TRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class DiscordRPC;

/**
 * @brief System tray icon with context menu
 * 
 * Manages the application's system tray icon and menu
 */
class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT

public:
    /**
     * @brief Construct TrayIcon
     * @param discordRpc Discord RPC instance for reconnection
     * @param parent Parent QObject
     */
    explicit TrayIcon(DiscordRPC* discordRpc, QObject* parent = nullptr);

    /**
     * @brief Update tooltip with current song information
     * @param text Tooltip text
     */
    void updateTooltip(const QString& text);

signals:
    /**
     * @brief Emitted when show/hide window is requested
     */
    void showWindowRequested();

    /**
     * @brief Emitted when quit is requested
     */
    void quitRequested();

    /**
     * @brief Emitted when status name is changed
     * @param statusName New status name
     */
    void statusNameChanged(const QString& statusName);

    /**
     * @brief Emitted when tooltip text preference is changed
     * @param tooltipText New tooltip text preference
     */
    void tooltipTextChanged(const QString& tooltipText);

    /**
     * @brief Emitted when don't close to tray is changed
     * @param dontClose New don't close to tray value
     */
    void dontCloseToTrayChanged(bool dontClose);

    /**
     * @brief Emitted when reconnect is requested
     */
    void reconnectRequested();

    /**
     * @brief Emitted when check for updates is requested
     */
    void checkForUpdatesRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowWindowAction();
    void onStatusNameAction();
    void onTooltipTextAction();
    void onDontCloseToTrayAction();
    void onReconnectAction();
    void onCheckForUpdatesAction();
    void onAboutAction();
    void onQuitAction();

private:
    void createMenu();
    
    QMenu* m_menu;
    QMenu* m_statusNameMenu;
    QMenu* m_tooltipTextMenu;
    
    QAction* m_showWindowAction;
    QAction* m_versionAction;
    QAction* m_checkForUpdatesAction;
    
    // Status name actions
    QAction* m_statusAppNameAction;
    QAction* m_statusSongTitleAction;
    QAction* m_statusArtistsSongAction;
    QAction* m_statusArtistsAndTitleAction;
    QAction* m_statusTitleAndArtistsAction;
    
    // Tooltip text actions
    QAction* m_tooltipAppNameAction;
    QAction* m_tooltipAppVersionAction;
    QAction* m_tooltipAppNameAndVersionAction;
    QAction* m_tooltipArtistsAndTitleAction;
    QAction* m_tooltipTitleAndArtistsAction;
    
    QAction* m_dontCloseToTrayAction;
    QAction* m_reconnectAction;
    QAction* m_aboutAction;
    QAction* m_quitAction;
    
    DiscordRPC* m_discordRpc;
};

#endif // TRAYICON_H
