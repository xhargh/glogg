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
protected:
    std::vector<std::pair<QDateTime, QString>> m_lines;
    int m_maxLineLength;

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
};

#endif // IODEVICELOGDATA_H
