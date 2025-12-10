#ifndef DISCORDRPC_H
#define DISCORDRPC_H

#include <QString>
#include <QObject>
#include <memory>
#include <cstdint>

// Forward declaration to avoid including discord_rpc.h in header
struct DiscordRichPresence;

/**
 * @brief Discord Rich Presence client wrapper
 * 
 * Manages Discord RPC connection and activity updates
 */
class DiscordRPC : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct Discord RPC client
     * @param parent Parent QObject
     */
    explicit DiscordRPC(QObject* parent = nullptr);
    
    /**
     * @brief Destructor - disconnects from Discord
     */
    ~DiscordRPC();

    /**
     * @brief Connect to Discord RPC
     * @return true if connection successful
     */
    bool connect();

    /**
     * @brief Disconnect from Discord RPC
     */
    void disconnect();

    /**
     * @brief Check if connected to Discord
     */
    bool isConnected() const;

    /**
     * @brief Update Discord Rich Presence activity
     * @param details Primary text (song title)
     * @param state Secondary text (artist name)
     * @param largeImageKey Large image key/URL (album cover)
     * @param largeImageText Large image hover text (album name)
     * @param startTimestamp Start timestamp for elapsed time
     * @param endTimestamp End timestamp for remaining time
     * @param buttonLabel Button text (e.g., "Listen on Deezer")
     * @param buttonUrl Button URL
     */
    void updatePresence(
        const QString& details,
        const QString& state,
        const QString& largeImageKey,
        const QString& largeImageText,
        int64_t startTimestamp = 0,
        int64_t endTimestamp = 0,
        const QString& buttonLabel = QString(),
        const QString& buttonUrl = QString()
    );

    /**
     * @brief Clear the current activity
     */
    void clearActivity();

    /**
     * @brief Run Discord callbacks (should be called periodically)
     */
    void runCallbacks();

signals:
    /**
     * @brief Emitted when successfully connected to Discord
     */
    void connected();

    /**
     * @brief Emitted when disconnected from Discord
     */
    void disconnected();

    /**
     * @brief Emitted on connection error
     */
    void errorOccurred(const QString& error);

private:
    bool m_connected;
    QString m_applicationId;
    
    // Static callback handlers
    static void handleReady(const struct DiscordUser* request);
    static void handleDisconnected(int errorCode, const char* message);
    static void handleErrored(int errorCode, const char* message);
};

#endif // DISCORDRPC_H
