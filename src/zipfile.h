#ifndef ZIPFILE_H
#define ZIPFILE_H

#include <QString>

class ZipFile
{
public:
    static bool unzipFile(QString zipFilePath, QString destFolder);
};

#endif // ZIPFILE_H
