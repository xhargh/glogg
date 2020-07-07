#ifndef IODEVICELOGDATA_H
#define IODEVICELOGDATA_H

#include <QObject>
#include <QDateTime>
#include "logdatabase.h"
#include "iodevicesettings.h"

class IoDeviceLogData : public LogDataBase {
    Q_OBJECT
public:
    IoDeviceLogData();
    virtual IoDeviceSettings * GetIoSettings() = 0;

    enum class TimeReferenceType {
        NoTimestamp,
        UTC,            // ISO-8601
        Local,          // ISO-8601
        RelativeS,      // s.mmm
        RelativeMS      // mmmmm
    };

protected:
    std::vector<std::pair<QDateTime, QString>> m_lines;
    int m_maxLineLength;
    TimeReferenceType m_timeRefType;
    // include timestamp in search and export
    bool m_showTimestamp;
    // show timestamp in view
    bool m_includeTimestamp;

    virtual QString doGetLineString(LineNumber line) const override;
    virtual QString doGetExpandedLineString(LineNumber line) const override;
    virtual std::vector<QString> doGetLines(LineNumber first_line, LinesCount number) const override;
    virtual std::vector<QString> doGetExpandedLines(LineNumber first_line, LinesCount number) const override;
    // Get the length of the longest line
    virtual LinesCount doGetNbLine() const override;
    // Get the total number of lines
    virtual LineLength doGetMaxLength() const override;
    virtual LineLength doGetLineLength(LineNumber line) const override;
    virtual void doSetDisplayEncoding( const char* encoding) override;

    virtual QDateTime getLastModifiedDate() const override;

    void addLine(QString d);

private:
    // Return timestamp as string including extra space, or empty string
    QString timestampPrefix(QDateTime t) const;
};

#endif // IODEVICELOGDATA_H
