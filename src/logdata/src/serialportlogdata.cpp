#include "serialportlogdata.h"

class UnableToOpenFileErr {

};

SerialPortLogData::SerialPortLogData(std::shared_ptr<SerialPortSettings> settings) :
    IoDeviceLogData(),
    m_serialPortSettings(*settings)
{
    m_serialPort.setPortName(settings->getName());
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
    } else {
        throw UnableToOpenFileErr();
    }
}

void SerialPortLogData::write(QString str)
{
    qInfo() << "write: " + str;
    m_serialPort.write(str.toLatin1() + "\n");
}

bool SerialPortLogData::isWritable() const
{
    return true;
}


IoDeviceSettings *SerialPortLogData::GetIoSettings()
{
    return &m_serialPortSettings;
}

void SerialPortLogData::reload(QTextCodec *forcedEncoding)
{
    (void)forcedEncoding;
    m_lines.clear();
    emit fileChanged( MonitoredFileStatus::Truncated );
    emit loadingFinished ( LoadingStatus::Successful );
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
        // qqq enqueue a partial reload?
        emit loadingFinished ( LoadingStatus::Successful );
    }
}
