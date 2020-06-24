#ifndef TTYLOGDATA_H
#define TTYLOGDATA_H

#include <QObject>

#include "logdata.h"

class TtyLogData : public AbstractLogData {
    Q_OBJECT

public:
  // Creates an empty LogData
  TtyLogData();
  // Destroy an object
  ~TtyLogData();

  // Creates a new filtered data.
  // ownership is passed to the caller
  LogFilteredData* getNewFilteredData() const {return nullptr;}


  // AbstractLogData interface
protected:
  QString doGetLineString(qint64 line) const override;
  QString doGetExpandedLineString(qint64 line) const override;
  QStringList doGetLines(qint64 first_line, int number) const override;
  QStringList doGetExpandedLines(qint64 first_line, int number) const override;
  qint64 doGetNbLine() const override;
  int doGetMaxLength() const override;
  int doGetLineLength(qint64 line) const override;
  void doSetDisplayEncoding(Encoding encoding) override;
  void doSetMultibyteEncodingOffsets(int before_cr, int after_cr) override;

signals:
  // Sent during the 'attach' process to signal progress
  // percent being the percentage of completion.
  void loadingProgressed( int percent );
  // Signal the client the file is fully loaded and available.
  void loadingFinished( LoadingStatus status );
  // Sent when the file on disk has changed, will be followed
  // by loadingProgressed if needed and then a loadingFinished.
  void fileChanged( LogData::MonitoredFileStatus status );
};

#endif // TTYLOGDATA_H
