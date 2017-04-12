#include "ProfileManager.h"

#include "RandomChartGenerator.h"
#include "Util.h"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>

#include <map>
#include <utility>
#include <cassert>

using std::map;

static const QString defaultProfileName = "default";

// JSON keys
static const QString KeyProfiles = "profiles";
static const QString KeyID = "id";
static const QString KeyChartManager = "chart_manager_settings";
static const QString KeyGeneratorTypes = "generator_types";
static const QString KeyGeneratorSettings = "generator_settings";
static const QString KeyName = "name";

ProfileManager::ProfileManager()
{

}

int ProfileManager::find(const QUuid &uuid) const {
    for (int i = 0; i < profiles_arr.size(); ++i) {
        if (QUuid(profiles_arr[i].toObject().value(KeyID).toString()) == uuid) {
            return i;
        }
    }
    return -1;
}

QJsonObject ProfileManager::defaultProfile() {
    QJsonObject empty_profile;
    empty_profile[KeyID] = QUuid::createUuid().toString();
    empty_profile[KeyChartManager] = QJsonObject();

    QJsonArray default_generator_types;
    default_generator_types.append(RandomChartGenerator::getName());
    default_generator_types.append(RandomChartGenerator::getName());

    empty_profile[KeyGeneratorTypes] = default_generator_types;
    empty_profile[KeyGeneratorSettings] = QJsonObject();
    empty_profile[KeyName] = defaultProfileName;

    return empty_profile;
}

void ProfileManager::fromFile(const QString &filepath) {
    QFile file(filepath);
    bool fileopen_succeeded = true;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fileopen_succeeded = false;
    }

    if (fileopen_succeeded) {
        QTextStream in(&file);
        QJsonDocument json_doc = QJsonDocument::fromJson(in.readAll().toUtf8());
        profiles = json_doc.object();
    } else {
        profiles = QJsonObject();
    }

    bool contains_valid_profile = true;
    contains_valid_profile &= profiles.contains(KeyProfiles);

    QJsonValue profiles_list = profiles.value(KeyProfiles);
    contains_valid_profile &= profiles_list.isArray();

    QJsonArray profiles_array = profiles_list.toArray();
    contains_valid_profile &= profiles_array.size() >= 1;

    if (!contains_valid_profile) {
        profiles_array = QJsonArray();
        profiles_array.append(defaultProfile());
        profiles = QJsonObject();
        profiles[KeyProfiles] = profiles_array;
    }

    order.clear();
    for (int i = 0; i < profiles_array.size(); ++i) {
        QJsonObject profile = profiles_array[i].toObject();
        if (!profile.contains(KeyID) || !profile[KeyID].isString()) {
            profile[KeyID] = QUuid::createUuid().toString();
            profiles_array[i] = profile;
        }
        order.push_back(QUuid(profile[KeyID].toString()));
    }

    profiles_arr = profiles_array;
    if (fileopen_succeeded) {
        file.close();
    }
}

bool ProfileManager::writeToFile(const QString &filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QJsonObject profiles_reordered;

    map<QString, int> uuid_to_idx;
    for (int i = 0; i < profiles_arr.size(); ++i) {
        uuid_to_idx[profiles_arr[i].toObject().value(KeyID).toString()] = i;
    }

    QJsonArray profiles_array_reordered;
    for (const QUuid& id : order) {
        profiles_array_reordered.append(profiles_arr[uuid_to_idx[id.toString()]]);
    }
    profiles_reordered[KeyProfiles] = profiles_array_reordered;

    QJsonDocument json_doc(profiles_reordered);
    QByteArray data(json_doc.toJson());

    if (file.write(data) != data.length()) {
        file.close();
        return false;
    }

    file.close();
    return true;
}

QUuid ProfileManager::addProfile(const ProfileContents& p, const QString &name) {
    QUuid uuid = QUuid::createUuid();

    QJsonObject profile;
    profile[KeyID] = uuid.toString();
    profile[KeyChartManager] = p.chart_manager_settings;
    profile[KeyGeneratorTypes] = p.generator_types;
    profile[KeyGeneratorSettings] = p.generator_settings;
    profile[KeyName] = name;

    order.push_back(uuid);
    profiles_arr.append(profile);

    return uuid;
}

void ProfileManager::updateProfile(const QUuid &profile_id, const ProfileContents &new_prof, const QString& new_name) {
    int idx = find(profile_id);
    if (idx >= 0) {
        QJsonObject prof = profiles_arr[idx].toObject();
        prof[KeyChartManager] = new_prof.chart_manager_settings;
        prof[KeyGeneratorTypes] = new_prof.generator_types;
        prof[KeyGeneratorSettings] = new_prof.generator_settings;
        prof[KeyName] = new_name;
        profiles_arr[idx] = prof;
    }
}

void ProfileManager::removeProfile(const QUuid &profile_id) {
    int idx = find(profile_id);
    if (idx >= 0) {
        order.erase(order.begin() + idx);
        profiles_arr.removeAt(idx);
    }
}

void ProfileManager::moveProfile(const QUuid &profile_id, int move) {
    int idx = find(profile_id);
    move = Util::clip(-1, move, +1);
    if (idx >= 0 && idx + move >= 0 && idx + move < static_cast<int>(order.size()) && move != 0) {
        std::swap(order[idx], order[idx + move]);
        QJsonValue t = profiles_arr[idx];
        profiles_arr[idx] = profiles_arr[idx + move];
        profiles_arr[idx + move] = t;
    }
}

void ProfileManager::changeName(const QUuid &profile_id, const QString &new_name) {
    int idx = find(profile_id);
    if (idx >= 0) {
        QJsonObject profile = profiles_arr[idx].toObject();
        profile[KeyName] = new_name;
        profiles_arr[idx] = profile;
    }
}

vector<ProfileManager::ProfileEntry> ProfileManager::getEntries() const {
    map<QString, int> uuid_to_idx;
    for (int i = 0; i < profiles_arr.size(); ++i) {
        uuid_to_idx[profiles_arr[i].toObject().value(KeyID).toString()] = i;
    }

    vector<ProfileEntry> res;
    for (int i = 0; i < static_cast<int>(order.size()); ++i) {
        int idx = uuid_to_idx[order[i].toString()];
        res.emplace_back(order[i], profiles_arr[idx].toObject().value(KeyName).toString(defaultProfileName));
    }

    return res;
}

ProfileManager::ProfileContents ProfileManager::readProfile(const QUuid &profile_id) const {
    ProfileContents res;
    int idx = find(profile_id);
    if (idx < 0) {
        return res;
    }

    QJsonObject profile = profiles_arr[idx].toObject();
    res.chart_manager_settings = profile.value(KeyChartManager).toObject();
    res.generator_types = profile.value(KeyGeneratorTypes).toArray();
    res.generator_settings = profile.value(KeyGeneratorSettings).toObject();

    return res;
}
