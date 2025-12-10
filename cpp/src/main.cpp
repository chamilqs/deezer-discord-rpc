#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QLocalServer>
#include <QLocalSocket>
#include <QFile>
#include <QResource>

#include "DeezerWindow.h"
#include "DiscordRPC.h"
#include "TrayIcon.h"
#include "Config.h"

// Single instance lock using QLocalServer/QLocalSocket
class SingleInstanceGuard {
public:
    SingleInstanceGuard(const QString& key) : m_key(key), m_isFirstInstance(false) {
        // Try to connect to existing instance
        QLocalSocket socket;
        socket.connectToServer(m_key);
        
        if (socket.waitForConnected(500)) {
            // Another instance is running
            qDebug() << "Another instance is already running";
            return;
        }

        // We are the first instance, create server
        m_server = new QLocalServer();
        
        // Remove any stale server
        QLocalServer::removeServer(m_key);
        
        if (!m_server->listen(m_key)) {
            qWarning() << "Failed to create local server:" << m_server->errorString();
            delete m_server;
            m_server = nullptr;
            return;
        }

        m_isFirstInstance = true;
        qDebug() << "First instance, created local server";
    }

    ~SingleInstanceGuard() {
        if (m_server) {
            m_server->close();
            delete m_server;
        }
    }

    bool isFirstInstance() const {
        return m_isFirstInstance;
    }

    QLocalServer* server() const {
        return m_server;
    }

private:
    QString m_key;
    QLocalServer* m_server = nullptr;
    bool m_isFirstInstance;
};

// Resource initialization
void initResources() {
    // Register resources
    // In a real build, this would be done via .qrc file
    // For now, we'll use file paths directly
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application info
    QApplication::setApplicationName("DeezerDiscordRPC");
    QApplication::setApplicationVersion("1.3.7");
    QApplication::setOrganizationName("DeezerDiscordRPC");
    QApplication::setOrganizationDomain("deezer-discord-rpc");

    // Initialize resources
    initResources();

    qDebug() << "Deezer Discord RPC version" << QApplication::applicationVersion() << "(C++ Implementation)";

    // Single instance check
    SingleInstanceGuard guard("DeezerDiscordRPC_SingleInstance");
    
    if (!guard.isFirstInstance()) {
        QMessageBox::information(nullptr, "Already Running",
            "Deezer Discord RPC is already running.\n"
            "Check your system tray.");
        return 0;
    }

    // Create Discord RPC client
    DiscordRPC discordRpc;
    if (!discordRpc.connect()) {
        qWarning() << "Failed to connect to Discord RPC initially";
        // Continue anyway, will retry
    }

    // Create main window
    DeezerWindow window(&discordRpc);
    window.show();

    // Create system tray icon
    TrayIcon trayIcon(&discordRpc);

    // Connect signals
    QObject::connect(&trayIcon, &TrayIcon::showWindowRequested, [&window]() {
        window.showWindow();
    });

    QObject::connect(&trayIcon, &TrayIcon::quitRequested, [&]() {
        discordRpc.disconnect();
        window.close();
        QApplication::quit();
    });

    QObject::connect(&window, &DeezerWindow::quitRequested, [&]() {
        discordRpc.disconnect();
        trayIcon.hide();
        QApplication::quit();
    });

    QObject::connect(&trayIcon, &TrayIcon::reconnectRequested, [&discordRpc]() {
        discordRpc.disconnect();
        discordRpc.connect();
    });

    // Handle second instance attempts
    if (guard.server()) {
        QObject::connect(guard.server(), &QLocalServer::newConnection, [&]() {
            qDebug() << "Second instance detected, showing window";
            window.showWindow();
            
            // Accept and close the connection
            QLocalSocket* socket = guard.server()->nextPendingConnection();
            if (socket) {
                socket->close();
                socket->deleteLater();
            }
        });
    }

    qDebug() << "Application started successfully";

    return app.exec();
}
