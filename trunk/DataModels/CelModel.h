#ifndef CelModel_H
#define CelModel_H
#include <QObject>
#include "GLSprite.h"

class CelModel : public QObject
{
Q_OBJECT

public:
    enum TweenType
    {
        eTT_None,
        eTT_Fix,
        eTT_Linear,
        eTT_Spline,
        eTT_LinearEaseIn,
        eTT_SplineEaseIn,
        eTT_LinearEaseOut,
        eTT_SplineEaseOut,

        eTT_COUNT
    };

    enum TweenAttribute
    {
      TweenAttribute_position,
      TweenAttribute_rotation,
      TweenAttribute_scale,
      TweenAttribute_alpha,

      TweenAttribute_COUNT
    };
    
    struct CelData
    {
        int mTextureID;
        int mCelNo;
        bool mRelativeToTarget;
        bool mLookAtTarget;
        bool mBlur;
        bool mIsTweenCel;

        GLSprite::SpriteDescriptor mSpriteDescriptor;

        TweenType mTweenTypes[TweenAttribute_COUNT];

        bool operator==(const CelData& item)
        {
            return mCelNo == item.mCelNo;
        }

        void copyAttribute(TweenAttribute tweenAttribute, const CelData& item)
        {
            switch(tweenAttribute)
            {
                case TweenAttribute_position:
                    mSpriteDescriptor.mPosition = item.mSpriteDescriptor.mPosition;
                break;
                case TweenAttribute_rotation:
                    mSpriteDescriptor.mRotation = item.mSpriteDescriptor.mRotation;
                break;
                case TweenAttribute_scale:
                    mSpriteDescriptor.mScale = item.mSpriteDescriptor.mScale;
                break;
                case TweenAttribute_alpha:
                    mSpriteDescriptor.mAlpha = item.mSpriteDescriptor.mAlpha;
                break;
                default:
                break;
            }
        }
    };

    static inline bool hasTween(CelData& celData);
    static inline bool celNoLessThan(CelData& item1, CelData& item2);
    CelData* getCelDataRef() const;
    void setCelDataRef(CelData* pCelData);

    CelModel();

public slots:
    void setTextureID(int value);
    void setTextureSrcRect(GLSprite::Rect value);

    void setBlendType(int index);
    void setRelativeToTarget(bool value);
    void setLookAtTarget(bool value);
    void setBlur(bool value);

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
    void blurChanged(bool value);

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
    CelData* mpCelData;
};

inline bool CelModel::hasTween(CelData& celData)
{
    return !(
            celData.mTweenTypes[TweenAttribute_position] == eTT_None &&
            celData.mTweenTypes[TweenAttribute_scale] == eTT_None &&
            celData.mTweenTypes[TweenAttribute_rotation] == eTT_None &&
            celData.mTweenTypes[TweenAttribute_alpha] == eTT_None
            );
}

static inline CelModel::CelData makeDefaultCelData()
{
    CelModel::CelData celData;
    celData.mTextureID = 0;
    celData.mBlur = false;
    celData.mLookAtTarget = false;
    celData.mRelativeToTarget = false;
    celData.mCelNo = 0;
    celData.mIsTweenCel = false;

    celData.mSpriteDescriptor = makeDefaultSpriteDescriptor();

    for (int i = 0; i < CelModel::TweenAttribute_COUNT; i++)
    {
        celData.mTweenTypes[i] = CelModel::eTT_Fix;
    }

    return celData;
}

inline bool CelModel::celNoLessThan(CelData& item1, CelData& item2)
{
    return item1.mCelNo < item2.mCelNo;
}
#endif // CelModel_H
