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

    virtual LogFilteredData* getNewFilteredData() const {
        qInfo() << __func__;
        LogFilteredData* newFilteredData = new LogFilteredData( this );

        return newFilteredData;
    }

    virtual qint64 getFileSize() const {
        qInfo() << __func__;
        return 0;
    }

    virtual QDateTime getLastModifiedDate() const {
        qInfo() << __func__;
        return QDateTime::currentDateTime();
    }

    virtual void attachFile( const QString& fileName ) {
        qInfo() << __func__ << " " << fileName;
    }

    virtual void interruptLoading() {
        qInfo() << __func__;
    }

    virtual void reload() {
        qInfo() << __func__;
    }

    virtual void setPollingInterval( uint32_t interval_ms ) {
        qInfo() << __func__ << " " << interval_ms;
    }

    virtual EncodingSpeculator::Encoding getDetectedEncoding() const {
        qInfo() << __func__;
        return EncodingSpeculator::Encoding();
    }

    virtual void write(QString str) {
        qInfo() << __func__;
    }

};

#endif // ILOGDATA_H
