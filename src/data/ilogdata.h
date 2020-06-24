#ifndef ILOGDATA_H
#define ILOGDATA_H

#include <QDateTime>
#include <QDebug>
#include "abstractlogdata.h"
#include "logfiltereddata.h"
#include "encodingspeculator.h"

class ILogData : public AbstractLogData {
    Q_OBJECT

public:
    ILogData() : AbstractLogData() {
        qInfo() << __func__;
    }

    LogFilteredData* getNewFilteredData() const {
        qInfo() << __func__;
        return nullptr;
    }

    qint64 getFileSize() const {
        qInfo() << __func__;
        return 0;
    }

    QDateTime getLastModifiedDate() const {
        qInfo() << __func__;
        return QDateTime::currentDateTime();
    }

    void attachFile( const QString& fileName ) {
        qInfo() << __func__ << " " << fileName;
    }

    void interruptLoading() {
        qInfo() << __func__;
    }

    void reload() {
        qInfo() << __func__;
    }

    void setPollingInterval( uint32_t interval_ms ) {
        qInfo() << __func__ << " " << interval_ms;
    }

    EncodingSpeculator::Encoding getDetectedEncoding() const {
        qInfo() << __func__;
        return EncodingSpeculator::Encoding();
    }

};

#endif // ILOGDATA_H
