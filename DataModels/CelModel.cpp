#include "CelModel.h"

CelModel::CelModel()
:   mpCelData(NULL)
{
}

CelModel::CelData* CelModel::getCelDataRef() const
{
    return mpCelData;
}

void CelModel::setCelDataRef(CelModel::CelData* pCelData)
{
    mpCelData = pCelData;
}

// Make our life easier by defining a macro here.
// This will generate a function
// 1: receive a value
// 2: compare it with current value
// 3: store value and emit signal if value changed
#define SETSLOT(name1, name2, type1, type2)\
void CelModel::set##name1(type1 value)\
{\
 if (mpCelData && mpCelData->m##name1 != value) {\
        mpCelData->m##name1 = (type2)value;\
        emit name2##Changed(value);\
    }\
}

// The same functionality with an additional argument
#define SETSLOT_STRUCT(name0, name1, name2, type1, type2)\
void CelModel::set##name1(type1 value)\
{\
 if (mpCelData && mpCelData->m##name0 != value) {\
        mpCelData->m##name0 = (type2)value;\
        emit name2##Changed(value);\
    }\
}

// Define all slots
SETSLOT(TextureID, textureID, int, int);

SETSLOT(RelativeToTarget, relativeToTarget, bool, bool);
SETSLOT(LookAtTarget, lookAtTarget, bool, bool);
SETSLOT(Blur, blur, bool, bool);

SETSLOT_STRUCT(TweenTypes[CelModel::TweenAttribute_position], PositionTweenType, positionTweenType, int, TweenType);
SETSLOT_STRUCT(TweenTypes[CelModel::TweenAttribute_rotation], RotationTweenType, rotationTweenType, int, TweenType);
SETSLOT_STRUCT(TweenTypes[CelModel::TweenAttribute_scale], ScaleTweenType, scaleTweenType, int, TweenType);
SETSLOT_STRUCT(TweenTypes[CelModel::TweenAttribute_alpha], AlphaTweenType, alphaTweenType,  int, TweenType);

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


