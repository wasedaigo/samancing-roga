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
#define SETSLOT(name1, name2, type1, type2)\
void CelModel::set##name1(type1 value)\
{\
 if (mpKeyFrameData && mpKeyFrameData->m##name1 != value) {\
        mpKeyFrameData->m##name1 = (type2)value;\
        emit name2##Changed(value);\
    }\
}

// The same functionality with an additional argument
#define SETSLOT_STRUCT(name0, name1, name2, type1, type2)\
void CelModel::set##name1(type1 value)\
{\
 if (mpKeyFrameData && mpKeyFrameData->m##name0 != value) {\
        mpKeyFrameData->m##name0 = (type2)value;\
        emit name2##Changed(value);\
    }\
}

// Define all slots
SETSLOT(TextureID, textureID, int, int);

SETSLOT_STRUCT(TweenTypes[KeyFrameData::TweenAttribute_position], PositionTweenType, positionTweenType, int, KeyFrameData::TweenType);
SETSLOT_STRUCT(TweenTypes[KeyFrameData::TweenAttribute_rotation], RotationTweenType, rotationTweenType, int, KeyFrameData::TweenType);
SETSLOT_STRUCT(TweenTypes[KeyFrameData::TweenAttribute_scale], ScaleTweenType, scaleTweenType, int, KeyFrameData::TweenType);
SETSLOT_STRUCT(TweenTypes[KeyFrameData::TweenAttribute_alpha], AlphaTweenType, alphaTweenType,  int, KeyFrameData::TweenType);

SETSLOT_STRUCT(SpriteDescriptor.mRelativeToTarget, RelativeToTarget, relativeToTarget, bool, bool);
SETSLOT_STRUCT(SpriteDescriptor.mLookAtTarget, LookAtTarget, lookAtTarget, bool, bool);
SETSLOT_STRUCT(SpriteDescriptor.mBlur, Blur, blur, int, int);

SETSLOT_STRUCT(SpriteDescriptor.mTextureSrcRect, TextureSrcRect, textureSrcRect, GLSprite::Rect, GLSprite::Rect);
SETSLOT_STRUCT(SpriteDescriptor.mBlendType, BlendType, blendType, int, GLSprite::BlendType);
SETSLOT_STRUCT(SpriteDescriptor.mCenter.mX, CenterX, centerX, int, int);
SETSLOT_STRUCT(SpriteDescriptor.mCenter.mY, CenterY, centerY, int, int);
SETSLOT_STRUCT(SpriteDescriptor.mCenter.mZ, CenterZ, centerZ, int, int);

SETSLOT_STRUCT(SpriteDescriptor.mPosition.mX, PositionX, positionX, int, int);
SETSLOT_STRUCT(SpriteDescriptor.mPosition.mY, PositionY, positionY, int, int);
SETSLOT_STRUCT(SpriteDescriptor.mPosition.mZ, PositionZ, positionZ, int, int);

SETSLOT_STRUCT(SpriteDescriptor.mRotation.mX, RotationX, rotationX, int, int);
SETSLOT_STRUCT(SpriteDescriptor.mRotation.mY, RotationY, rotationY, int, int);
SETSLOT_STRUCT(SpriteDescriptor.mRotation.mZ, RotationZ, rotationZ, int, int);

SETSLOT_STRUCT(SpriteDescriptor.mScale.mX, ScaleX, scaleX, double, float);
SETSLOT_STRUCT(SpriteDescriptor.mScale.mY, ScaleY, scaleY, double, float);

SETSLOT_STRUCT(SpriteDescriptor.mAlpha, Alpha, alpha, double, float);


