#include <QDebug>
#include "ttylogdata.h"


TtyLogData::TtyLogData() : AbstractLogData()
{

}

TtyLogData::~TtyLogData()
{

}

QString TtyLogData::doGetLineString(qint64 line) const
{
    return QString("doGetLineString: ") + QString::number(line);
}

QString TtyLogData::doGetExpandedLineString(qint64 line) const
{
    return QString("doGetExpandedLineString: ") + QString::number(line);
}

QStringList TtyLogData::doGetLines(qint64 first_line, int number) const
{
    QStringList qs;
    for (int i = 0; i < number; i++) {
        qs.append(QString("doGetLines: ") + QString::number(first_line + i));
    }
    return qs;
}

QStringList TtyLogData::doGetExpandedLines(qint64 first_line, int number) const
{
    QStringList qs;
    for (int i = 0; i < number; i++) {
        qs.append(QString("doGetExpandedLines: ") + QString::number(first_line + i));
    }
    return qs;
}

qint64 TtyLogData::doGetNbLine() const
{
    return 200;
}

int TtyLogData::doGetMaxLength() const
{
    return 200;
}

int TtyLogData::doGetLineLength(qint64 line) const
{
    return doGetLineString(line).length();
}

void TtyLogData::doSetDisplayEncoding(Encoding encoding)
{
    qInfo() << __func__ << (int)encoding << "\n";
}

void TtyLogData::doSetMultibyteEncodingOffsets(int before_cr, int after_cr)
{
    qInfo() << __func__ << before_cr << " - " << after_cr << "\n";

}

