#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QString>
#include <QVariant>
#include <memory>

/**
 * @brief Configuration management class using QSettings
 * 
 * Stores and retrieves application settings persistently
 */
class Config {
public:
    /**
     * @brief Get the singleton instance
     */
    static Config& instance();

    /**
     * @brief Set a configuration value
     * @param key Configuration key
     * @param value Value to set
     */
    void set(const QString& key, const QVariant& value);

    /**
     * @brief Get a configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Configuration value
     */
    QVariant get(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Get window width
     */
    int getWindowWidth() const;

    /**
     * @brief Set window width
     */
    void setWindowWidth(int width);

    /**
     * @brief Get window height
     */
    int getWindowHeight() const;

    /**
     * @brief Set window height
     */
    void setWindowHeight(int height);

    /**
     * @brief Get status name preference
     * Options: app_name, song_title, artists_song, artists_and_title, title_and_artists
     */
    QString getStatusName() const;

    /**
     * @brief Set status name preference
     */
    void setStatusName(const QString& statusName);

    /**
     * @brief Get tooltip text preference
     * Options: app_name, app_version, app_name_and_version, artists_and_title, title_and_artists
     */
    QString getTooltipText() const;

    /**
     * @brief Set tooltip text preference
     */
    void setTooltipText(const QString& tooltipText);

    /**
     * @brief Get don't close to tray preference
     */
    bool getDontCloseToTray() const;

    /**
     * @brief Set don't close to tray preference
     */
    void setDontCloseToTray(bool dontClose);

private:
    Config();
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    std::unique_ptr<QSettings> m_settings;
};

#endif // CONFIG_H
