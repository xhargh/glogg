#ifndef SERIALPORTLOGDATA_H
#define SERIALPORTLOGDATA_H

#include "iodevicelogdata.h"

#include <QObject>
#include <QSerialPort>

#include "serialportsettings.h"

class QSerialPort;

class SerialPortLogData : public IoDeviceLogData {
    Q_OBJECT
  private:

    QSerialPort m_serialPort;
    SerialPortSettings m_serialPortSettings;
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

  protected:

  private slots:
    void readDataSlot();
};

#endif // SERIALPORTLOGDATA_H
