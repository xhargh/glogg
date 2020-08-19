#include "iodevicelogdata.h"

IoDeviceLogData::IoDeviceLogData()
    : LogDataBase()
    , m_timeRefType(TimeReferenceType::NoTimestamp)
    , m_showTimestamp(true)
    , m_includeTimestamp(true)
{
    // qInfo() << __func__;
}

QStringList IoDeviceLogData::supportedTimestampFormats() {
    return TimestampFormats;
}

void IoDeviceLogData::changeTimestampFormat(int index)
{
    // qInfo() << __func__ << " " << index;
    m_timeRefType = static_cast<TimeReferenceType>(index);
}

QString IoDeviceLogData::timestampPrefix(QDateTime current, const QDateTime* const previous) const {
    if (m_lines.empty()) {
        return QString();
    }

    QString separator = " ";

    switch (m_timeRefType) {
    case TimeReferenceType::UTC:
        return current.toUTC().toString(Qt::ISODateWithMs) + separator;
    case TimeReferenceType::Local:
        return current.toLocalTime().toString(Qt::ISODateWithMs) + separator;
    case TimeReferenceType::RelativeS: {
        auto deltaT = m_lines[0].first.msecsTo(current);
        return QString::number(deltaT / 1000) + "." + QString("%1").arg(deltaT % 1000, 3, 10, QChar('0')) + separator;
    }
    case TimeReferenceType::RelativeMS: {
        auto deltaT = m_lines[0].first.msecsTo(current);
        return QString::number(deltaT) + separator;
    }
    case TimeReferenceType::DeltaTimeS: {
        if (previous) {
            auto deltaT = previous->msecsTo(current);
            return QString("%1").arg(deltaT / 1000, 5, 10, QChar(' ')) + "." + QString("%1").arg(deltaT % 1000, 3, 10, QChar('0')) + separator;
        }
        return "    0.000" + separator;
    }
    case TimeReferenceType::DeltaTimeMS: {
        if (previous) {
            auto deltaT = previous->msecsTo(current);
            return QString("%1").arg(deltaT, 6, 10, QChar(' ')) + separator;
        }
        return "     0" + separator;
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
        if (line.get() == 0) {
            return timestampPrefix(l.first, nullptr) + l.second;
        } else {
            return timestampPrefix(l.first, &m_lines[line.get() - 1].first) + l.second;
        }
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
        if (line.get() == 0) {
            return timestampPrefix(l.first, nullptr) + l.second;
        } else {
            return timestampPrefix(l.first, &m_lines[line.get() - 1].first) + l.second;
        }
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
    (void)encoding;
    // qInfo() << __func__ << encoding << "\n";
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
        m_maxLineLength = (timestampPrefix(ts, m_lines.size() ? &m_lines.back().first : nullptr) + d).length();
    }
    m_lines.push_back(std::make_pair(ts, d));
    // qDebug() << d;
}
