#ifndef GLSPRITE_H
#define GLSPRITE_H
#include "qgl.h"
#include <QMatrix>
#include "DataModels/KeyFrame.h"

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
    };

    struct Point3
    {
        float mX;
        float mY;
        float mZ;

        void operator=(const Point2& point)
        {
            mX = static_cast<int>(point.mX);
            mY = static_cast<int>(point.mY);
            mZ = 0;
        }

        void operator=(const Point3& point)
        {
            mX = point.mX;
            mY = point.mY;
            mZ = point.mZ;
        }
    };

    struct Rect
    {
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        Rect(int x, int y, int width, int height)
        {
            mX = x;
            mY = y;
            mWidth = width;
            mHeight = height;
        }

        Rect()
        {
            mX = 0;
            mY = 0;
            mWidth = 0;
            mHeight = 0;
        }

        bool operator!=(Rect& rect)
        {
            return !(rect.mX == mX && rect.mY == mY && rect.mWidth == mWidth && rect.mHeight == mHeight);
        }

        void operator=(Rect rect)
        {
            mX = rect.mX;
            mY = rect.mY;
            mWidth = rect.mWidth;
            mHeight = rect.mHeight;
        }


    };

    struct SpriteDescriptor
    {
        QString mSourcePath;

        bool mRelativeToTarget;
        FacingOptionType mFacingOptionType;
        int mBlur;
        Rect mTextureSrcRect; // only valid when it is not a child animation
        BlendType mBlendType;

        Point3 mCenter;
        Point2 mScale;
        Point3 mPosition;
        Point3 mRotation;

        float mAlpha;

        // Sub animation data
        int mFrameNo; // frameNo for sub animation

        void operator=(SpriteDescriptor spriteDescriptor)
        {
            mSourcePath = spriteDescriptor.mSourcePath;
            mRelativeToTarget = spriteDescriptor.mRelativeToTarget;
            mFacingOptionType = spriteDescriptor.mFacingOptionType;
            mBlur = spriteDescriptor.mBlur;
            mAlpha = spriteDescriptor.mAlpha;
            mTextureSrcRect = spriteDescriptor.mTextureSrcRect;
            mBlendType = spriteDescriptor.mBlendType;
            mCenter = spriteDescriptor.mCenter;
            mScale = spriteDescriptor.mScale;
            mPosition = spriteDescriptor.mPosition;
            mRotation = spriteDescriptor.mRotation;
        }

        QTransform getTransform() const
        {
            QTransform transform;
            transform.translate(mPosition.mX, mPosition.mY);
            transform.rotate(mRotation.mX);
            transform.scale(mScale.mX, mScale.mY);

            return transform;
        }
    };

    static QString blendTypeSting[eBT_COUNT];
    static BlendType getBlendTypeByString(QString typeString) ;
    static QString facingOptionTypeSting[eBT_COUNT];
    static FacingOptionType getFacingOptionTypeByString(QString typeString) ;

    static SpriteDescriptor makeDefaultSpriteDescriptor();
    GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id,  const SpriteDescriptor& spriteDescriptor, bool selectable, int lineNo, int frameNo);
    GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap);

    const AnimationModel* getParentAnimationModel() const;
    const GLSprite* getParentSprite() const;
    const GLSprite* getRootSprite() const;
    QTransform getTransform() const;
    QMatrix getCombinedMatrix() const;
    QList<KeyFrame::KeyFramePosition> getNodePath() const;

    bool isSelectable() const;
    void render(QPoint offset, QPainter& painter, const GLSprite* pTargetSprite) const;
    QRect getRect() const;
    bool contains(QPoint point, const QPoint& targetPosition) const;
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
};

#endif // GLSPRITE_H
