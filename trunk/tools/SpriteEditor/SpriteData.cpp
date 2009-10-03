#include "SpriteData.h"
#include <QPixmap>
#include <QDir>

SpriteData::SpriteData()
:  mFilePath(""),
   mResourceDirPath(""),
   mpSelectedSpriteDef(NULL)
{
}

void SpriteData::clear()
{
    mFilePath = "";
    mResourceDirPath = "";
    mpSelectedSpriteDef = NULL;
    mSpriteHash.clear();
}

// If passed key is not there, we will simply add that key
QString SpriteData::getSelectedImageID() const
{
    if (mpSelectedSpriteDef)
    {
        return mpSelectedSpriteDef->mImageID;
    }
    else
    {
        return "";
    }
}

// if this key is unused, we will remove this
bool SpriteData::imageFileExists(QString key) const
{
    QString imagePath = mResourceDirPath;
    imagePath.append("/").append(key);
    return QFile::exists(imagePath);
}

// Remove invalid reference
void SpriteData::refreshSpriteData()
{
    QMutableHashIterator<QString, SpriteDef> iter(mSpriteHash);
    while(iter.hasNext())
    {
        iter.next();
        if (!imageFileExists(iter.key()))
        {
            iter.remove();
        }
    }
}

void SpriteData::loadKey(QString key)
{
    if (!mSpriteHash.contains(key))
    {
        SpriteDef sDef;


        sDef.mCenterX = 0;
        sDef.mCenterY = 0;
        sDef.mImageID = key;

        // A bit too heavy for getting simple value, but i don't know what is the better way
        QString imagePath = mResourceDirPath;
        imagePath.append("/").append(sDef.mImageID);

        QPixmap *pImage = new QPixmap(imagePath);
        sDef.mGridX = pImage->width();
        sDef.mGridY = pImage->height();
        delete pImage;

        mSpriteHash.insert(key, sDef);
    }
}

void SpriteData::selectSpriteDef(QString key)
{
    loadKey(key);

    mpSelectedSpriteDef = &mSpriteHash[key];
}

bool SpriteData::importData(Json::Value root)
{
    mSpriteHash.clear();
    if (root.empty())
    {
        return false;
    }
    else
    {
        mResourceDirPath = QString::fromStdString(root["resourceDirPath"].asString());

        Json::Value sprites = root["sprites"];
        for(Json::Value::iterator iter = sprites.begin() ; sprites.end() != iter ; iter++)
        {
            QString keyName = QString::fromStdString(iter.key().asString());
            Json::Value data = *iter;
            SpriteDef sDef;

            sDef.mGridX = data["gridX"].asInt();
            sDef.mGridY = data["gridY"].asInt();
            sDef.mCenterX = data["centerX"].asInt();
            sDef.mCenterY = data["centerY"].asInt();
            sDef.mImageID = keyName;

            mSpriteHash.insert(keyName, sDef);
        }
        return true;
    }
}

Json::Value SpriteData::exportData() const
{
    Json::Value root;
    root["resourceDirPath"] = mResourceDirPath.toStdString();

    // Save Sprites' definition
    Json::Value& sprites = root["sprites"];
    QHash<QString, SpriteDef>::const_iterator iter = mSpriteHash.begin();
    while(iter != mSpriteHash.end())
    {
        SpriteDef spDef = static_cast<SpriteDef>(iter.value());

        Json::Value& spriteNode = sprites[spDef.mImageID.toStdString()];

        spriteNode["gridX"] = spDef.mGridX;
        spriteNode["gridY"] = spDef.mGridY;
        spriteNode["centerX"] = spDef.mCenterX;
        spriteNode["centerY"] = spDef.mCenterY;

        iter++;
    }

    return root;
}

int SpriteData::getGridX() const
{
    if (mpSelectedSpriteDef)
    {
        return mpSelectedSpriteDef->mGridX;
    }
    return 0;
}

int SpriteData::getGridY() const
{
    if (mpSelectedSpriteDef)
    {
        return mpSelectedSpriteDef->mGridY;
    }
    return 0;
}

int SpriteData::getCenterX() const
{
    if (mpSelectedSpriteDef)
    {
        return mpSelectedSpriteDef->mCenterX;
    }
    return 0;
}

int SpriteData::getCenterY() const
{
    if (mpSelectedSpriteDef)
    {
        return mpSelectedSpriteDef->mCenterY;
    }
    return 0;
}

// Selected Sprite def change
void SpriteData::setGridX(int value)
{
    if (mpSelectedSpriteDef)
    {
        mpSelectedSpriteDef->mGridX = value;
    }
}

void SpriteData::setGridY(int value)
{
    if (mpSelectedSpriteDef)
    {
        mpSelectedSpriteDef->mGridY = value;
    }
}

void SpriteData::setCenterX(int value)
{
    if (mpSelectedSpriteDef)
    {
        mpSelectedSpriteDef->mCenterX = value;
    }
}

void SpriteData::setCenterY(int value)
{
    if (mpSelectedSpriteDef)
    {
        mpSelectedSpriteDef->mCenterY = value;
    }
}
