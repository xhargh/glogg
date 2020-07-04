#ifndef LOGDATABASE_H
#define LOGDATABASE_H

#include "abstractlogdata.h"
#include "loadingstatus.h"
#include <QDebug>
#include <QDateTime>
#include "logfiltereddata.h"

class LogDataBase : public AbstractLogData {
    Q_OBJECT
public:
    LogDataBase() : AbstractLogData() {}

    virtual void interruptLoading() {
        qInfo() << __func__;
    }

    virtual void reload(QTextCodec* forcedEncoding = nullptr) {
        (void)forcedEncoding;
        qInfo() << __func__;
    }

    // Get the auto-detected encoding for the indexed text.
    virtual QTextCodec* getDetectedEncoding() const {
        qInfo() << __func__;
        return nullptr;
    }

    virtual qint64 getFileSize() const {
        qInfo() << __func__;
        return 0;
    }

    virtual QDateTime getLastModifiedDate() const {
        qInfo() << __func__;
        return QDateTime::currentDateTime();
    }

    virtual std::unique_ptr<LogFilteredData> getNewFilteredData() const
    {
        return std::make_unique<LogFilteredData>( this );
    }

    virtual void attachFile( const QString& fileName ) {
        qInfo() << __func__ << "(" << fileName << ")";
    }

    virtual void write(QString str) {
        qInfo() << __func__ << "(" << str << ") - not implemented";
    };
    virtual bool isWritable() const {
        return false;
    };


signals:
  // Sent during the 'attach' process to signal progress
  // percent being the percentage of completion.
  void loadingProgressed( int percent );
  // Signal the client the file is fully loaded and available.
  void loadingFinished( LoadingStatus status );
  // Sent when the file on disk has changed, will be followed
  // by loadingProgressed if needed and then a loadingFinished.
  void fileChanged( MonitoredFileStatus status );

    // AbstractLogData interface
protected:
    virtual QString doGetLineString(LineNumber line) const override {
        qInfo() << __func__ << "(" << line.get() << ")";
        return QString();
    }
    virtual QString doGetExpandedLineString(LineNumber line) const override {
        qInfo() << __func__ << "(" << line.get() << ")";
        return QString();
    }
    virtual std::vector<QString> doGetLines(LineNumber first_line, LinesCount number) const override {
        qInfo() << __func__ << "(" << first_line.get() << ", " << number.get() << ")";
        return std::vector<QString>();
    }
    virtual std::vector<QString> doGetExpandedLines(LineNumber first_line, LinesCount number) const override {
        qInfo() << __func__ << "(" << first_line.get() << ", " << number.get() << ")";
        return std::vector<QString>();
    }
    virtual LinesCount doGetNbLine() const override {
        qInfo() << __func__;
        return LinesCount(0);
    }
    virtual LineLength doGetMaxLength() const override {
        qInfo() << __func__;
        return LineLength(0);
    }
    virtual LineLength doGetLineLength(LineNumber line) const override {
        qInfo() << __func__ << "(" << line.get() << ")";
        return LineLength(0);
    }
    virtual void doSetDisplayEncoding(const char *encoding) override {
        qInfo() << __func__ << "(" << encoding << ")";
    }
    virtual QTextCodec *doGetDisplayEncoding() const override {
        qInfo() << __func__;
        return nullptr;
    }
    virtual void doAttachReader() const override {
        qInfo() << __func__;
    }
    virtual void doDetachReader() const override {
        qInfo() << __func__;
    }

};

#endif // LOGDATABASE_H
