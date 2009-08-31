#include "GLSprite.h"

#include <math.h>
#include <QPixmap>
#include <QPainter>
#include "DataModels/AnimationModel.h"
#include "ResourceManager.h"

QString GLSprite::blendTypeSting[GLSprite::eBT_COUNT] =
{
    "alpha",
    "add",
    "sub"
};

GLSprite::BlendType GLSprite::getBlendTypeByString(QString typeString)
{
    for (int i = 0; i < eBT_COUNT; i++)
    {
        if (typeString == blendTypeSting[i])
        {
            return static_cast<GLSprite::BlendType>(i);;
        }
    }
    return eBT_Alpha;
}


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
    spriteDescriptor.mFrameNo = 0;

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

GLSprite::GLSprite(const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mIsSelectable(selectable),
          mpPixmap(NULL)

{
}

//
GLSprite::GLSprite(const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mIsSelectable(selectable),
          mpPixmap(pPixmap)

{
}

bool GLSprite::isSelectable() const
{
    return mIsSelectable;
}
void GLSprite::render(QPainter& painter, GLSprite* pParentSprite, GLSprite* pTargetSprite)
{
    QPoint spritePosition = QPoint(mSpriteDescriptor.mPosition.mX, mSpriteDescriptor.mPosition.mY);
    QPoint spriteRenderPoint = spritePosition;

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

    // Rotation & Scale & translate
    QTransform saveTransform = painter.combinedTransform();
    painter.translate(spriteRenderPoint.x(), spriteRenderPoint.y());
    painter.rotate(mSpriteDescriptor.mRotation.mX);
    painter.scale(mSpriteDescriptor.mScale.mX, mSpriteDescriptor.mScale.mY);

    // Get source texture rectangle (Not used for sub animation)
    QRect srcRect(
            (int)mSpriteDescriptor.mTextureSrcRect.mX,
            (int)mSpriteDescriptor.mTextureSrcRect.mY,
            (int)mSpriteDescriptor.mTextureSrcRect.mWidth,
            (int)mSpriteDescriptor.mTextureSrcRect.mHeight
    );

    // Where it actually render the iamge
    QPoint dstPoint = QPoint(-mSpriteDescriptor.mCenter.mX, -mSpriteDescriptor.mCenter.mY);

    QPixmap* pQPixmap;
    if (mpPixmap)
    {
        pQPixmap = mpPixmap;
        if (pQPixmap) {painter.drawPixmap(dstPoint, *pQPixmap, srcRect);}
    }
    else
    {
         QString sourcePath = mSpriteDescriptor.mSourcePath;
         switch(ResourceManager::getFileType(sourcePath))
         {
            case ResourceManager::FileType_Image:
                pQPixmap = AnimationModel::getPixmap(mSpriteDescriptor.mSourcePath);
                painter.drawPixmap(dstPoint, *pQPixmap, srcRect);
                break;
            case ResourceManager::FileType_Animation:
                // Render Subanimation
                AnimationModel* pAnimationModel = ResourceManager::getAnimation(sourcePath);
                if (pAnimationModel)
                {
                    if (pAnimationModel->getMaxFrameCount() > 0)
                    {
                        int subFrameNo = mSpriteDescriptor.mFrameNo % pAnimationModel->getMaxFrameCount();
                        QList<GLSprite*> glSpriteList = pAnimationModel->createGLSpriteListAt(subFrameNo, this);
                        for (int i = 0; i < glSpriteList.count(); i++)
                        {
                            glSpriteList[i]->render(painter, this, pTargetSprite);
                        }
                        while (!glSpriteList.empty()) { delete glSpriteList.takeFirst();}
                    }
                }
                break;
             default:

             break;
         }
    }

    painter.setTransform(saveTransform, false);
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
    point += QPoint((int)(mSpriteDescriptor.mCenter.mX), (int)(mSpriteDescriptor.mCenter.mY));
    return getRect().contains(point, true);
}
