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

static QPainter::CompositionMode sCompositionMode[GLSprite::eBT_COUNT] =
{
    QPainter::CompositionMode_SourceOver,
    QPainter::CompositionMode_Plus,
    QPainter::CompositionMode_SourceOver
};

GLSprite::GLSprite(const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, int lineNo, int frameNo, const AnimationModel* pParentAnimationModel)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mLineNo(lineNo),
          mFrameNo(frameNo),
          mpParentAnimationModel(pParentAnimationModel),
          mIsSelectable(selectable),
          mpPixmap(NULL)

{
}

//
GLSprite::GLSprite(const int& id, const SpriteDescriptor& spriteDescriptor, bool selectable, QPixmap* pPixmap)
        : mID(id),
          mSpriteDescriptor(spriteDescriptor),
          mLineNo(0),
          mFrameNo(0),
          mpParentAnimationModel(NULL),
          mIsSelectable(selectable),
          mpPixmap(pPixmap)
{
}


QPoint GLSprite::getAbsolutePositionAt(int currentFrameNo, int currentLineNo, int nextFrameNo)
{
    QList<KeyFrame::KeyFramePosition> list;
    KeyFrame::KeyFramePosition position;
    position.mFrameNo = currentFrameNo;
    position.mLineNo = currentLineNo;
    list.push_front(position);

    const GLSprite* pRootSprite = this;
    const AnimationModel* pParentAnimationModel = mpParentAnimationModel;

    // get trace result
    while(true)
    {
        KeyFrame::KeyFramePosition position;
        position.mFrameNo = pRootSprite->mFrameNo;
        position.mLineNo = pRootSprite->mLineNo;
        list.push_front(position);

        const GLSprite* pTempSprite = pParentAnimationModel->getParentSprite();
        if (!pTempSprite) {break;}
        pRootSprite = pTempSprite;
    }

    // calculate absolute positions
    const AnimationModel* pAnimationModel = pRootSprite->mpParentAnimationModel;
    GLSprite::SpriteDescriptor spriteDescriptor = pRootSprite->mSpriteDescriptor;
    pAnimationModel->copyTweenedAttribute(spriteDescriptor, currentLineNo, nextFrameNo, KeyFrameData::TweenAttribute_position);
    int absX = spriteDescriptor.mPosition.mX;
    int absY = spriteDescriptor.mPosition.mY;

    for (int i = 1; i < list.count(); i++)
    {
        int index = pAnimationModel->getPreviousKeyFrameIndex(list[i].mLineNo, list[i].mFrameNo, KeyFrameData::TweenAttribute_any);
        KeyFrame* pKeyFrame = pAnimationModel->getKeyFrameList(list[i].mLineNo).at(index);

        GLSprite::SpriteDescriptor spriteDescriptor = pKeyFrame->mpKeyFrameData->mSpriteDescriptor;
        pAnimationModel = ResourceManager::getAnimation(spriteDescriptor.mSourcePath, this);
        if (!pAnimationModel) {break;}

        pAnimationModel->copyTweenedAttribute(spriteDescriptor, list[i].mLineNo, list[i].mFrameNo, KeyFrameData::TweenAttribute_position);
        absX += spriteDescriptor.mPosition.mX;
        absY += spriteDescriptor.mPosition.mY;
    }

    return QPoint(absX, absY);
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
    painter.setCompositionMode(sCompositionMode[mSpriteDescriptor.mBlendType]);

   // Rotation & Scale & translate
    QTransform saveTransform = painter.combinedTransform();
    painter.setMatrix(getTransformationMatrix(), true);
//    QMatrix offSetMatrix = QMatrix();
//    offSetMatrix.translate(320, 240);
//    painter.setMatrix((getWorldMatrix()).inverted(), false);

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
                AnimationModel* pAnimationModel = ResourceManager::getAnimation(sourcePath, this);
                if (pAnimationModel)
                {
                    if (pAnimationModel->getMaxFrameCount() > 0)
                    {
                        int subFrameNo = mSpriteDescriptor.mFrameNo % pAnimationModel->getMaxFrameCount();
                        QList<GLSprite*> glSpriteList = pAnimationModel->createGLSpriteListAt(subFrameNo);
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

const GLSprite* GLSprite::getRootSprite() const
{
    const GLSprite* pRootSprite = this;
    while (const GLSprite* pSprite = this->mpParentAnimationModel->getParentSprite())
    {
        pRootSprite = pSprite;
    }
    return pRootSprite;
}

QMatrix GLSprite::getTransformationMatrix() const
{
    QMatrix matrix = QMatrix();
    matrix.translate(mSpriteDescriptor.mPosition.mX, mSpriteDescriptor.mPosition.mY);
    matrix.rotate(mSpriteDescriptor.mRotation.mX);
    matrix.scale(mSpriteDescriptor.mScale.mX, mSpriteDescriptor.mScale.mY);

    return matrix;
}

QMatrix GLSprite::getWorldMatrix() const
{
    QMatrix matrix = getTransformationMatrix();
    if (mpParentAnimationModel)
    {
        const AnimationModel* pAnimationModel = mpParentAnimationModel;
        while(const GLSprite* pSprite = pAnimationModel->getParentSprite())
        {
            matrix *= pSprite->getTransformationMatrix();
            pAnimationModel = pSprite->mpParentAnimationModel;
        }
    }

    QMatrix offSetMatrix = QMatrix();

    return matrix * offSetMatrix;
}

QList<KeyFrame::KeyFramePosition> GLSprite::getNodePath() const
{
    QList<KeyFrame::KeyFramePosition> list;
    list.push_front(KeyFrame::KeyFramePosition(mLineNo, mFrameNo));
    if (mpParentAnimationModel)
    {
        const AnimationModel* pAnimationModel = mpParentAnimationModel;
        while(const GLSprite* pSprite = pAnimationModel->getParentSprite())
        {
            list.push_front(KeyFrame::KeyFramePosition(pSprite->mLineNo, pSprite->mFrameNo));
            pAnimationModel = pSprite->mpParentAnimationModel;
        }
    }

    return list;
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
