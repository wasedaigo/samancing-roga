#ifndef FILELOADER_H
#define FILELOADER_H

#include "json/reader.h"
#include "json/writer.h"
#include <QString>

class FileLoader
{
public:
    static std::string getFileData(QString path);
    static Json::Value loadJsonFile(QString path);

    static void save(QString filePath, Json::Value root);
    static Json::Value load(QString filePath);
    static QString getInitpath();
    static void saveInitFile(QString targetpath);
};

#endif // FILELOADER_H
