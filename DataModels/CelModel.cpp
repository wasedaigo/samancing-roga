#include "CelModel.h"
#include "KeyFrameData.h"
CelModel::CelModel()
:   mpKeyFrameData(NULL)
{
}

KeyFrameData* CelModel::getKeyFrameDataReference() const
{
    return mpKeyFrameData;
}

void CelModel::setKeyFrameDataReference(KeyFrameData* pKeyFrameData)
{
    mpKeyFrameData = pKeyFrameData;
}

// Make our life easier by defining a macro here.
// This will generate a function
// 1: receive a value
// 2: compare it with current value
// 3: store value and emit signal if value changed
// The same functionality with an additional argument
#define SETSLOT(name0, name1, name2, type1, type2)\
void CelModel::set##name1(type1 value)\
{\
 if (mpKeyFrameData && mpKeyFrameData->m##name0 != value) {\
        mpKeyFrameData->m##name0 = (type2)value;\
        emit name2##Changed(value);\
    }\
}

SETSLOT(TweenTypes[KeyFrameData::TweenAttribute_position], PositionTweenType, positionTweenType, int, KeyFrameData::TweenType);
SETSLOT(TweenTypes[KeyFrameData::TweenAttribute_rotation], RotationTweenType, rotationTweenType, int, KeyFrameData::TweenType);
SETSLOT(TweenTypes[KeyFrameData::TweenAttribute_scale], ScaleTweenType, scaleTweenType, int, KeyFrameData::TweenType);
SETSLOT(TweenTypes[KeyFrameData::TweenAttribute_alpha], AlphaTweenType, alphaTweenType,  int, KeyFrameData::TweenType);

SETSLOT(SpriteDescriptor.mRelativeToTarget, RelativeToTarget, relativeToTarget, bool, bool);
SETSLOT(SpriteDescriptor.mLookAtTarget, LookAtTarget, lookAtTarget, bool, bool);
SETSLOT(SpriteDescriptor.mBlur, Blur, blur, int, int);

SETSLOT(SpriteDescriptor.mTextureSrcRect, TextureSrcRect, textureSrcRect, GLSprite::Rect, GLSprite::Rect);
SETSLOT(SpriteDescriptor.mBlendType, BlendType, blendType, int, GLSprite::BlendType);

SETSLOT(SpriteDescriptor.mPosition.mX, PositionX, positionX, int, int);
SETSLOT(SpriteDescriptor.mPosition.mY, PositionY, positionY, int, int);
SETSLOT(SpriteDescriptor.mPosition.mZ, PositionZ, positionZ, int, int);

SETSLOT(SpriteDescriptor.mRotation.mX, RotationX, rotationX, int, int);
SETSLOT(SpriteDescriptor.mRotation.mY, RotationY, rotationY, int, int);
SETSLOT(SpriteDescriptor.mRotation.mZ, RotationZ, rotationZ, int, int);

SETSLOT(SpriteDescriptor.mScale.mX, ScaleX, scaleX, double, float);
SETSLOT(SpriteDescriptor.mScale.mY, ScaleY, scaleY, double, float);

SETSLOT(SpriteDescriptor.mAlpha, Alpha, alpha, double, float);

void CelModel::setCenterX(int value)
{
    if (mpKeyFrameData)
    {
        int delta = (int)mpKeyFrameData->mSpriteDescriptor.mCenter.mX - value;
         if(delta != 0)
        {
            mpKeyFrameData->mSpriteDescriptor.mCenter.mX = value;
            mpKeyFrameData->mSpriteDescriptor.mPosition.mX -= delta;
            emit centerXChanged(value);
            emit positionXChanged((int)mpKeyFrameData->mSpriteDescriptor.mPosition.mX);
        }
     }
}

void CelModel::setCenterY(int value)
{
    int delta = (int)mpKeyFrameData->mSpriteDescriptor.mCenter.mY - value;
     if(delta != 0)
    {
        mpKeyFrameData->mSpriteDescriptor.mCenter.mY = value;
        mpKeyFrameData->mSpriteDescriptor.mPosition.mY -= delta;
        emit centerYChanged(value);
        emit positionYChanged((int)mpKeyFrameData->mSpriteDescriptor.mPosition.mY);
    }
}

void CelModel::setCenterZ(int value)
{
    int delta = (int)mpKeyFrameData->mSpriteDescriptor.mCenter.mZ != value;
     if(delta != 0)
    {
        mpKeyFrameData->mSpriteDescriptor.mCenter.mZ = value;
        emit centerZChanged(value);
    }
}

void CelModel::setSourceTexture(QString path, GLSprite::Rect rect)
{
    if (mpKeyFrameData->mSpriteDescriptor.mSourcePath != path || mpKeyFrameData->mSpriteDescriptor.mTextureSrcRect != rect)
    {
        mpKeyFrameData->mSpriteDescriptor.mSourcePath = path;
        mpKeyFrameData->mSpriteDescriptor.mTextureSrcRect = rect;

        emit sourceTextureChanged(path, rect);
    }
}
