#include "GLSprite.h"

#include <math.h>
#include <QPixmap>
#include <QPainter>
#include "DataModels/AnimationModel.h"
#include "DataModels/KeyFrame.h"
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

QString GLSprite::facingOptionTypeSting[GLSprite::FacingOptionType_COUNT] =
{
    "none",
    "lookAtTarget",
    "faceToMov"
};


GLSprite::FacingOptionType GLSprite::getFacingOptionTypeByString(QString typeString)
{
    for (int i = 0; i < eBT_COUNT; i++)
    {
        if (typeString == facingOptionTypeSting[i])
        {
            return static_cast<GLSprite::FacingOptionType>(i);
        }
    }
    return FacingOptionType_none;
}

GLSprite::SpriteDescriptor GLSprite::makeDefaultSpriteDescriptor()
{
    GLSprite::SpriteDescriptor spriteDescriptor;
    spriteDescriptor.mSourcePath = "";
    spriteDescriptor.mBlendType = GLSprite::eBT_Alpha;
    spriteDescriptor.mFacingOptionType = FacingOptionType_none;
    spriteDescriptor.mRelativeToTarget = false;
    spriteDescriptor.mBlur = 0;
    spriteDescriptor.mCenter.mX = 0;
    spriteDescriptor.mCenter.mY = 0;
    spriteDescriptor.mFrameNo = 0;

    spriteDescriptor.mAlpha = 1.0;

    spriteDescriptor.mPosition.mX = 0;
    spriteDescriptor.mPosition.mY = 0;

    spriteDescriptor.mRotation = 0;

    spriteDescriptor.mScale.mX = 1.0f;
    spriteDescriptor.mScale.mY = 1.0f;

    spriteDescriptor.mTextureSrcRect = QRect(0, 0, 0, 0);

    return spriteDescriptor;
}

static QPainter::CompositionMode sCompositionMode[GLSprite::eBT_COUNT] =
{
    QPainter::CompositionMode_SourceOver,
    QPainter::CompositionMode_Plus,
    QPainter::CompositionMode_SourceOver
};

GLSprite::GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, int lineNo, int frameNo)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mLineNo(lineNo),
          mFrameNo(frameNo),
          mpParentGLSprite(pGLSprite),
          mpParentAnimationModel(pAnimationModel),
          mIsSelectable(selectable),
          mpPixmap(NULL)

{
}

//
GLSprite::GLSprite(const GLSprite* pGLSprite, const AnimationModel* pAnimationModel, const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mLineNo(0),
          mFrameNo(0),
          mpParentGLSprite(pGLSprite),
          mpParentAnimationModel(pAnimationModel),
          mIsSelectable(selectable),
          mpPixmap(pPixmap)
{
}

QTransform GLSprite::getParentTransform() const
{
    QTransform transform;
    if (mpParentGLSprite)
    {
        transform = transform * mpParentGLSprite->getCombinedTransform();
    }
    return transform;
}

const GLSprite* GLSprite::getParentSprite() const
{
    return mpParentGLSprite;
}

const AnimationModel* GLSprite::getParentAnimationModel() const
{
    return mpParentAnimationModel;
}

bool GLSprite::isSelectable() const
{
    return mIsSelectable;
}

void GLSprite::render(QPoint offset, QPainter& painter, const GLSprite* pTargetSprite) const
{
    QPointF spritePosition = QPointF(mSpriteDescriptor.mPosition.mX, mSpriteDescriptor.mPosition.mY);
    QPointF spriteRenderPoint = spritePosition;

    // Choose Blend Type
    painter.setCompositionMode(sCompositionMode[mSpriteDescriptor.mBlendType]);

    // Alpha
    painter.setOpacity(mSpriteDescriptor.mAlpha);

   // Rotation & Scale & translate
    QTransform saveTransform = painter.combinedTransform();
    painter.setTransform(getTransform(), true);

    // Get source texture rectangle (Not used for sub animation)
    QRect srcRect = mSpriteDescriptor.mTextureSrcRect;

    // Where it actually render the iamge
    QPointF dstPoint = QPointF(-mSpriteDescriptor.mCenter.mX, -mSpriteDescriptor.mCenter.mY) - mSpriteDescriptor.textureCenter() + offset;
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
                        QList<const GLSprite*> glSpriteList = pAnimationModel->createGLSpriteListAt(this, subFrameNo);
                        for (int i = 0; i < glSpriteList.count(); i++)
                        {
                            glSpriteList[i]->render(QPoint(0, 0), painter, pTargetSprite);
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

const GLSprite* GLSprite::getRootSprite() const
{
    const GLSprite* pRootSprite = this;

    const GLSprite* pSprite = this;
    while (pSprite = pSprite->getParentSprite())
    {
        pRootSprite = pSprite;
    }
    return pRootSprite;
}

QTransform GLSprite::getTransform() const
{
    QTransform transform = QTransform();
    transform.translate(mSpriteDescriptor.mPosition.mX, mSpriteDescriptor.mPosition.mY);
    transform.rotate(mSpriteDescriptor.mRotation);
    transform.scale(mSpriteDescriptor.mScale.mX, mSpriteDescriptor.mScale.mY);

    return transform;
}

QTransform GLSprite::getCombinedTransform() const
{
    return getParentTransform() * getTransform();
}

QList<KeyFrame::KeyFramePosition> GLSprite::getNodePath() const
{
    QList<KeyFrame::KeyFramePosition> list;
    list.push_front(KeyFrame::KeyFramePosition(mLineNo, mFrameNo));

    const GLSprite* pSprite = this;
    while(pSprite = pSprite->getParentSprite())
    {
        list.push_front(KeyFrame::KeyFramePosition(pSprite->mLineNo, pSprite->mFrameNo));
    }

    return list;
}

QRect GLSprite::getRect() const
{
    return QRect(
        (int)mSpriteDescriptor.mPosition.mX,
        (int)mSpriteDescriptor.mPosition.mY,
        (int)mSpriteDescriptor.mTextureSrcRect.width(),
        (int)mSpriteDescriptor.mTextureSrcRect.height()
    );
}

bool GLSprite::contains(QPoint point) const
{
    point += QPoint((int)(mSpriteDescriptor.mCenter.mX), (int)(mSpriteDescriptor.mCenter.mY));
    point += mSpriteDescriptor.textureCenter();
    return getRect().contains(point, true);
}
