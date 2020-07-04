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
    std::vector<std::pair<QDateTime, QString>> m_lines;
    QSerialPort m_serialPort;
    SerialPortSettings m_serialPortSettings;
    int m_maxLineLength;
  public:
    // Creates an empty LogData
    SerialPortLogData(const SerialPortSettings* settings);
    // Destroy an object
    ~SerialPortLogData();

    virtual void attachFile( const QString& fileName ) override;
    virtual void write(QString str) override;
    virtual bool isWritable() const override;
    virtual IoDeviceSettings * GetIoSettings() override;
    virtual void reload(QTextCodec* forcedEncoding = nullptr) override;

    // AbstractLogData interface
  protected:
    virtual QString doGetLineString(LineNumber line) const override;
    virtual QString doGetExpandedLineString(LineNumber line) const override;
    virtual std::vector<QString> doGetLines(LineNumber first_line, LinesCount number) const override;
    virtual std::vector<QString> doGetExpandedLines(LineNumber first_line, LinesCount number) const override;
    // Get the length of the longest line
    virtual LinesCount doGetNbLine() const override;
    // Get the total number of lines
    virtual LineLength doGetMaxLength() const override;
    virtual LineLength doGetLineLength(LineNumber line) const override;
    virtual void doSetDisplayEncoding( const char* encoding) override;

    virtual QDateTime getLastModifiedDate() const override;

  private slots:
    void readDataSlot();
};

#endif // SERIALPORTLOGDATA_H
