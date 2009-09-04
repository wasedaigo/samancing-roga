#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QString>

#define ANIMATION_FORMAT "ani"
#define IMAGE_FORMAT "png"

#define ROOT_RESOURCE_DIR QDir(QString("GameResource"))
#define ANIMATION_DIR QDir(QString("GameResource/Animations"))
#define ANIMATION_IMAGE_DIR QDir(QString("GameResource/Images"))
#define SOUND_DIR QDir(QString("GameResource/Sounds"))

class AnimationModel;
class GLSprite;
class QWidget;
class ResourceManager
{
public:
    enum FileType
    {
        FileType_Invalid,
        FileType_Image,
        FileType_Animation
    };
    ResourceManager();

    static AnimationModel* getAnimation(QString path);
    static void clearAnimationCache();
    static QString getResourcePath(QString path);
    static FileType getFileType(QString path);
};

#endif // RESOURCEMANAGER_H
