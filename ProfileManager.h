#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

#include <vector>

using std::vector;

class ProfileManager
{
private:
    QJsonObject profiles;
    QJsonArray profiles_arr;
    vector<QUuid> order;

    int find(const QUuid& uuid) const;

    static QJsonObject defaultProfile();

public:
    ProfileManager();

    void fromFile(const QString& filepath);
    bool writeToFile(const QString& filepath);

    struct ProfileContents {
        QJsonObject chart_manager_settings;
        QJsonArray generator_types;
        QJsonObject generator_settings;
    };

    QUuid addProfile(const ProfileContents& profile, const QString& name);
    void removeProfile(const QUuid& profile_id);
    void moveProfile(const QUuid& profile_id, int move);
    void updateProfile(const QUuid& profile_id, const ProfileContents& new_profile, const QString& new_name);
    void changeName(const QUuid& profile_id, const QString& new_name);

    struct ProfileEntry {
        QUuid id;
        QString name;
        ProfileEntry(const QUuid& id, const QString& name) : id(id), name(name) { }
    };
    vector<ProfileEntry> getEntries() const;
    ProfileContents readProfile(const QUuid& profile_id) const;
};

#endif // PROFILEMANAGER_H
