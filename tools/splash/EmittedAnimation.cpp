#include "EmittedAnimation.h"
#include "GLSprite.h"
#include "DataModels/AnimationModel.h"
#include <QTransform>

EmittedAnimation::EmittedAnimation(AnimationModel* pAnimationModel, const GLSprite* pParentGLSprite)
        : mpAnimationModel(pAnimationModel),
          mCounter(0)
{
    GLSprite::SpriteDescriptor spriteDescriptor = pParentGLSprite->mSpriteDescriptor;

    // Kinda hacky. I couldn't decompose transformation to each attribute
    spriteDescriptor.mOptionalTransform = pParentGLSprite->getParentTransform();

    // This will emit under world coordinate, therefore the priority needs to be come from its root
    spriteDescriptor.mPriority = pParentGLSprite->getRootSprite()->mSpriteDescriptor.mPriority;
    spriteDescriptor.mAlpha = pParentGLSprite->getAbsoluteAlpha() * spriteDescriptor.mAlpha;

    spriteDescriptor.mEmitter = false;
    mpGLSprite = new GLSprite(NULL, pAnimationModel, 0, spriteDescriptor, false, 0, 0);
}

EmittedAnimation::~EmittedAnimation()
{
    delete mpGLSprite;
}

void EmittedAnimation::update()
{
    if (mCounter < mpGLSprite->getParentAnimationModel()->getMaxFrameCount())
    {
        mpGLSprite->mSpriteDescriptor.mFrameNo = mCounter;
        mCounter++;
    }
}

const GLSprite* EmittedAnimation::getSprite() const
{
    return mpGLSprite;
}

bool EmittedAnimation::isDone() const
{
    return (mCounter >= mpGLSprite->getParentAnimationModel()->getMaxFrameCount());
}
