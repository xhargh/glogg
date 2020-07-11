#include "serialportlogdata.h"
#include <QMetaEnum>


template<typename QEnum>
QString QtEnumToString (const QEnum value)
{
  return QMetaEnum::fromType<QEnum>().valueToKey(value);
}

const auto logIdString = QString("[klogg-io] - SerialPortLogData::");


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
        qInfo() << logIdString << __func__ << " " << fileName << " opened successfully";
        connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialPortLogData::onReadyRead);

        connect(&m_serialPort, &QSerialPort::aboutToClose, this, &SerialPortLogData::onAboutToClose);
        connect(&m_serialPort, &QSerialPort::bytesWritten, this, &SerialPortLogData::onBytesWritten);
        connect(&m_serialPort, &QSerialPort::channelBytesWritten, this, &SerialPortLogData::onChannelBytesWritten);
        connect(&m_serialPort, &QSerialPort::channelReadyRead, this, &SerialPortLogData::onChannelReadyRead);
        connect(&m_serialPort, &QSerialPort::readChannelFinished, this, &SerialPortLogData::onReadChannelFinished);

        connect(&m_serialPort, &QSerialPort::flowControlChanged, this, &SerialPortLogData::onFlowControlChanged);
        connect(&m_serialPort, &QSerialPort::parityChanged, this, &SerialPortLogData::onParityChanged);
        connect(&m_serialPort, &QSerialPort::requestToSendChanged, this, &SerialPortLogData::onRequestToSendChanged);
        connect(&m_serialPort, &QSerialPort::stopBitsChanged, this, &SerialPortLogData::onStopBitsChanged);
        connect(&m_serialPort, &QSerialPort::errorOccurred, this, &SerialPortLogData::onErrorOccurred);

        connect(&m_serialPort, &QSerialPort::baudRateChanged, this, &SerialPortLogData::onBaudRateChanged);
        connect(&m_serialPort, &QSerialPort::dataBitsChanged, this, &SerialPortLogData::onDataBitsChanged);
        connect(&m_serialPort, &QSerialPort::dataTerminalReadyChanged, this, &SerialPortLogData::onDataTerminalReadyChanged);
        connect(&m_serialPort, &QSerialPort::breakEnabledChanged, this, &SerialPortLogData::onBreakEnabledChanged);
    } else {
        addLineInternal(logIdString + tr("Unable to open") + " " + fileName);
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
    // m_lines.clear();
    disconnectPort(true);
    attachFile(m_serialPortSettings.getName());
    emit fileChanged( MonitoredFileStatus::Truncated );
    emit loadingFinished ( LoadingStatus::Successful );
}

void SerialPortLogData::disconnectPort(bool silent)
{
    if (!silent) {
        addLineInternal(logIdString + tr("Serial port") + " " + m_serialPortSettings.getName() + " " + tr("disconnected!"));
    }
    m_serialPort.close();

    disconnect(&m_serialPort, &QSerialPort::readyRead, this, &SerialPortLogData::onReadyRead);

    disconnect(&m_serialPort, &QSerialPort::aboutToClose, this, &SerialPortLogData::onAboutToClose);
    disconnect(&m_serialPort, &QSerialPort::bytesWritten, this, &SerialPortLogData::onBytesWritten);
    disconnect(&m_serialPort, &QSerialPort::channelBytesWritten, this, &SerialPortLogData::onChannelBytesWritten);
    disconnect(&m_serialPort, &QSerialPort::channelReadyRead, this, &SerialPortLogData::onChannelReadyRead);
    disconnect(&m_serialPort, &QSerialPort::readChannelFinished, this, &SerialPortLogData::onReadChannelFinished);

    disconnect(&m_serialPort, &QSerialPort::flowControlChanged, this, &SerialPortLogData::onFlowControlChanged);
    disconnect(&m_serialPort, &QSerialPort::parityChanged, this, &SerialPortLogData::onParityChanged);
    disconnect(&m_serialPort, &QSerialPort::requestToSendChanged, this, &SerialPortLogData::onRequestToSendChanged);
    disconnect(&m_serialPort, &QSerialPort::stopBitsChanged, this, &SerialPortLogData::onStopBitsChanged);
    disconnect(&m_serialPort, &QSerialPort::errorOccurred, this, &SerialPortLogData::onErrorOccurred);

    disconnect(&m_serialPort, &QSerialPort::baudRateChanged, this, &SerialPortLogData::onBaudRateChanged);
    disconnect(&m_serialPort, &QSerialPort::dataBitsChanged, this, &SerialPortLogData::onDataBitsChanged);
    disconnect(&m_serialPort, &QSerialPort::dataTerminalReadyChanged, this, &SerialPortLogData::onDataTerminalReadyChanged);
    disconnect(&m_serialPort, &QSerialPort::breakEnabledChanged, this, &SerialPortLogData::onBreakEnabledChanged);
}

void SerialPortLogData::clearLog()
{
    m_lines.clear();
    emit fileChanged( MonitoredFileStatus::Truncated );
    emit loadingFinished ( LoadingStatus::Successful );
}

void SerialPortLogData::addLineInternal(QString str)
{
    addLine(str);
    emit fileChanged( MonitoredFileStatus::DataAdded );
    emit loadingFinished ( LoadingStatus::Successful );
}

void SerialPortLogData::onReadyRead()
{
    while (m_serialPort.canReadLine()) {
        const QByteArray data = m_serialPort.readLine();
        QString d = QString(data);
        addLine(d);
    }
    emit fileChanged( MonitoredFileStatus::DataAdded );
    emit loadingFinished ( LoadingStatus::Successful );
}

void SerialPortLogData::onAboutToClose()
{
    qInfo() << (logIdString + __func__ + "()");
}

void SerialPortLogData::onBytesWritten(qint64 bytes)
{
    qInfo() << (logIdString + __func__ + "(" + QString::number(bytes) +")");
}

void SerialPortLogData::onChannelBytesWritten(int channel, qint64 bytes)
{
    qInfo() << (logIdString + __func__ + "(" + QString::number(channel) + ", " + QString::number(bytes) +")");
}

void SerialPortLogData::onChannelReadyRead(int channel)
{
    qInfo() << (logIdString + __func__ + "(" + QString::number(channel) + ")");
}

void SerialPortLogData::onReadChannelFinished()
{
    qInfo() << (logIdString + __func__ + "()");
}

void SerialPortLogData::onBaudRateChanged(qint32 baudRate, QSerialPort::Directions directions)
{
    QString tmp;
    switch(directions) {
        case QSerialPort::Input:
            tmp = "Input";
            break;
        case QSerialPort::Output:
            tmp = "Output";
            break;
        case QSerialPort::AllDirections:
            tmp = "AllDirections";
            break;
        default:
            tmp = QString(tr("Unknown: ")) + QString::number(directions);
    }

    addLineInternal(logIdString + __func__ + "(" + QString::number((int)baudRate) + ", " + tmp + ")");
}

void SerialPortLogData::onDataBitsChanged(QSerialPort::DataBits dataBits)
{
    addLineInternal(logIdString + __func__ + "(" + QtEnumToString(dataBits) + ")");
}

void SerialPortLogData::onDataTerminalReadyChanged(bool set)
{
    addLineInternal(logIdString + __func__ + "(" + (set ? "true" : "false") + ")");
}

void SerialPortLogData::onBreakEnabledChanged(bool set)
{
    addLineInternal(logIdString + __func__ + "(" + (set ? "true" : "false") + ")");
}

void SerialPortLogData::onErrorOccurred(QSerialPort::SerialPortError error)
{
    addLineInternal(logIdString + __func__ + "(" + QtEnumToString(error) + ")");
    disconnectPort();
}

void SerialPortLogData::onFlowControlChanged(QSerialPort::FlowControl flow)
{
    addLineInternal(logIdString + __func__ + "(" + QtEnumToString(flow) + ")");
}

void SerialPortLogData::onParityChanged(QSerialPort::Parity parity)
{
    addLineInternal(logIdString + __func__ + "(" + QtEnumToString(parity) + ")");
}

void SerialPortLogData::onRequestToSendChanged(bool set)
{
    addLineInternal(logIdString + __func__ + "(" + (set ? "true" : "false") + ")");
}

void SerialPortLogData::onStopBitsChanged(QSerialPort::StopBits stopBits)
{
    addLineInternal(logIdString + __func__ + "(" + QtEnumToString(stopBits) + ")");
}
