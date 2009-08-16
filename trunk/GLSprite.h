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
        float mX;
        float mY;
        float mWidth;
        float mHeight;

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
        Rect mTextureSrcRect;
        BlendType mBlendType;

        Point3 mCenter;
        Point2 mScale;
        Point3 mPosition;
        Point3 mRotation;

        float mAlpha;

        void operator=(SpriteDescriptor spriteDescriptor)
        {
            mAlpha = spriteDescriptor.mAlpha;
            mTextureSrcRect = spriteDescriptor.mTextureSrcRect;
            mBlendType = spriteDescriptor.mBlendType;
            mCenter = spriteDescriptor.mCenter;
            mScale = spriteDescriptor.mScale;
            mPosition = spriteDescriptor.mPosition;
            mRotation = spriteDescriptor.mRotation;
        }
    };

    GLSprite(const int& id,  QPixmap* pixmap, const SpriteDescriptor& spriteDescriptor, bool selectable);
    bool isSelectable() const;
    void render(QPoint renderCenterPoint, QPainter& painter);
    QRect getRect() const;
    bool contains(QPoint point) const;

    int mID;
    SpriteDescriptor mSpriteDescriptor;

private:
    QPixmap* mpPixmap;
    bool mIsSelectable;
};


static inline GLSprite::SpriteDescriptor makeDefaultSpriteDescriptor()
{
    GLSprite::SpriteDescriptor spriteDescriptor;
    spriteDescriptor.mBlendType = GLSprite::eBT_Alpha;

    spriteDescriptor.mCenter.mX = 0;
    spriteDescriptor.mCenter.mY = 0;
    spriteDescriptor.mCenter.mZ = 0;

    spriteDescriptor.mAlpha = 1.0;

    spriteDescriptor.mPosition.mX = 0;
    spriteDescriptor.mPosition.mY = 0;
    spriteDescriptor.mPosition.mZ = 0;

    spriteDescriptor.mRotation.mX = 0;
    spriteDescriptor.mRotation.mY = 0;
    spriteDescriptor.mRotation.mZ = 0;

    spriteDescriptor.mScale.mX = 1.0f;
    spriteDescriptor.mScale.mY = 1.0f;
    return spriteDescriptor;
}

#endif // GLSPRITE_H
