#include "iodevicelogdata.h"

IoDeviceLogData::IoDeviceLogData() : LogDataBase()
{
    qInfo() << __func__;
}


QString IoDeviceLogData::doGetLineString(LineNumber line) const
{
    if (m_lines.empty()) {
        return QString();
    }
    return m_lines[line.get()].second;
}

QString IoDeviceLogData::doGetExpandedLineString(LineNumber line) const
{
    if (m_lines.empty()) {
        return QString();
    }
    auto l = m_lines[line.get()];
    return l.first.toString(Qt::ISODateWithMs) + " " + l.second;
}

std::vector<QString> IoDeviceLogData::doGetLines(LineNumber first_line, LinesCount number) const
{
    std::vector<QString> qs;
    for (unsigned int i = 0; i < number.get(); i++) {
        qs.push_back(doGetLineString(LineNumber(i + first_line.get())));
    }
    return qs;
}

std::vector<QString> IoDeviceLogData::doGetExpandedLines(LineNumber first_line, LinesCount number) const
{
    std::vector<QString> qs;
    for (unsigned int i = 0; i < number.get(); i++) {
        qs.push_back(doGetExpandedLineString(LineNumber(i + first_line.get())));
    }
    return qs;
}

LinesCount IoDeviceLogData::doGetNbLine() const
{
    return LinesCount(static_cast<unsigned int>(m_lines.size()));
}

LineLength IoDeviceLogData::doGetMaxLength() const
{
    return LineLength(m_maxLineLength);
}

LineLength IoDeviceLogData::doGetLineLength(LineNumber line) const
{
    return LineLength(doGetLineString(line).length());
}

void IoDeviceLogData::doSetDisplayEncoding(const char *encoding)
{
    qInfo() << __func__ << encoding << "\n";
}

QDateTime IoDeviceLogData::getLastModifiedDate() const
{
    if (m_lines.size() == 0) {
        return QDateTime::currentDateTime();
    } else {
        return m_lines.back().first;
    }
}
