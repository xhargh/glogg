#include <QDebug>

#include "seriallogdata.h"
#include "serialportsettings.h"

SerialLogData::SerialLogData(SerialPortSettings* settings) : ILogData(), m_serialPortSettings(*settings), m_maxLineLength(0)
{
    m_serialPort.setPortName(settings->name);
    m_serialPort.setBaudRate(settings->baudRate);
    m_serialPort.setDataBits(settings->dataBits);
    m_serialPort.setParity(settings->parity);
    m_serialPort.setStopBits(settings->stopBits);
    m_serialPort.setFlowControl(settings->flowControl);
}

SerialLogData::~SerialLogData()
{

}

void SerialLogData::attachFile( const QString& fileName ) {

    m_serialPort.setPortName(fileName);
    if (m_serialPort.open(QIODevice::ReadWrite)) {
        qInfo() << __func__ << " " << fileName << " opened successfully";
        connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialLogData::readDataSlot);
    }
}

void SerialLogData::write(QString str)
{
    qInfo() << "write: " + str;
    m_serialPort.write(str.toLatin1() + "\n");
}

bool SerialLogData::isWritable() const
{
    return true;
}

SerialPortSettings *SerialLogData::GetIoSettings()
{
    return &m_serialPortSettings;
}

void SerialLogData::reload()
{
    m_lines.clear();
    emit fileChanged( LogData::MonitoredFileStatus::Truncated );
    emit loadingFinished ( LoadingStatus::Successful );
}

QString SerialLogData::doGetLineString(qint64 line) const
{
    if (m_lines.empty()) {
        return QString();
    }
    return m_lines[line].second;
}

QString SerialLogData::doGetExpandedLineString(qint64 line) const
{
    if (m_lines.empty()) {
        return QString();
    }
    auto l = m_lines[line];
    return l.first.toString(Qt::ISODateWithMs) + " " + l.second;
}

QStringList SerialLogData::doGetLines(qint64 first_line, int number) const
{
    QStringList qs;
    for (int i = 0; i < number; i++) {
        qs.append(doGetLineString(i + first_line));
    }
    return qs;
}

QStringList SerialLogData::doGetExpandedLines(qint64 first_line, int number) const
{
    QStringList qs;
    for (int i = 0; i < number; i++) {
        qs.append(doGetExpandedLineString(i + first_line));
    }
    return qs;
}

qint64 SerialLogData::doGetNbLine() const
{
    return m_lines.size();
}

int SerialLogData::doGetMaxLength() const
{
    return m_maxLineLength;
}

int SerialLogData::doGetLineLength(qint64 line) const
{
    return doGetLineString(line).length();
}

void SerialLogData::doSetDisplayEncoding(Encoding encoding)
{
    qInfo() << __func__ << (int)encoding << "\n";
}

void SerialLogData::doSetMultibyteEncodingOffsets(int before_cr, int after_cr)
{
    qInfo() << __func__ << before_cr << " - " << after_cr << "\n";

}

QDateTime SerialLogData::getLastModifiedDate() const
{
    if (m_lines.size() == 0) {
        return QDateTime::currentDateTime();
    } else {
        return m_lines.back().first;
    }
}

void SerialLogData::readDataSlot()
{
    while (m_serialPort.canReadLine()) {
        const QByteArray data = m_serialPort.readLine();
        QString d = QString(data);
        d.remove(QRegExp("[\\n\\r]")); // remove new line and carrage return
        if (d.length() > m_maxLineLength) {
            m_maxLineLength = d.length();
        }
        m_lines.push_back(std::make_pair(QDateTime::currentDateTime(), d));
        qDebug() << d;
        emit fileChanged( LogData::MonitoredFileStatus::DataAdded );
        emit loadingFinished ( LoadingStatus::Successful );
    }
}

