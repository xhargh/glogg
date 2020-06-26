#include <QDebug>

#include "ttylogdata.h"

TtyLogData::TtyLogData() : ILogData(), m_maxLineLength(0)
{
    m_serialPort.setBaudRate(QSerialPort::Baud115200);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setStopBits(QSerialPort::OneStop);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);
}

TtyLogData::~TtyLogData()
{

}

void TtyLogData::attachFile( const QString& fileName ) {
    m_serialPort.setPortName(fileName);
    if (m_serialPort.open(QIODevice::ReadWrite)) {
        qInfo() << __func__ << " " << fileName << " opened successfully";
        connect(&m_serialPort, &QSerialPort::readyRead, this, &TtyLogData::readDataSlot);
    }
}

void TtyLogData::write(QString str)
{
    qInfo() << "write: " + str;
    m_serialPort.write(str.toLatin1() + "\n");
}

QString TtyLogData::doGetLineString(qint64 line) const
{
    return m_lines[line].second;
}

QString TtyLogData::doGetExpandedLineString(qint64 line) const
{
    auto l = m_lines[line];
    return l.first.toString(Qt::ISODateWithMs) + " " + l.second;
}

QStringList TtyLogData::doGetLines(qint64 first_line, int number) const
{
    QStringList qs;
    for (int i = 0; i < number; i++) {
        qs.append(doGetLineString(i + first_line));
    }
    return qs;
}

QStringList TtyLogData::doGetExpandedLines(qint64 first_line, int number) const
{
    QStringList qs;
    for (int i = 0; i < number; i++) {
        qs.append(doGetExpandedLineString(i + first_line));
    }
    return qs;
}

qint64 TtyLogData::doGetNbLine() const
{
    return m_lines.size();
}

int TtyLogData::doGetMaxLength() const
{
    return m_maxLineLength;
}

int TtyLogData::doGetLineLength(qint64 line) const
{
    return doGetLineString(line).length();
}

void TtyLogData::doSetDisplayEncoding(Encoding encoding)
{
    qInfo() << __func__ << (int)encoding << "\n";
}

void TtyLogData::doSetMultibyteEncodingOffsets(int before_cr, int after_cr)
{
    qInfo() << __func__ << before_cr << " - " << after_cr << "\n";

}

QDateTime TtyLogData::getLastModifiedDate() const
{
    if (m_lines.size() == 0) {
        return QDateTime::currentDateTime();
    } else {
        return m_lines.back().first;
    }
}

void TtyLogData::readDataSlot()
{
    while (m_serialPort.canReadLine()) {
        const QByteArray data = m_serialPort.readLine();
        QString d = QString(data);
        if (d.length() > m_maxLineLength) {
            m_maxLineLength = d.length();
        }
        m_lines.push_back(std::make_pair(QDateTime::currentDateTime(), d));
        qDebug() << d;
        emit fileChanged( LogData::MonitoredFileStatus::DataAdded );
        emit loadingFinished ( LoadingStatus::Successful );
    }
}

