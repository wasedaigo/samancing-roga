#ifndef CelModel_H
#define CelModel_H
#include <QObject>
#include "GLSprite.h"

class KeyFrameData;
class CelModel : public QObject
{
Q_OBJECT

public:
    KeyFrameData* getKeyFrameDataReference() const;
    void setKeyFrameDataReference(KeyFrameData* pKeyFrameData);

    CelModel();

public slots:
    void setTextureID(int value);
    void setTextureSrcRect(GLSprite::Rect value);

    void setBlendType(int index);
    void setRelativeToTarget(bool value);
    void setLookAtTarget(bool value);
    void setBlur(int value);

    void setCenterX(int value);
    void setCenterY(int value);
    void setCenterZ(int value);

    void setPositionX(int value);
    void setPositionY(int value);
    void setPositionZ(int value);
    void setPositionTweenType(int index);

    void setRotationX(int value);
    void setRotationY(int value);
    void setRotationZ(int value);
    void setRotationTweenType(int index);

    void setScaleX(double value);
    void setScaleY(double value);
    void setScaleTweenType(int index);

    void setAlpha(double value);
    void setAlphaTweenType(int index);

signals:
    void textureIDChanged(int value);
    void textureSrcRectChanged(GLSprite::Rect value);

    void blendTypeChanged(int value);
    void relativeToTargetChanged(bool value);
    void lookAtTargetChanged(bool value);
    void blurChanged(int value);

    void centerXChanged(int value);
    void centerYChanged(int value);
    void centerZChanged(int value);

    void positionXChanged(int value);
    void positionYChanged(int value);
    void positionZChanged(int value);
    void positionTweenTypeChanged(int value);

    void rotationXChanged(int value);
    void rotationYChanged(int value);
    void rotationZChanged(int value);
    void rotationTweenTypeChanged(int index);

    void scaleXChanged(double value);
    void scaleYChanged(double value);
    void scaleTweenTypeChanged(float index);

    void alphaChanged(double value);
    void alphaTweenTypeChanged(float index);

private:
    KeyFrameData* mpKeyFrameData;
};
#endif // CelModel_H
