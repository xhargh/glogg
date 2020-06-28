#ifndef FILELOGDATA_H
#define FILELOGDATA_H

#include <QObject>

#include "ilogdata.h"

class FileLogData : public ILogData {
    Q_OBJECT

public:
  // Creates an empty LogData
  FileLogData();
  // Destroy an object
  ~FileLogData();

};


#endif // FILELOGDATA_H
