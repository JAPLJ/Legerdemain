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

    QUuid addProfile(const QJsonObject& chart_manager_settings,
                     const QJsonArray& generator_types,
                     const QJsonObject& generator_settings,
                     const QString& name);
    void removeProfile(const QUuid& profile_id);
    void moveProfile(const QUuid& profile_id, int move);
    void changeName(const QUuid& profile_id, const QString& new_name);

    struct ProfileEntry {
        QUuid id;
        QString name;
        ProfileEntry(const QUuid& id, const QString& name) : id(id), name(name) { }
    };
    vector<ProfileEntry> getEntries() const;
    void readProfile(const QUuid& profile_id,
                     QJsonObject& chart_manager_settings,
                     QJsonArray& generator_types,
                     QJsonObject& generator_settings) const;
};

#endif // PROFILEMANAGER_H
