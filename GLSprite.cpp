#include "GLSprite.h"

#include "Common.h"
#include <math.h>
#include <QPixmap>
#include <QPainter>
#include "DataModels/AnimationModel.h"

GLSprite::SpriteDescriptor GLSprite::makeDefaultSpriteDescriptor()
{
    GLSprite::SpriteDescriptor spriteDescriptor;
    spriteDescriptor.mSourcePath = "";
    spriteDescriptor.mBlendType = GLSprite::eBT_Alpha;
    spriteDescriptor.mLookAtTarget = false;
    spriteDescriptor.mRelativeToTarget = false;
    spriteDescriptor.mBlur = 0;
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

    spriteDescriptor.mTextureSrcRect.mX = 0;
    spriteDescriptor.mTextureSrcRect.mY = 0;
    spriteDescriptor.mTextureSrcRect.mWidth = 0;
    spriteDescriptor.mTextureSrcRect.mHeight = 0;

    return spriteDescriptor;
}

GLSprite::GLSprite(const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, int animationDepth, AnimationModel* pAnimationModel)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mIsSelectable(selectable),
          mAnimationDepth(animationDepth),
          mpAnimationModel(pAnimationModel),
          mpPixmap(NULL)

{
}

//
GLSprite::GLSprite(const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, int animationDepth, QPixmap* pPixmap)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mIsSelectable(selectable),
          mAnimationDepth(animationDepth),
          mpAnimationModel(NULL),
          mpPixmap(pPixmap)

{
}

bool GLSprite::isSelectable() const
{
    return mIsSelectable;
}

int GLSprite::getAnimationDepth() const
{
    return mAnimationDepth;
}

void GLSprite::render(QPoint renderCenterPoint, QPainter& painter, GLSprite* pTargetSprite)
{
    // Relative to target option
    QPoint positionOffset = QPoint(0, 0);
    if (mSpriteDescriptor.mRelativeToTarget)
    {
        positionOffset.setX((int)pTargetSprite->mSpriteDescriptor.mPosition.mX);
        positionOffset.setY((int)pTargetSprite->mSpriteDescriptor.mPosition.mY);
    }
    renderCenterPoint += positionOffset;

    // Look at target option
    float angleOffset = 0;
    if (mSpriteDescriptor.mLookAtTarget)
    {
        float dx = pTargetSprite->mSpriteDescriptor.mPosition.mX - mSpriteDescriptor.mPosition.mX - positionOffset.x();
        float dy = pTargetSprite->mSpriteDescriptor.mPosition.mY - mSpriteDescriptor.mPosition.mY - positionOffset.y();

        angleOffset = floor((180 * atan2(dy, dx)) / PI);
    }

    // Choose Blend Type
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

    // Rotation & Scale
    int dx = (int)mSpriteDescriptor.mPosition.mX + renderCenterPoint.x() + (int)mSpriteDescriptor.mTextureSrcRect.mWidth / 2;
    int dy = (int)mSpriteDescriptor.mPosition.mY + renderCenterPoint.y() + (int)mSpriteDescriptor.mTextureSrcRect.mHeight / 2;
    painter.translate(dx, dy);
    painter.rotate(mSpriteDescriptor.mRotation.mX + angleOffset);
    painter.scale(mSpriteDescriptor.mScale.mX, mSpriteDescriptor.mScale.mY);
    painter.translate(-dx, -dy);

    // Render
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

    QPixmap* pQPixmap;
    if (mpPixmap)
    {
        pQPixmap = mpPixmap;
    }
    else
    {
         pQPixmap = mpAnimationModel->getPixmap(mSpriteDescriptor.mSourcePath);
    }

    if (pQPixmap)
    {
        painter.drawPixmap(dstPoint, *pQPixmap, srcRect);
    }

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
bool GLSprite::contains(QPoint point, const GLSprite::Point3& targetPosition) const
{
    if (mSpriteDescriptor.mRelativeToTarget)
    {
        point -= QPoint((int)targetPosition.mX, (int)targetPosition.mY);
    }
    return getRect().contains(point, true);
}
