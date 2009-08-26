#include "KeyFrameData.h"
#include "GLSprite.h"

KeyFrameData::KeyFrameData()
{
    mSpriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();

    for (int i = 0; i < TweenAttribute_COUNT; i++)
    {
        mTweenTypes[i] = eTT_Fix;
    }
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
