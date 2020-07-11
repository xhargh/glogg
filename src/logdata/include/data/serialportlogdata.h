#ifndef SERIALPORTLOGDATA_H
#define SERIALPORTLOGDATA_H

#include "iodevicelogdata.h"

#include <QObject>
#include <QSerialPort>
#include <QTimer>

#include "serialportsettings.h"

class QSerialPort;

class SerialPortLogData : public IoDeviceLogData {
    Q_OBJECT
  private:

    QSerialPort m_serialPort;
    SerialPortSettings m_serialPortSettings;

    QTimer m_checkPort;
    QString m_partialLine;
  public:
    // Creates an empty LogData
    SerialPortLogData(std::shared_ptr<SerialPortSettings> settings);
    // Destroy an object
    ~SerialPortLogData();

    virtual void attachFile( const QString& fileName ) override;
    virtual void write(QString str) override;
    virtual bool isWritable() const override;
    virtual IoDeviceSettings * GetIoSettings() override;
    virtual void reload(QTextCodec* forcedEncoding = nullptr) override;
    virtual void addLineInternal(QString string);

    virtual void disconnectPort(bool silent = false);

    virtual void clearLog() override;
  protected:

  private slots:
    void onCheckPortTimer();

    // QSerialPort
    void onBaudRateChanged(qint32 baudRate, QSerialPort::Directions directions);
    void onDataBitsChanged(QSerialPort::DataBits dataBits);
    void onDataTerminalReadyChanged(bool set);
    void onBreakEnabledChanged(bool set);
    void onErrorOccurred(QSerialPort::SerialPortError error);
    void onFlowControlChanged(QSerialPort::FlowControl flow);
    void onParityChanged(QSerialPort::Parity parity);
    void onRequestToSendChanged(bool set);
    void onStopBitsChanged(QSerialPort::StopBits stopBits);

    // QIoDevice
    void onReadyRead();
    void onAboutToClose();
    void onBytesWritten(qint64 bytes);
    void onChannelBytesWritten(int channel, qint64 bytes);
    void onChannelReadyRead(int channel);
    void onReadChannelFinished();
};

#endif // SERIALPORTLOGDATA_H
