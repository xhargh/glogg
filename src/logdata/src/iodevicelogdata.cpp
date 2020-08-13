#include "iodevicelogdata.h"

IoDeviceLogData::IoDeviceLogData()
    : LogDataBase()
    , m_timeRefType(TimeReferenceType::UTC)
    , m_showTimestamp(true)
    , m_includeTimestamp(true)
{
    qInfo() << __func__;
}

QString IoDeviceLogData::timestampPrefix(QDateTime t) const {
    if (m_lines.empty()) {
        return QString();
    }

    QString separator = " ";

    switch (m_timeRefType) {
    case TimeReferenceType::UTC:
        return t.toUTC().toString(Qt::ISODateWithMs) + separator;
    case TimeReferenceType::Local:
        return t.toLocalTime().toString(Qt::ISODateWithMs) + separator;
    case TimeReferenceType::RelativeS: {
        auto deltaT = m_lines[0].first.msecsTo(t);
        return QString::number(deltaT / 1000) + "." + QString("%1").arg(deltaT % 1000, 3, 10, QChar('0')) + separator;
    }
    case TimeReferenceType::RelativeMS: {
        auto deltaT = m_lines[0].first.msecsTo(t);
        return QString::number(deltaT) + separator;
    }
    default:
        ;
    }

    return "";
}

QString IoDeviceLogData::doGetLineString(LineNumber line) const
{
    if (m_lines.empty() || line.get() >= m_lines.size()) {
        return QString();
    }
    auto l = m_lines[line.get()];
    if (m_includeTimestamp) {
        return timestampPrefix(l.first) + l.second;
    }
    return l.second;
}


QString IoDeviceLogData::doGetExpandedLineString(LineNumber line) const
{
    if (m_lines.empty() || line.get() >= m_lines.size()) {
        return QString();
    }
    auto l = m_lines[line.get()];
    if (m_showTimestamp) {
        return timestampPrefix(l.first) + l.second;
    }
    return l.second;
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

void IoDeviceLogData::addLine(QString d)
{
    auto ts = QDateTime::currentDateTime();
    d.remove(QRegExp("[\\n\\r]")); // remove new line and carrage return
    if (d.length() > m_maxLineLength) {
        m_maxLineLength = (timestampPrefix(ts) + d).length();
    }
    m_lines.push_back(std::make_pair(ts, d));
    qDebug() << d;
}
