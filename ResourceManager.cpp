#include "ResourceManager.h"

#include <QWidget>
#include <QFileInfo>
#include "DataModels/AnimationModel.h"
#include "GLSprite.h"
static QHash<QString, AnimationModel*> sAnimationHash;

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
    QString rootPath = QDir::currentPath();
    rootPath.append("/");
    rootPath.append(ROOT_RESOURCE_DIR.path());
    return rootPath.append("/").append(path);
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

