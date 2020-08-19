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


    enum class TimeReferenceType : int {
        NoTimestamp = 0,
        UTC = 1,            // ISO-8601
        Local = 2,          // ISO-8601
        RelativeS = 3,      // s.mmm
        RelativeMS = 4,     // mmmmm
        DeltaTimeS = 5,     // s.mmm
        DeltaTimeMS = 6     // mmmmm
    };

    // Make sure numeric value of TimeReferenceType matches the index in this string list
    const QStringList TimestampFormats = {
        "No timestamp",
        "UTC",
        "Local",
        "Relative s",
        "Relative ms",
        "Delta s",
        "Delta ms"
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

    virtual QStringList supportedTimestampFormats() override;
    virtual void changeTimestampFormat(int index) override;

private:
    // Return timestamp as string including extra space, or empty string
    QString timestampPrefix(QDateTime current, const QDateTime* const previous) const;
};

#endif // IODEVICELOGDATA_H
