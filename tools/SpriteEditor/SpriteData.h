#ifndef SPRITEDATA_H
#define SPRITEDATA_H

#include "json/reader.h"
#include "json/writer.h"
#include <QString>
#include <QHash>
#include <QObject>

#include "SpriteDef.h"

class SpriteData : public QObject
{
    Q_OBJECT
public:
    SpriteData();

    void clear();
    void refreshSpriteData();
    void loadKey(QString key);
    void selectSpriteDef(QString key);
    QString getSelectedImageID() const;

    int getGridX() const;
    int getGridY() const;
    int getCenterX() const;
    int getCenterY() const;

    QString mFilePath;
    QString mResourceDirPath;

    bool importData(Json::Value root);
    Json::Value exportData() const;

public slots:
    void setGridX(int value);
    void setGridY(int value);
    void setCenterX(int value);
    void setCenterY(int value);

private:
    bool imageFileExists(QString key) const;
    SpriteDef* mpSelectedSpriteDef;
    QHash<QString, SpriteDef> mSpriteHash;
};

#endif // SPRITEDATA_H
