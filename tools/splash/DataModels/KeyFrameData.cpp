#include "KeyFrameData.h"
#include "GLSprite.h"


QString KeyFrameData::swapTargetTypeString[KeyFrameData::SwapTargetType_COUNT] =
{
    "none",
    "character",
    "weapon"
};

KeyFrameData::SwapTargetType KeyFrameData::getSwapTargetTypeByString(QString typeString)
{
    for (int i = 0; i < SwapTargetType_COUNT; i++)
    {
        if (typeString == swapTargetTypeString[i])
        {
            return static_cast<KeyFrameData::SwapTargetType>(i);
        }
    }
    return SwapTargetType_None;
}

QString KeyFrameData::positionTypeOptionString[KeyFrameData::PositionTypeOption_COUNT] =
{
    "top_left",
    "top_center",
    "top_right",

    "center_left",
    "center",
    "center_right",

    "bottom_left",
    "bottom_center",
    "bottom_right"
};

KeyFrameData::PositionTypeOption KeyFrameData::getPositionTypeOptionByString(QString typeString)
{
    for (int i = 0; i < PositionTypeOption_COUNT; i++)
    {
        if (typeString == positionTypeOptionString[i])
        {
            return static_cast<KeyFrameData::PositionTypeOption>(i);
        }
    }
    return PositionTypeOption_Center;
}


QString KeyFrameData::positionTypeString[KeyFrameData::PositionType_COUNT] =
{
    "none",
    "relativeToTarget",
    "relativeToTargetOrigin"
};

KeyFrameData::PositionType KeyFrameData::getPositionTypeByString(QString typeString)
{
    for (int i = 0; i < PositionType_COUNT; i++)
    {
        if (typeString == positionTypeString[i])
        {
            return static_cast<KeyFrameData::PositionType>(i);
        }
    }
    return PositionType_None;
}

QString KeyFrameData::tweenAttributeSting[KeyFrameData::TweenAttribute_COUNT] =
{
    "positionTween",
    "rotationTween",
    "scaleTween",
    "alphaTween",
    "colorTween"
};

QString KeyFrameData::tweenTypeSting[KeyFrameData::eTT_COUNT] =
{
    "none",
    "fix",
    "linear",
    "easeIn",
    "easeOut",
    "easeInOut"
};

KeyFrameData::TweenType KeyFrameData::getTweenTypeByString(QString typeString)
{
    for (int i = 0; i < eTT_COUNT; i++)
    {
        if (typeString == tweenTypeSting[i])
        {
            return static_cast<KeyFrameData::TweenType>(i);
        }
    }
    return eTT_None;
}

KeyFrameData::KeyFrameData()
{
    mSpriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();

    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        mTweenTypes[i] = eTT_None;
    }
    mSwapTargetType = SwapTargetType_None;
    mHideActor = false;
}

void KeyFrameData::copyAttribute(KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData* item)
{
    switch(tweenAttribute)
    {
        case TweenAttribute_position:
            mSpriteDescriptor.mPosition = item->mSpriteDescriptor.mPosition;
        break;
        case TweenAttribute_rotation:
            mSpriteDescriptor.mRotation = item->mSpriteDescriptor.mRotation;
        break;
        case TweenAttribute_scale:
            mSpriteDescriptor.mScale = item->mSpriteDescriptor.mScale;
        break;
        case TweenAttribute_alpha:
            mSpriteDescriptor.mAlpha = item->mSpriteDescriptor.mAlpha;
        break;
        case TweenAttribute_color:
            mSpriteDescriptor.mColor = item->mSpriteDescriptor.mColor;
        break;
        default:
        break;
    }
}

bool KeyFrameData::hasTween() const
{
    bool tween = false;
    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        if (mTweenTypes[i] != eTT_None && mTweenTypes[i] != eTT_Fix)
        {
            tween = true;
        }
    }

    return tween;
}

bool KeyFrameData::allAttributesNone() const
{
    bool allNone = true;
    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        if (mTweenTypes[i] != eTT_None)
        {
            allNone = false;
        }
    }

    return allNone;
}
