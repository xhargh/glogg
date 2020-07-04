#ifndef IODEVICESETTINGS_H
#define IODEVICESETTINGS_H

#include <QString>

class IoDeviceSettings {
public:
    virtual ~IoDeviceSettings() = default;

    QString ioDeviceType;
    QString name;

    virtual QString Serialize() const = 0;
};

#endif // IODEVICESETTINGS_H
