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

    DiscordRichPresence presence;
    std::memset(&presence, 0, sizeof(presence));

    // Convert QString to char* (need to keep strings alive)
    static QByteArray detailsData;
    static QByteArray stateData;
    static QByteArray largeImageKeyData;
    static QByteArray largeImageTextData;
    static QByteArray buttonLabelData;
    static QByteArray buttonUrlData;

    if (!details.isEmpty()) {
        detailsData = details.toUtf8();
        presence.details = detailsData.constData();
    }

    if (!state.isEmpty()) {
        stateData = state.toUtf8();
        presence.state = stateData.constData();
    }

    if (!largeImageKey.isEmpty()) {
        largeImageKeyData = largeImageKey.toUtf8();
        presence.largeImageKey = largeImageKeyData.constData();
    }

    if (!largeImageText.isEmpty()) {
        largeImageTextData = largeImageText.toUtf8();
        presence.largeImageText = largeImageTextData.constData();
    }

    if (startTimestamp > 0) {
        presence.startTimestamp = startTimestamp;
    }

    if (endTimestamp > 0) {
        presence.endTimestamp = endTimestamp;
    }

    // Discord RPC supports up to 2 buttons
    static DiscordButton buttons[1];
    if (!buttonLabel.isEmpty() && !buttonUrl.isEmpty()) {
        buttonLabelData = buttonLabel.toUtf8();
        buttonUrlData = buttonUrl.toUtf8();
        
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
