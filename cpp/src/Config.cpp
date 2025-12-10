#include "Config.h"
#include <QCoreApplication>

Config::Config()
    : m_settings(std::make_unique<QSettings>(
        QSettings::IniFormat,
        QSettings::UserScope,
        "DeezerDiscordRPC",
        "DeezerDiscordRPC"))
{
}

Config& Config::instance() {
    static Config instance;
    return instance;
}

void Config::set(const QString& key, const QVariant& value) {
    m_settings->setValue(key, value);
    m_settings->sync();
}

QVariant Config::get(const QString& key, const QVariant& defaultValue) const {
    return m_settings->value(key, defaultValue);
}

int Config::getWindowWidth() const {
    return get("window_width", 1920).toInt();
}

void Config::setWindowWidth(int width) {
    set("window_width", width);
}

int Config::getWindowHeight() const {
    return get("window_height", 1080).toInt();
}

void Config::setWindowHeight(int height) {
    set("window_height", height);
}

QString Config::getStatusName() const {
    return get("status_name", "artists_song").toString();
}

void Config::setStatusName(const QString& statusName) {
    set("status_name", statusName);
}

QString Config::getTooltipText() const {
    return get("tooltip_text", "app_name").toString();
}

void Config::setTooltipText(const QString& tooltipText) {
    set("tooltip_text", tooltipText);
}

bool Config::getDontCloseToTray() const {
    return get("dont_close_to_tray", false).toBool();
}

void Config::setDontCloseToTray(bool dontClose) {
    set("dont_close_to_tray", dontClose);
}
