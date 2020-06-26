#ifndef SERIALPORTSETTINGS_H
#define SERIALPORTSETTINGS_H

#include <QObject>
#include <QSerialPort>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class SerialPortSettings {
public:
    QString name;
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

    QString Serialize() const {
        QJsonObject json;
        json["name"] = name;
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

    static SerialPortSettings* Deserialize(QString json_string) {
        QByteArray json_bytes = json_string.toLocal8Bit();
        auto json_doc=QJsonDocument::fromJson(json_bytes);

        if(json_doc.isNull()){
            qDebug()<<"Failed to create JSON doc.";
            return nullptr;
        }
        if(!json_doc.isObject()){
            qDebug()<<"JSON is not an object.";
            return nullptr;
        }

        QJsonObject json=json_doc.object();

        if(json.isEmpty()){
            qDebug()<<"JSON object is empty.";
            return nullptr;
        }

        SerialPortSettings *sps = new SerialPortSettings();
        sps->name = json["name"].toString();
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
