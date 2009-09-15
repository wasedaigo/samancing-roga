#ifndef BaseKEYFRAME_H
#define BaseKEYFRAME_H

#include "GLSprite.h"
class KeyFrameData
{
public:   
    enum TweenType
    {
        eTT_None,
        eTT_Fix,
        eTT_Linear,
        eTT_EaseIn,
        eTT_EaseOut,
        eTT_EaseInOut,

        eTT_COUNT
    };

    enum TweenAttribute
    {
      TweenAttribute_any = -1,
      TweenAttribute_position = 0,
      TweenAttribute_rotation,
      TweenAttribute_scale,
      TweenAttribute_alpha,
      TweenAttribute_color,

      TweenAttribute_COUNT
    };

    KeyFrameData();

    GLSprite::SpriteDescriptor mSpriteDescriptor;
    TweenType mTweenTypes[TweenAttribute_COUNT];
    void copyAttribute(TweenAttribute tweenAttribute, KeyFrameData* item);
    bool hasTween() const;
    bool allAttributesNone() const;

    static QString tweenAttributeSting[TweenAttribute_COUNT];
    static QString tweenTypeSting[eTT_COUNT];
    static TweenType getTweenTypeByString(QString typeString);
};

#endif // KEYFRAMEDATA_H
