#include "GLSprite.h"

#include <QPixmap>
#include <QPainter>

GLSprite::SpriteDescriptor GLSprite::makeDefaultSpriteDescriptor()
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

GLSprite::GLSprite(const int& id, QPixmap* pixmap, const SpriteDescriptor& spriteDescriptor, bool selectable)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mpPixmap(pixmap),
          mIsSelectable(selectable)

{
}

bool GLSprite::isSelectable() const
{
    return mIsSelectable;
}

void GLSprite::render(QPoint renderCenterPoint, QPainter& painter)
{
    switch (mSpriteDescriptor.mBlendType)
    {
        default:
        case GLSprite::eBT_Alpha:
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            break;

        case GLSprite::eBT_Add:
            painter.setCompositionMode(QPainter::CompositionMode_Plus);
            break;
    }
    int dx = (int)mSpriteDescriptor.mPosition.mX + renderCenterPoint.x() + (int)mSpriteDescriptor.mTextureSrcRect.mWidth / 2;
    int dy = (int)mSpriteDescriptor.mPosition.mY + renderCenterPoint.y() + (int)mSpriteDescriptor.mTextureSrcRect.mHeight / 2;
    painter.translate(dx, dy);
    painter.rotate(mSpriteDescriptor.mRotation.mX);
    painter.scale(mSpriteDescriptor.mScale.mX, mSpriteDescriptor.mScale.mY);
    painter.translate(-dx, -dy);

    QPoint dstPoint(
                    (int)mSpriteDescriptor.mPosition.mX,
                    (int)mSpriteDescriptor.mPosition.mY
                   );
    dstPoint += renderCenterPoint;
    QRect srcRect(
            (int)mSpriteDescriptor.mTextureSrcRect.mX,
            (int)mSpriteDescriptor.mTextureSrcRect.mY,
            (int)mSpriteDescriptor.mTextureSrcRect.mWidth,
            (int)mSpriteDescriptor.mTextureSrcRect.mHeight
    );
    painter.drawPixmap(dstPoint, *mpPixmap, srcRect);

    painter.resetTransform();
}

QRect GLSprite::getRect() const
{
    return QRect(
        (int)mSpriteDescriptor.mPosition.mX,
        (int)mSpriteDescriptor.mPosition.mY,
        (int)mSpriteDescriptor.mTextureSrcRect.mWidth,
        (int)mSpriteDescriptor.mTextureSrcRect.mHeight
    );
}
bool GLSprite::contains(QPoint point) const
{
    return getRect().contains(point, true);
}
