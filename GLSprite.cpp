#include "GLSprite.h"

#include <QPixmap>
#include <QPainter>

GLSprite::GLSprite(const int& id, QPixmap* pixmap, const SpriteDescriptor& spriteDescriptor)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mpPixmap(pixmap)

{
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
