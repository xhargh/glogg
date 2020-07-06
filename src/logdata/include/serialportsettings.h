#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QObject>
#include <QSerialPort>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QString>
#include "iodevicesettings.h"

const QString SerialPortSettings_id = "SerialPortSettings";

class SerialPortSettings : public IoDeviceSettings {
public:

    SerialPortSettings(QString type) : IoDeviceSettings(type) {}

    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;

    QString Serialize() const override {
        QJsonObject json;
        json["iodevicetype"] = SerialPortSettings_id;
        json["name"] = name_;
        json["baudRate"] = baudRate;
        json["stringBaudRate"] = stringBaudRate;
        json["dataBits"] = dataBits;
        json["stringDataBits"] = stringDataBits;
        json["parity"] = parity;
        json["stringParity"] = stringParity;
        json["stopBits"] = stopBits;
        json["stringStopBits"] = stringStopBits;
        json["flowControl"] = flowControl;
        json["stringFlowControl"] = stringFlowControl;
        QJsonDocument jdoc(json);
        return jdoc.toJson();
    }

    static std::shared_ptr<SerialPortSettings> Create(QJsonObject& json) {
        auto sps = std::make_shared<SerialPortSettings>(SerialPortSettings_id);
        assert(sps->ioDeviceType_ == json["iodevicetype"].toString());
        sps->name_ = json["name"].toString();
        sps->baudRate = json["baudRate"].toInt();
        sps->stringBaudRate = json["stringBaudRate"].toString();
        sps->dataBits = static_cast<QSerialPort::DataBits>(json["dataBits"].toInt());
        sps->stringDataBits = json["stringDataBits"].toString();
        sps->parity = static_cast<QSerialPort::Parity>(json["parity"].toInt());
        sps->stringParity = json["stringParity"].toString();
        sps->stopBits = static_cast<QSerialPort::StopBits>(json["stopBits"].toInt());
        sps->stringStopBits = json["stringStopBits"].toString();
        sps->flowControl = static_cast<QSerialPort::FlowControl>(json["flowControl"].toInt());
        sps->stringFlowControl = json["stringFlowControl"].toString();

        return sps;
    }

};

#endif // SERIALPORTSETTINGS_H
