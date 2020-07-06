#ifndef IODEVICESETTINGS_DESERIALIZE_H
#define IODEVICESETTINGS_DESERIALIZE_H

#include "iodevicesettings.h"
#include "serialportsettings.h"

class IoDeviceSettingsHelper {
public:
    static std::shared_ptr<IoDeviceSettings> Deserialize(const QString json_string) {
        QByteArray json_bytes = json_string.toLocal8Bit();
        auto json_doc=QJsonDocument::fromJson(json_bytes);

        if(json_doc.isNull()){
            qDebug()<<"Failed to create JSON doc.";
            return nullptr;
        }
        if(!json_doc.isObject()){
            qDebug()<<"JSON is not an object.";
            return nullptr;
        }

        QJsonObject json=json_doc.object();

        if(json.isEmpty()){
            qDebug()<<"JSON object is empty.";
            return nullptr;
        }

        const QString settingsType = json["iodevicetype"].toString();
        if (settingsType.isNull()) {
            qDebug() << "no IoDeviceSettings";
            return nullptr;
        } else if (SerialPortSettings_id == settingsType){
            return SerialPortSettings::Create(json);
        } else {
            qDebug() << "unknown IoDeviceSettings: " << settingsType;
            return nullptr;
        }
    }
};

#endif // IODEVICESETTINGS_DESERIALIZE_H
