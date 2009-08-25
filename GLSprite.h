#ifndef GLSPRITE_H
#define GLSPRITE_H
#include "qgl.h"


class QPainter;
class QPoint;
class QPixmap;
class GLSprite
{
public:
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
            mX = point.mX;
            mY = point.mY;
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
        bool mRelativeToTarget;
        bool mLookAtTarget;
        int mBlur;

        Rect mTextureSrcRect;
        BlendType mBlendType;

        Point3 mCenter;
        Point2 mScale;
        Point3 mPosition;
        Point3 mRotation;

        float mAlpha;

        void operator=(SpriteDescriptor spriteDescriptor)
        {
            mRelativeToTarget = spriteDescriptor.mRelativeToTarget;
            mLookAtTarget = spriteDescriptor.mLookAtTarget;
            mBlur = spriteDescriptor.mBlur;
            mAlpha = spriteDescriptor.mAlpha;
            mTextureSrcRect = spriteDescriptor.mTextureSrcRect;
            mBlendType = spriteDescriptor.mBlendType;
            mCenter = spriteDescriptor.mCenter;
            mScale = spriteDescriptor.mScale;
            mPosition = spriteDescriptor.mPosition;
            mRotation = spriteDescriptor.mRotation;
        }
    };

    static SpriteDescriptor makeDefaultSpriteDescriptor();
    GLSprite(const int& id,  QPixmap* pixmap, const SpriteDescriptor& spriteDescriptor, bool selectable);

    bool isSelectable() const;
    void render(QPoint renderCenterPoint, QPainter& painter, GLSprite* pTargetSprite);
    QRect getRect() const;
    bool contains(QPoint point, const GLSprite::Point3& targetPosition) const;

    int mID;
    SpriteDescriptor mSpriteDescriptor;

private:
    QPixmap* mpPixmap;
    bool mIsSelectable;
};

#endif // GLSPRITE_H
