#include "ResourceManager.h"

#include <QWidget>
#include <QFileInfo>
#include <QSound>
#include "DataModels/AnimationModel.h"
#include "GLSprite.h"
#include "FileLoader.h"
#include <QTextStream>

#include <irrKlang.h>
using namespace irrklang;

static QHash<QString, AnimationModel*> sAnimationHash;
static QString mResourcePath;

static ISoundEngine* engine = createIrrKlangDevice();

ResourceManager::ResourceManager()
{
}

AnimationModel* ResourceManager::getAnimation(QString path)
{
    if(!sAnimationHash.contains(path))
    {
        AnimationModel* pAnimationModel = new AnimationModel((QWidget*)NULL);

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
    QString fullPath = getResourcePath(SOUND_DIR.path()).append("/").append(path).append(".").append(SOUND_FORMAT);

    engine->play2D(fullPath.toStdString().c_str(), false);
}

QString ResourceManager::loadWorkingDirectory()
{
    mResourcePath = FileLoader::getInitpath();
    return mResourcePath;
}

void ResourceManager::setWorkingDirectory(QString newDirectory)
{
    mResourcePath = newDirectory;
    FileLoader::saveInitFile(mResourcePath);
}
