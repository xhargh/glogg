#ifndef IODEVICESETTINGS_H
#define IODEVICESETTINGS_H

#include <QString>

class IoDeviceSettings {
public:
    IoDeviceSettings(QString type) : ioDeviceType_(type) {
    }
    virtual ~IoDeviceSettings() = default;

    QString getType() const {
        return ioDeviceType_;
    }
    QString getName() const     {
        return name_;
    }
    void setName(QString name) {
        name_ = name;
    }

    virtual QString Serialize() const = 0;
protected:
    QString ioDeviceType_;
    QString name_;

};

#endif // IODEVICESETTINGS_H
