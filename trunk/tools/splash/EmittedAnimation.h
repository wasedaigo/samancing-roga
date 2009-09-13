#ifndef EMITTEDANIMATION_H
#define EMITTEDANIMATION_H
#include <QList>

class GLSprite;
class QPainter;
class AnimationModel;

class EmittedAnimation
{
public:
    EmittedAnimation(AnimationModel* pAnimationModel, const GLSprite* pParentGLSprite);
    ~EmittedAnimation();
    void update();
    const GLSprite* getSprite() const;
    bool isDone() const;
private:
    AnimationModel* mpAnimationModel;
    GLSprite* mpGLSprite;
    int mCounter;
};

#endif // EMITTEDANIMATION_H
