#ifndef IODEVICELOGDATA_H
#define IODEVICELOGDATA_H

#include <QObject>
#include "logdatabase.h"
#include "iodevicesettings.h"

class IoDeviceLogData : public LogDataBase {
    Q_OBJECT
public:
    IoDeviceLogData();
    virtual IoDeviceSettings * GetIoSettings() = 0;
};

#endif // IODEVICELOGDATA_H
