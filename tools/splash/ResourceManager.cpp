#include "ResourceManager.h"

#include <QWidget>
#include <QFileInfo>
#include <QSound>
#include "DataModels/AnimationModel.h"
#include "GLSprite.h"

#include <QTextStream>

static QHash<QString, AnimationModel*> sAnimationHash;
static QString mResourcePath;

ResourceManager::ResourceManager()
{
}

AnimationModel* ResourceManager::getAnimation(QString path)
{
    if(!sAnimationHash.contains(path))
    {
        AnimationModel* pAnimationModel = new AnimationModel(NULL);

        if (pAnimationModel->loadData(ResourceManager::getResourcePath(path)))
        {
          sAnimationHash.insert(path, pAnimationModel);
        }
        else
        {
          delete pAnimationModel;
          pAnimationModel = NULL;
        }
    }

    return sAnimationHash[path];
}

// Call this function when you want to clean up all animation data allocated to
// the memory.
void ResourceManager::clearAnimationCache()
{
    QHash<QString, AnimationModel*>::Iterator iter = sAnimationHash.begin();
    while(iter != sAnimationHash.end())
    {
        delete iter.value();
        iter++;
    }
    sAnimationHash.clear();
}

QString ResourceManager::getResourcePath(QString path)
{
    QString rootPath = mResourcePath;

    if (path != "")
    {
        return rootPath.append("/").append(path);
    }
    else
    {
        return rootPath;
    }
}

ResourceManager::FileType ResourceManager::getFileType(QString path)
{
    QFileInfo info = QFileInfo(path);
    QString suffix = info.suffix();

    // Load Image
    if (suffix.compare(IMAGE_FORMAT, Qt::CaseInsensitive) == 0)
    {
        const QPixmap* pPixmap = AnimationModel::getPixmap(path);
        if (pPixmap)
        {
            return FileType_Image;
        }
    }
    else if (suffix.compare(ANIMATION_FORMAT, Qt::CaseInsensitive) == 0)
    {
        return FileType_Animation;
    }

    return FileType_Invalid;
}

// Play sound
void ResourceManager::playSound(QString path)
{
    QString rootPath = mResourcePath;
    QString fullPath = rootPath.append("/").append(SOUND_DIR.path()).append("/").append(path).append(".").append(SOUND_FORMAT);

    QSound *sound = new QSound(fullPath);
    if (sound->isAvailable()) {
        sound->play();
    }
    delete sound;
}

std::string ResourceManager::getFileData(QString path)
{
    // Get json file data
    QFile file(path);
    QString fileData;
    if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file );
        while ( !stream.atEnd() ) {
            fileData = stream.readAll();
        }
        file.close();
    }
    return fileData.toStdString();
}

Json::Value ResourceManager::loadJsonFile(QString path)
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

void ResourceManager::loadInitFile()
{
    Json::Value root = loadJsonFile(QDir::currentPath().append("/").append("init.json"));
    mResourcePath = QString::fromStdString(root["resourcePath"].asString());
}
