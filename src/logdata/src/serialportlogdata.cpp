#include "serialportlogdata.h"


SerialPortLogData::SerialPortLogData(SerialPortSettings *settings) : IoDeviceLogData()
{
    m_serialPort.setPortName(settings->name);
    m_serialPort.setBaudRate(settings->baudRate);
    m_serialPort.setDataBits(settings->dataBits);
    m_serialPort.setParity(settings->parity);
    m_serialPort.setStopBits(settings->stopBits);
    m_serialPort.setFlowControl(settings->flowControl);
}

SerialPortLogData::~SerialPortLogData()
{

}

void SerialPortLogData::attachFile(const QString &fileName)
{
    m_serialPort.setPortName(fileName);
    if (m_serialPort.open(QIODevice::ReadWrite)) {
        qInfo() << __func__ << " " << fileName << " opened successfully";
        connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialPortLogData::readDataSlot);
    }
}

/* qqq
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
*/


void SerialPortLogData::reload(QTextCodec *forcedEncoding)
{
    (void)forcedEncoding;
    m_lines.clear();
    emit fileChanged( MonitoredFileStatus::Truncated );
    emit loadingFinished ( LoadingStatus::Successful );
}

QString SerialPortLogData::doGetLineString(LineNumber line) const
{
    if (m_lines.empty()) {
        return QString();
    }
    return m_lines[line.get()].second;
}

QString SerialPortLogData::doGetExpandedLineString(LineNumber line) const
{
    if (m_lines.empty()) {
        return QString();
    }
    auto l = m_lines[line.get()];
    return l.first.toString(Qt::ISODateWithMs) + " " + l.second;
}

std::vector<QString> SerialPortLogData::doGetLines(LineNumber first_line, LinesCount number) const
{
    std::vector<QString> qs;
    for (unsigned int i = 0; i < number.get(); i++) {
        qs.push_back(doGetLineString(LineNumber(i + first_line.get())));
    }
    return qs;
}

std::vector<QString> SerialPortLogData::doGetExpandedLines(LineNumber first_line, LinesCount number) const
{
    std::vector<QString> qs;
    for (unsigned int i = 0; i < number.get(); i++) {
        qs.push_back(doGetExpandedLineString(LineNumber(i + first_line.get())));
    }
    return qs;
}

LinesCount SerialPortLogData::doGetNbLine() const
{
    return LinesCount(static_cast<unsigned int>(m_lines.size()));
}

LineLength SerialPortLogData::doGetMaxLength() const
{
    return LineLength(m_maxLineLength);
}

LineLength SerialPortLogData::doGetLineLength(LineNumber line) const
{
    return LineLength(doGetLineString(line).length());
}

void SerialPortLogData::doSetDisplayEncoding(const char *encoding)
{
    qInfo() << __func__ << encoding << "\n";
}

QDateTime SerialPortLogData::getLastModifiedDate() const
{
    if (m_lines.size() == 0) {
        return QDateTime::currentDateTime();
    } else {
        return m_lines.back().first;
    }
}

void SerialPortLogData::readDataSlot()
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
        emit fileChanged( MonitoredFileStatus::DataAdded );
        emit loadingFinished ( LoadingStatus::Successful );
    }
}
