#ifndef GLSPRITE_H
#define GLSPRITE_H
#include "qgl.h"
#include <QMatrix>
#include "DataModels/KeyFrame.h"
#include "ResourceManager.h"
#include "EmittedAnimation.h"

class QPainter;
class QPoint;
class AnimationModel;
class QPixmap;
class GLSprite
{
public:

    enum FacingOptionType
    {
        FacingOptionType_none,
        FacingOptionType_lookAtTarget,
        FacingOptionType_FaceToMovingDir,

        FacingOptionType_COUNT
    };

    enum BlendType
    {
        eBT_Alpha,
        eBT_Add,
        eBT_Sub,

        eBT_COUNT
    };

    struct Point2
    {
        float mX;
        float mY;

        void operator=(const Point2& point)
        {
            mX = point.mX;
            mY = point.mY;
        }

        bool operator==(const QPoint& point)
        {
            return mX == point.x() && mY == point.y();
        }

        bool operator!=(const QPoint& point)
        {
            return !(mX == point.x() && mY == point.y());
        }
    };

    struct Color
    {
        float mR;
        float mG;
        float mB;

        void operator=(const Color& color)
        {
            mR = color.mR;
            mG = color.mG;
            mB = color.mB;
        }

        bool operator==(const Color& color)
        {
            return  (mR == color.mR &&
                    mG == color.mG &&
                    mB == color.mB);
        }

        bool operator!=(const Color& color)
        {
            return  !(mR == color.mR &&
                    mG == color.mG &&
                    mB == color.mB);
        }
     
        Color ()
        {
            mR = 0;
            mG = 0;
            mB = 0;
        }
        
        Color (float r, float g, float b)
        {
            mR = r;
            mG = g;
            mB = b;
        }
    };

    struct SpriteDescriptor
    {
        QString mSourcePath;

        bool mRelativeToTarget;
        FacingOptionType mFacingOptionType;
        int mBlur;
        QRect mTextureSrcRect; // only valid when it is not a child animation
        BlendType mBlendType;

        Point2 mCenter;
        Point2 mScale;
        Point2 mPosition;
        int mRotation;

        Color mColor;

        float mAlpha;
        float mPriority;

        // Sub animation data
        int mFrameNo; // frameNo for sub animation

        // tell whether this is an emitter of subanimation (valid only this sprite is sub animation
        bool mEmitter;

        void operator=(SpriteDescriptor spriteDescriptor)
        {
            mEmitter = spriteDescriptor.mEmitter;
            mSourcePath = spriteDescriptor.mSourcePath;
            mRelativeToTarget = spriteDescriptor.mRelativeToTarget;
            mFacingOptionType = spriteDescriptor.mFacingOptionType;
            mBlur = spriteDescriptor.mBlur;
            mAlpha = spriteDescriptor.mAlpha;
            mPriority = spriteDescriptor.mPriority;
            mTextureSrcRect = spriteDescriptor.mTextureSrcRect;
            mBlendType = spriteDescriptor.mBlendType;
            mCenter = spriteDescriptor.mCenter;
            mScale = spriteDescriptor.mScale;
            mPosition = spriteDescriptor.mPosition;
            mRotation = spriteDescriptor.mRotation;
            mColor = spriteDescriptor.mColor;
        }

        QTransform mOptionalTransform;
        QTransform getTransform() const
        {
            QTransform transform;
            transform.translate(mPosition.mX, mPosition.mY);
            transform.rotate(mRotation);
            transform.scale(mScale.mX, mScale.mY);

            return mOptionalTransform * transform;
        }

        bool isImage() const
        {
            return ResourceManager::getFileType(mSourcePath) == ResourceManager::FileType_Image;
        }

        QPoint textureCenter() const
        {
            return QPoint(mTextureSrcRect.width()/2, mTextureSrcRect.height()/2);
        }

        QPoint center() const
        {
            return QPointF(textureCenter().x() + mCenter.mX, textureCenter().y() + mCenter.mY).toPoint();
        }
    };

    static QString blendTypeSting[eBT_COUNT];
    static BlendType getBlendTypeByString(QString typeString) ;
    static QString facingOptionTypeSting[eBT_COUNT];
    static FacingOptionType getFacingOptionTypeByString(QString typeString) ;
    static SpriteDescriptor makeDefaultSpriteDescriptor();
    static bool priorityLessThan(const GLSprite* pItem1, const GLSprite* pItem2);

    GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id,  const SpriteDescriptor& spriteDescriptor, bool selectable, int lineNo, int frameNo, bool emitted);
    GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap);

    const AnimationModel* getParentAnimationModel() const;
    const GLSprite* getParentSprite() const;
    const GLSprite* getRootSprite() const;
    float getAbsoluteAlpha() const;
    Color getAbsoluteColor() const;

    QTransform getTransform() const;
    bool isEmitted() const;
    QList<KeyFrame::KeyFramePosition> getNodePath() const;

    bool isSelectable() const;
    void render(QPoint offset, QPainter& painter, const GLSprite* pTargetSprite, bool isPlaying, QList<EmittedAnimation*>* emittedAnimationList) const;
    QRect getRect() const;
    bool contains(QPoint point) const;
    QTransform getCombinedTransform() const;
    QTransform getParentTransform() const;

    int mID;
    SpriteDescriptor mSpriteDescriptor;

    const int mLineNo;
    const int mFrameNo;

private:

    const GLSprite* mpParentGLSprite;
    const AnimationModel* mpParentAnimationModel;

    bool mIsSelectable;
    QPixmap* mpPixmap;
    QTransform mParentTransform;
    bool mEmitted;
};
#endif // GLSPRITE_H
