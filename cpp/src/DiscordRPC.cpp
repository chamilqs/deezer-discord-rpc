#include "DiscordRPC.h"
#include <discord_rpc.h>
#include <QDebug>
#include <cstring>

// Static pointer to instance for callbacks
static DiscordRPC* s_instance = nullptr;

DiscordRPC::DiscordRPC(QObject* parent)
    : QObject(parent)
    , m_connected(false)
    , m_applicationId("1258131430928547880") // Same client ID as TypeScript version
{
    s_instance = this;
}

DiscordRPC::~DiscordRPC() {
    disconnect();
    s_instance = nullptr;
}

bool DiscordRPC::connect() {
    if (m_connected) {
        return true;
    }

    DiscordEventHandlers handlers;
    std::memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleReady;
    handlers.disconnected = handleDisconnected;
    handlers.errored = handleErrored;

    Discord_Initialize(m_applicationId.toUtf8().constData(), &handlers, 1, nullptr);
    
    qDebug() << "Discord RPC: Initializing...";
    m_connected = true;
    
    return true;
}

void DiscordRPC::disconnect() {
    if (!m_connected) {
        return;
    }

    clearActivity();
    Discord_Shutdown();
    m_connected = false;
    emit disconnected();
    qDebug() << "Discord RPC: Disconnected";
}

bool DiscordRPC::isConnected() const {
    return m_connected;
}

void DiscordRPC::updatePresence(
    const QString& details,
    const QString& state,
    const QString& largeImageKey,
    const QString& largeImageText,
    int64_t startTimestamp,
    int64_t endTimestamp,
    const QString& buttonLabel,
    const QString& buttonUrl)
{
    if (!m_connected) {
        qWarning() << "Discord RPC: Not connected, cannot update presence";
        return;
    }

    // Convert QString to QByteArray and keep them alive for the duration of Discord_UpdatePresence
    QByteArray detailsData = details.toUtf8();
    QByteArray stateData = state.toUtf8();
    QByteArray largeImageKeyData = largeImageKey.toUtf8();
    QByteArray largeImageTextData = largeImageText.toUtf8();
    QByteArray buttonLabelData = buttonLabel.toUtf8();
    QByteArray buttonUrlData = buttonUrl.toUtf8();

    DiscordRichPresence presence;
    std::memset(&presence, 0, sizeof(presence));

    if (!details.isEmpty()) {
        presence.details = detailsData.constData();
    }

    if (!state.isEmpty()) {
        presence.state = stateData.constData();
    }

    if (!largeImageKey.isEmpty()) {
        presence.largeImageKey = largeImageKeyData.constData();
    }

    if (!largeImageText.isEmpty()) {
        presence.largeImageText = largeImageTextData.constData();
    }

    if (startTimestamp > 0) {
        presence.startTimestamp = startTimestamp;
    }

    if (endTimestamp > 0) {
        presence.endTimestamp = endTimestamp;
    }

    // Discord RPC supports up to 2 buttons
    DiscordButton buttons[1];
    if (!buttonLabel.isEmpty() && !buttonUrl.isEmpty()) {
        std::memset(buttons, 0, sizeof(buttons));
        buttons[0].label = buttonLabelData.constData();
        buttons[0].url = buttonUrlData.constData();
        
        presence.buttons = buttons;
        presence.buttonCount = 1;
    }

    Discord_UpdatePresence(&presence);
    qDebug() << "Discord RPC: Updated presence -" << details << "|" << state;
}

void DiscordRPC::clearActivity() {
    if (!m_connected) {
        return;
    }

    Discord_ClearPresence();
    qDebug() << "Discord RPC: Cleared activity";
}

void DiscordRPC::runCallbacks() {
    if (m_connected) {
        Discord_RunCallbacks();
    }
}

// Static callback handlers
void DiscordRPC::handleReady(const DiscordUser* user) {
    if (s_instance) {
        qDebug() << "Discord RPC: Connected as" << user->username << "#" << user->discriminator;
        emit s_instance->connected();
    }
}

void DiscordRPC::handleDisconnected(int errorCode, const char* message) {
    qWarning() << "Discord RPC: Disconnected -" << errorCode << message;
    if (s_instance) {
        s_instance->m_connected = false;
        emit s_instance->disconnected();
    }
}

void DiscordRPC::handleErrored(int errorCode, const char* message) {
    qWarning() << "Discord RPC: Error -" << errorCode << message;
    if (s_instance) {
        emit s_instance->errorOccurred(QString("%1: %2").arg(errorCode).arg(message));
    }
}
