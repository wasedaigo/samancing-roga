#include "FileLoader.h"

#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>


std::string FileLoader::getFileData(QString path)
{
    // Get json file data
    QFile file(path);
    QString fileData;
    if (file.open( QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        while (!stream.atEnd())
        {
            fileData = stream.readAll();
        }
        file.close();
    }
    return fileData.toStdString();
}

Json::Value FileLoader::loadJsonFile(QString path)
{
    std::string inputJson = getFileData(path);

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(inputJson, root))
    {
        printf("json parse error");
    }
    return root;
}

void FileLoader::save(QString filePath, Json::Value root)
{
    // Save to file
    Json::StyledWriter writer;
    std::string outputJson = writer.write(root);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        file.close();
    }

    saveInitFile(filePath);
}

Json::Value FileLoader::load(QString filePath)
{
    Json::Value root = loadJsonFile(filePath);

    if (!root.empty())
    {
        // load succeeded
        // update initfile
        saveInitFile(filePath);
    }

    return root;
}

void FileLoader::init()
{
    QString initPath = getInitpath();
    load(initPath);
}

QString FileLoader::getInitpath()
{
    QFileInfo initFileInfo = QFileInfo(QDir::currentPath(), QString("init.json"));
    QString filePath = initFileInfo.absoluteFilePath();
    Json::Value root = loadJsonFile(filePath);

    if (root.empty())
    {
        return "";
    }
    else
    {
        return QString::fromStdString(root["initFilePath"].asString());
    }
}

void FileLoader::saveInitFile(QString targetpath)
{
    QFileInfo initFileInfo = QFileInfo(QDir::currentPath(), QString("init.json"));
    QString filePath = initFileInfo.absoluteFilePath();

    Json::StyledWriter writer;
    Json::Value data;
    data["initFilePath"] = targetpath.toStdString();
    std::string outputJson = writer.write(data);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        file.close();
    }
}
