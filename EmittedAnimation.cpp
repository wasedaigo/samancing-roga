#include "EmittedAnimation.h"
#include "GLSprite.h"
#include "DataModels/AnimationModel.h"
#include <QTransform>

EmittedAnimation::EmittedAnimation(AnimationModel* pAnimationModel, const GLSprite* pParentGLSprite)
        : mpAnimationModel(pAnimationModel),
          mCounter(0)
{
    GLSprite::SpriteDescriptor spriteDescriptor = pParentGLSprite->mSpriteDescriptor;
    spriteDescriptor.mOptionalTransform = pParentGLSprite->getParentTransform();
    spriteDescriptor.mEmitter = false;
    mpGLSprite = new GLSprite(NULL, pAnimationModel, 0, spriteDescriptor, false, 0, 0);
}

EmittedAnimation::~EmittedAnimation()
{
    delete mpGLSprite;
}

void EmittedAnimation::update(QPainter& painter, QList<EmittedAnimation*>* emittedAnimationList)
{
    if (mCounter < mpGLSprite->getParentAnimationModel()->getMaxFrameCount())
    {
        mpGLSprite->mSpriteDescriptor.mFrameNo = mCounter;
        mpGLSprite->render(QPoint(0, 0), painter, AnimationModel::getTargetSprite(), true, emittedAnimationList);
        mCounter++;
    }
}

bool EmittedAnimation::isDone() const
{
    return (mCounter >= mpGLSprite->getParentAnimationModel()->getMaxFrameCount());
}
