#ifndef LOGDATABASE_H
#define LOGDATABASE_H

#include "abstractlogdata.h"
#include <QDebug>


class LogDataBase : public AbstractLogData {
    // Q_OBJECT
public:
    LogDataBase() : AbstractLogData() {}

    // AbstractLogData interface
protected:
    QString doGetLineString(LineNumber line) const override {
        qInfo() << __func__ << "(" << line.get() << ")";
        return QString();
    }
    QString doGetExpandedLineString(LineNumber line) const override {
        qInfo() << __func__ << "(" << line.get() << ")";
        return QString();
    }
    std::vector<QString> doGetLines(LineNumber first_line, LinesCount number) const override {
        qInfo() << __func__ << "(" << first_line.get() << ", " << number.get() << ")";
        return std::vector<QString>();
    }
    std::vector<QString> doGetExpandedLines(LineNumber first_line, LinesCount number) const override {
        qInfo() << __func__ << "(" << first_line.get() << ", " << number.get() << ")";
        return std::vector<QString>();
    }
    LinesCount doGetNbLine() const override {
        qInfo() << __func__;
        return LinesCount(0);
    }
    LineLength doGetMaxLength() const override {
        qInfo() << __func__;
        return LineLength(0);
    }
    LineLength doGetLineLength(LineNumber line) const override {
        qInfo() << __func__ << "(" << line.get() << ")";
        return LineLength(0);
    }
    void doSetDisplayEncoding(const char *encoding) override {
        qInfo() << __func__ << "(" << encoding << ")";
    }
    QTextCodec *doGetDisplayEncoding() const override {
        qInfo() << __func__;
        return QTextCodec::codecForName( "ISO-8859-1" );
    }
    void doAttachReader() const override {
        qInfo() << __func__;
    }
    void doDetachReader() const override {
        qInfo() << __func__;
    }
};

#endif // LOGDATABASE_H
