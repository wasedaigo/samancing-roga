#include "AnimationModel.h"
#include <QPixmap>
#include <QMessageBox>
#include "ResourceManager.h"
#include "Macros.h"
#include "json/writer.h"
#include "json/reader.h"
#include <fstream>
#include <sstream>
#include "KeyFrameData.h"
#include <math.h>

#define PI 3.14159265358979323846
#define DEFAULT_SELECTION_RECT QRect(0, 0, 32, 32);

static bool sIsNesting = false;

// Macros
#define TweenValue(field, startValue, endValue, frameNo, startFrameNo, endFrameNo)\
field = LERP(startValue, endValue, frameNo, startFrameNo, endFrameNo)

#define Tween(tweenType, startValue, endValue, field, frameNo, startFrameNo, endFrameNo)\
{\
 switch (tweenType){\
    case KeyFrameData::eTT_Linear:\
        TweenValue(field, startValue, endValue, frameNo, startFrameNo, endFrameNo);\
    break;\
    case KeyFrameData::eTT_Fix:\
        TweenValue(field, startValue, startValue, frameNo, startFrameNo, endFrameNo);\
    break;\
    default:\
    break;\
    }\
}
////

static QHash<QString, QPixmap*> sSourceImageHash;
static QPixmap* spTargetPixmap = NULL;
static GLSprite* spTargetSprite = NULL;
static QPixmap* spCenterPointPixmap = NULL;
static GLSprite* spCenterPointSprite = NULL;

QPixmap* AnimationModel::getPixmap(QString path)
{
    if (!sSourceImageHash.contains(path))
    {
        QString rootPath = QDir::currentPath();
        QString fullPath = rootPath.append("/").append(ROOT_RESOURCE_DIR.path()).append(path);
        QFileInfo fileInfo = QFileInfo (fullPath);

        if (!fileInfo.isFile()) {return NULL;}
        try
        {
            sSourceImageHash.insert(path, new QPixmap(fullPath));
        }
        catch(char *str)
        {
            printf("File:%s couldn't be loaded.", str);
            return NULL;
        }
    };

    return sSourceImageHash[path];
}

GLSprite* AnimationModel::getTargetSprite()
{
    return spTargetSprite;
}

GLSprite* AnimationModel::getCenterPointSprite()
{
    return spCenterPointSprite;
}

AnimationModel::AnimationModel(QWidget* parent)
    : mpParent(parent),
      mAnimationName(QString("")),
      mAnimationDirectory(QString("")),
      mAnimationID(QString("")),
      mOriginalAnimationID(QString(""))
{
    setup();
}

void AnimationModel::setup()
{
    mSelectedSourcePath = "";
    mSelectedKeyFramePosition.mFrameNo = -1;
    mSelectedKeyFramePosition.mLineNo = -1;

    // Target sprite
    if (spTargetPixmap == NULL)
    {
        spTargetPixmap = new QPixmap(":/resource/target.png");
        GLSprite::SpriteDescriptor spriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();

        spriteDescriptor.mTextureSrcRect = DEFAULT_SELECTION_RECT;
        spriteDescriptor.mCenter.mX = 0;
        spriteDescriptor.mCenter.mY = 0;
        spriteDescriptor.mPosition.mX = -100;

        spTargetSprite = new GLSprite(NULL, NULL, -1, spriteDescriptor, false, spTargetPixmap);
    }

    // Center sprite
    if (spCenterPointPixmap == NULL)
    {
        spCenterPointPixmap = new QPixmap(":/resource/center_point.png");
        GLSprite::SpriteDescriptor spriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();
        spriteDescriptor.mTextureSrcRect.setWidth(8);
        spriteDescriptor.mTextureSrcRect.setHeight(8);
        spriteDescriptor.mCenter.mX = 0;
        spriteDescriptor.mCenter.mY = 0;

        spCenterPointSprite = new GLSprite(NULL, NULL, -1, spriteDescriptor, false, spCenterPointPixmap);
    }
}

AnimationModel::~AnimationModel()
{
}

void AnimationModel::setTargetSpritePosition(float x, float y)
{
    if (spTargetSprite->mSpriteDescriptor.mPosition.mX != x || spTargetSprite->mSpriteDescriptor.mPosition.mY != y)
    {
        spTargetSprite->mSpriteDescriptor.mPosition.mX = x;
        spTargetSprite->mSpriteDescriptor.mPosition.mY = y;

        emit targetPositionMoved((int)x, (int)y);
    }
}

void AnimationModel::setAnimationName(QString name)
{
    if (mAnimationName != name)
    {
        mAnimationName = name;
        emit animationNameChanged(name);
    }
}

void AnimationModel::setAnimationID(QString id)
{
    if (mAnimationID != id)
    {
        mAnimationID = id;
        emit animationIDChanged(id);
    }
}

void AnimationModel::clearPixmapHash()
{
    QHash<QString, QPixmap*>::Iterator iter = sSourceImageHash.begin();
    while(iter != sSourceImageHash.end())
    {
        delete iter.value();
        iter++;
    }
    sSourceImageHash.clear();
}

int AnimationModel::getMaxFrameCount(int lineNo) const
{
    int max = 0;
    if (mKeyFrames[lineNo].count() > 0)
    {
        max = mKeyFrames[lineNo].last()->mFrameNo + 1;
    }

    return max;
}

int AnimationModel::getMaxFrameCount() const
{
    int max = 0;
    for (int i = 0; i < MaxLineNo; i++)
    {
        int t = getMaxFrameCount(i);
        if (t > max)
        {
            max = t;
        }
    }

    return max;
}

AnimationModel::EventList AnimationModel::getEventList(int frameNo) const
{
    if(mEvents.contains(frameNo))
    {
        return mEvents[frameNo];
    }
    else
    {
        return EventList();
    }
}

void AnimationModel::setEventText(int frameNo, int index, QString text)
{
    if(index >= 0 && mEvents.contains(frameNo))
    {
        mEvents[frameNo].mList[index] = text;
    }

}

KeyFrame* AnimationModel::getKeyFrame(int lineNo, int frameNo) const
{
    if (lineNo < 0 || frameNo < 0){return NULL;}
    for (int i = 0; i < mKeyFrames[lineNo].count(); i++)
    {
        if (mKeyFrames[lineNo][i]->mFrameNo == frameNo)
        {
            return mKeyFrames[lineNo][i];
        }
    }

    return NULL;
}

int AnimationModel::getKeyFrameIndex(int lineNo, int frameNo) const
{
    if (lineNo < 0 || frameNo < 0) {return -2;}
    for (int i = 0; i < mKeyFrames[lineNo].count(); i++)
    {
        if (mKeyFrames[lineNo][i]->mFrameNo == frameNo)
        {
            return i;
        }
    }
    return -1;
}

bool AnimationModel::isKeyData(KeyFrameData::TweenAttribute tweenAttribute, const KeyFrame* pKeyframe) const
{
    // empty keyframe
    if(pKeyframe->getKeyFrameType() == KeyFrame::KeyFrameType_empty) {return true;}

    // Whatever keyframe
    if (tweenAttribute == KeyFrameData::TweenAttribute_any) {return true;}

    // keyframe with specified tween attribute
    if( pKeyframe->mpKeyFrameData->mTweenTypes[tweenAttribute] != KeyFrameData::eTT_None ) { return true; }

    return false;
}

int AnimationModel::getPreviousKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const
{
    const QList<KeyFrame*>& keyframeList = mKeyFrames[lineNo];
    for (int i = keyframeList.count() - 1; i >= 0; i--)
    {
        const KeyFrame* pKeyframe = keyframeList[i];
        if (frameNo >= pKeyframe->mFrameNo)
        {
            if (isKeyData(tweenAttribute, pKeyframe))
            {
                return i;
            }
        }
    }

    return -1;
}

int AnimationModel::getNextKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const
{
    const QList<KeyFrame*>& keyframeList = mKeyFrames[lineNo];
    for (int i = 0; i < keyframeList.count(); i++)
    {
        const KeyFrame* pKeyframe = keyframeList[i];
        if (pKeyframe->mFrameNo >= frameNo)
        {
            if (isKeyData(tweenAttribute, pKeyframe))
            {
                return i;
            }
        }
    }

    return -1;
}


int AnimationModel::getSubanimationStartKeyFrameIndex(int lineNo, int frameNo) const
{
    int prevIndex = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
    if (prevIndex < 0){return -1;}
    if (!mKeyFrames[lineNo][prevIndex]->mpKeyFrameData){return -1;}
    QString sourthPath = mKeyFrames[lineNo][prevIndex]->mpKeyFrameData->mSpriteDescriptor.mSourcePath;

    for (int i = prevIndex - 1; i >= 0; i--)
    {
        if(!mKeyFrames[lineNo][i]->mpKeyFrameData || sourthPath != mKeyFrames[lineNo][i]->mpKeyFrameData->mSpriteDescriptor.mSourcePath)
        {
            return i + 1;
        }
    }

    return 0;
}

void AnimationModel::addEvent(int frameNo)
{
    if(!mEvents.contains(frameNo))
    {
        mEvents.insert(frameNo, EventList());
    }
    mEvents[frameNo].mList.push_back(QString(""));
}

void AnimationModel::removeEvent(int frameNo, int index)
{
    if(mEvents.contains(frameNo))
    {
        mEvents[frameNo].mList.removeAt(index);
        if(mEvents[frameNo].mList.count() == 0)
        {
            mEvents.remove(frameNo);
        }
    }
}

// set new key frame
void AnimationModel::setKeyFrame(int lineNo, int frameNo, const GLSprite::Point2& position)
{
    if (lineNo >= MaxLineNo){return;}
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

        KeyFrameData* pKeyframeData = new KeyFrameData();
        pKeyframeData->mSpriteDescriptor.mPosition = position;
        pKeyframeData->mSpriteDescriptor.mTextureSrcRect = mSelectedPaletTextureSrcRect;
        pKeyframeData->mSpriteDescriptor.mSourcePath = mSelectedSourcePath;

        KeyFrame* pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyframeData);
        mKeyFrames[lineNo].insert(index + 1, pKeyFrame);

        emit refreshTimeLine();
    }
}

void AnimationModel::setKeyFrame(int lineNo, int frameNo, KeyFrameData* pKeyframeData)
{
    if (lineNo >= MaxLineNo){return;}
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
        KeyFrame* pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyframeData);
        mKeyFrames[lineNo].insert(index + 1, pKeyFrame);

        emit refreshTimeLine();
    }
}

void AnimationModel::insertEmptyKeyFrame(int lineNo, int frameNo)
{
    if (lineNo >= MaxLineNo){return;}
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

        KeyFrame* pKeyframe = new KeyFrame(lineNo, frameNo, NULL);
        mKeyFrames[lineNo].insert(index + 1, pKeyframe);

        emit refreshTimeLine();
    }
}

void AnimationModel::addFrameLength(int lineNo, int frameNo, int value)
{
    if (value >= 0)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
        for (int i = index + 1; i < mKeyFrames[lineNo].count(); i++)
        {
            mKeyFrames[lineNo][i]->mFrameNo += value;
        }

        emit refreshTimeLine();
    }
}

void AnimationModel::reduceFrameLength(int lineNo, int frameNo)
{
    int endKeyFrameIndex = getNextKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
    
    // If it cannot reduce frame length more, return it
    if (
            endKeyFrameIndex > 0 &&
            mKeyFrames[lineNo][endKeyFrameIndex]->mFrameNo - mKeyFrames[lineNo][endKeyFrameIndex - 1]->mFrameNo <= 1
    )
    {
        return;
    }

    // If keyframe exists on this timeline
    if (endKeyFrameIndex >= 0)
    {
        // Move position of keyframes effected
        for (int i = endKeyFrameIndex; i < mKeyFrames[lineNo].count(); i++)
        {
            mKeyFrames[lineNo][i]->mFrameNo -= 1;
        }

        emit refreshTimeLine();
    }
}

void AnimationModel::clearFrames(int lineNo, int startFrameNo, int endFrameNo)
{
    // Remove frames
    for (int i = endFrameNo; i >= startFrameNo; i--)
    {
        int keyframeIndex = getKeyFrameIndex(lineNo, i);
        if (keyframeIndex >= 0)
        {
            delete mKeyFrames[lineNo][keyframeIndex];
            mKeyFrames[lineNo].removeAt(keyframeIndex);
        }
    }

    emit refreshTimeLine();
}

void AnimationModel::clearAllKeyFrames()
{
    for (int lineNo = 0;  lineNo < MaxLineNo; lineNo++)
    {
        for (int frameNo = mKeyFrames[lineNo].count() - 1; frameNo >= 0; frameNo--)
        {
            delete mKeyFrames[lineNo][frameNo];
            mKeyFrames[lineNo].removeAt(frameNo);
        }
    }
    emit refreshTimeLine();
}



const QList<KeyFrame*>& AnimationModel::getKeyFrameList(int lineNo) const
{
    return mKeyFrames[lineNo];
}


QString AnimationModel::getSelectedSourcePath() const
{
    return mSelectedSourcePath;
}

void AnimationModel::setSelectedSourcePath(QString path)
{
    if (ResourceManager::getFileType(path) == ResourceManager::FileType_Invalid) { path = ""; }

    if (mSelectedSourcePath != path)
    {
        mSelectedSourcePath = path;
        mSelectedPaletTextureSrcRect = DEFAULT_SELECTION_RECT;
        emit selectedPaletChanged(path);
    }
}

// Tween Related Stuff
const GLSprite* AnimationModel::createGLSpriteAt(const GLSprite* parentGLSprite, int frameNo, int lineNo) const
{
    const GLSprite* pGLSprite = tweenFrame(parentGLSprite, lineNo, frameNo);
    return pGLSprite;
}

const GLSprite* AnimationModel::createGLSpriteAt(const GLSprite* parentGLSprite, QList<KeyFrame::KeyFramePosition>nodePath) const
{
    const GLSprite* pGLSprite = parentGLSprite;
    const AnimationModel* pAnimationModel = this;
    for (int i = 0; i < nodePath.count(); i++)
    {
        pGLSprite = pAnimationModel->createGLSpriteAt(pGLSprite, nodePath[i].mFrameNo, nodePath[i].mLineNo);
        if (!pGLSprite) {return NULL;}
        if (ResourceManager::getFileType(pGLSprite->mSpriteDescriptor.mSourcePath) == ResourceManager::FileType_Animation)
        {
            pAnimationModel = ResourceManager::getAnimation(pGLSprite->mSpriteDescriptor.mSourcePath);
        }
    }

    return pGLSprite;
}

const QList<const GLSprite*> AnimationModel::createGLSpriteListAt(const GLSprite* parentGLSprite, int frameNo) const
{
    QList<const GLSprite*> glSpriteList;
    const GLSprite* pGLSprite = parentGLSprite;
    for (int lineNo = 0; lineNo < MaxLineNo; lineNo++)
    {
        pGLSprite = createGLSpriteAt(parentGLSprite, frameNo, lineNo);
        if (pGLSprite)
        {
            glSpriteList.push_back(pGLSprite);
        }
    }

    return glSpriteList;
}

void AnimationModel::tweenElement(GLSprite::SpriteDescriptor& spriteDescriptor, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData::TweenType tweenType, GLSprite::SpriteDescriptor& startDescriptor, GLSprite::SpriteDescriptor& endDescriptor, int lineNo, int frameNo, int startFrameNo, int endFrameNo) const
{
    switch(tweenAttribute)
    {
        case KeyFrameData::TweenAttribute_alpha:
            Tween(tweenType, startDescriptor.mAlpha, endDescriptor.mAlpha, spriteDescriptor.mAlpha, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_position:
            Tween(tweenType, startDescriptor.mPosition.mX, endDescriptor.mPosition.mX, spriteDescriptor.mPosition.mX, frameNo, startFrameNo, endFrameNo);
            Tween(tweenType, startDescriptor.mPosition.mY, endDescriptor.mPosition.mY, spriteDescriptor.mPosition.mY, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_rotation:
            Tween(tweenType, startDescriptor.mRotation, endDescriptor.mRotation, spriteDescriptor.mRotation, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_scale:
            Tween(tweenType, startDescriptor.mScale.mX, endDescriptor.mScale.mX, spriteDescriptor.mScale.mX, frameNo, startFrameNo, endFrameNo);
            Tween(tweenType, startDescriptor.mScale.mY, endDescriptor.mScale.mY, spriteDescriptor.mScale.mY, frameNo, startFrameNo, endFrameNo);
            break;
        default:
            break;
    }
}

// Update the sprite so that it reflects some options
void AnimationModel::setFinalAlpha(const GLSprite* parentGLSprite, GLSprite::SpriteDescriptor& spriteDescriptor) const
{
    float dAlpha = 1.0;
    if (parentGLSprite)
    {
        dAlpha = parentGLSprite->mSpriteDescriptor.mAlpha;
    }
    spriteDescriptor.mAlpha *= dAlpha;
}

void AnimationModel::setFinalPosition(const GLSprite* parentGLSprite, GLSprite::SpriteDescriptor& spriteDescriptor) const
{
    if(spriteDescriptor.mRelativeToTarget)
    {
        // Transform current position based on target point
        QPoint point = QPointF(spriteDescriptor.mPosition.mX, spriteDescriptor.mPosition.mY).toPoint();
        if(parentGLSprite)
        {
            point = point * spTargetSprite->getCombinedTransform() * parentGLSprite->getCombinedTransform().inverted();
        }
        else
        {
            point = point * spTargetSprite->getTransform();
        }

        spriteDescriptor.mPosition.mX = point.x();
        spriteDescriptor.mPosition.mY = point.y();
    }
}

void AnimationModel::setFinalRotation(const GLSprite* parentGLSprite, int lineNo, int frameNo, GLSprite::SpriteDescriptor& spriteDescriptor) const
{
    if (sIsNesting){return;} // if it is nesting, don't do this part
    sIsNesting = true;
    switch(spriteDescriptor.mFacingOptionType)
    {
        // Look at target sprite
        case GLSprite::FacingOptionType_lookAtTarget:
        {
            QPoint point = QPointF(spriteDescriptor.mPosition.mX, spriteDescriptor.mPosition.mY).toPoint();

            // Transform current position to screen coordinate
            if (parentGLSprite)
            {
                point = point * (parentGLSprite->getCombinedTransform());
            }
            else
            {
                point = point * (spTargetSprite->getCombinedTransform() * spTargetSprite->getTransform().inverted());
            }

            // Transform target position to screen coordinate
            QPointF targetPoint = QPointF(spTargetSprite->mSpriteDescriptor.mPosition.mX, spTargetSprite->mSpriteDescriptor.mPosition.mY);
            targetPoint = targetPoint * spTargetSprite->getCombinedTransform() * spTargetSprite->getTransform().inverted();

            // Compare target and current sprite in screen coordinate
            float dx = targetPoint.x() - point.x();
            float dy = targetPoint.y() - point.y();

            if (dx == 0 && dy == 0)
            {
                GLSprite::SpriteDescriptor tempSpriteDescriptor = spriteDescriptor;
                // In order to face to correct direction, it needs to calculate the position first
                copyTweenedAttribute(parentGLSprite, tempSpriteDescriptor, lineNo, frameNo - 1, KeyFrameData::TweenAttribute_position);
                copyTweenedAttribute(parentGLSprite, tempSpriteDescriptor, lineNo, frameNo - 1, KeyFrameData::TweenAttribute_rotation);
                spriteDescriptor.mRotation = tempSpriteDescriptor.mRotation;
                int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                spriteDescriptor.mRotation += angleOffset;
            }
            else
            {
                int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                spriteDescriptor.mRotation += angleOffset;
            }
       }
       break;

       // Face to the direction where it is going to move
       case GLSprite::FacingOptionType_FaceToMovingDir:
           {
                const AnimationModel* pAnimationModel = this;
                QList<KeyFrame::KeyFramePosition> list;
                if (parentGLSprite)
                {
                    list = parentGLSprite->getNodePath();
                    pAnimationModel = parentGLSprite->getRootSprite()->getParentAnimationModel();
                }
                list.push_back(KeyFrame::KeyFramePosition(lineNo, frameNo));
                for (int i = 0; i < list.count(); i++)
                {
                    list[i].mFrameNo += 1;
                }

                const GLSprite* pTargetSprite = pAnimationModel->createGLSpriteAt(NULL, list);

                if (pTargetSprite)
                {
                    QPointF point = QPointF(spriteDescriptor.mPosition.mX, spriteDescriptor.mPosition.mY);

                    // Transform current position to screen coordinate
                    if (parentGLSprite)
                    {
                        point = point * (parentGLSprite->getCombinedTransform());
                    }
                    else
                    {
                        point = point * (spTargetSprite->getCombinedTransform() * spTargetSprite->getTransform().inverted());
                    }
                    // Transform target position to screen coordinate
                    QPointF targetPoint = QPointF(pTargetSprite->mSpriteDescriptor.mPosition.mX, pTargetSprite->mSpriteDescriptor.mPosition.mY);
                    if (pTargetSprite->getParentSprite())
                    {
                        targetPoint = targetPoint * pTargetSprite->getParentSprite()->getCombinedTransform();
                    }
                    else
                    {
                        targetPoint = targetPoint * (spTargetSprite->getCombinedTransform() * spTargetSprite->getTransform().inverted());
                    }
                    // Compare target and current sprite in screen coordinate
                    float dx = targetPoint.x() - point.x();
                    float dy = targetPoint.y() - point.y();

                    int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                    spriteDescriptor.mRotation += angleOffset;
                }

                // delete all temporaly generated Sprites.
                const GLSprite* pSprite = pTargetSprite;
                while (pSprite)
                {
                    const GLSprite* tSprite = pSprite->getParentSprite();
                    delete pSprite;
                    pSprite = tSprite;
                }
           }
           break;
       default:

       break;
    }
    sIsNesting = false;
}

bool AnimationModel::copyTweenedAttribute(const GLSprite* pParentGLSprite, GLSprite::SpriteDescriptor& spriteDescriptor, int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const
{
    bool tweenFound = false;
    int startIndex = getPreviousKeyFrameIndex(lineNo, frameNo, tweenAttribute);
    int endIndex = getNextKeyFrameIndex(lineNo, frameNo, tweenAttribute);
    if (startIndex >= 0 && endIndex > startIndex)
    {
        tweenFound = true;
        KeyFrame* pStartKeyFrame = mKeyFrames[lineNo][startIndex];
        KeyFrame* pEndKeyFrame = mKeyFrames[lineNo][endIndex];

        GLSprite::SpriteDescriptor startDescriptor = pStartKeyFrame->mpKeyFrameData->mSpriteDescriptor;
        setFinalAlpha(pParentGLSprite, startDescriptor);
        setFinalPosition(pParentGLSprite, startDescriptor);

        GLSprite::SpriteDescriptor endDescriptor;
        if (pEndKeyFrame->mpKeyFrameData)
        {
            endDescriptor = pEndKeyFrame->mpKeyFrameData->mSpriteDescriptor;
            setFinalAlpha(pParentGLSprite, endDescriptor);
            setFinalPosition(pParentGLSprite, endDescriptor);
        }
        else
        {
            endDescriptor = startDescriptor;
        }

        tweenElement(spriteDescriptor, tweenAttribute, pStartKeyFrame->mpKeyFrameData->mTweenTypes[tweenAttribute], startDescriptor, endDescriptor, lineNo, frameNo, pStartKeyFrame->mFrameNo, pEndKeyFrame->mFrameNo);

        
        if(tweenAttribute == KeyFrameData::TweenAttribute_rotation)
        {
            setFinalRotation(pParentGLSprite, lineNo, frameNo, spriteDescriptor);
        }
    }
    else if (endIndex == startIndex)
    {
        tweenFound = true;
        if (tweenAttribute == KeyFrameData::TweenAttribute_rotation)
        {
            setFinalRotation(pParentGLSprite, lineNo, frameNo, spriteDescriptor);
        }
    }

    return tweenFound;
}


// Return true if it find a cel to tween, if not return false;
GLSprite* AnimationModel::tweenFrame(const GLSprite* parentGLSprite, int lineNo, int frameNo) const
{
    // no keyframes in this line
    if (mKeyFrames[lineNo].count() == 0) {return NULL;}

    // exceeds max frame count
    if(frameNo >= getMaxFrameCount(lineNo)){return NULL;}

    // Set up base for keyframe.(inherit textureID etc from previous keyframe
    int baseIndex = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);
    if (baseIndex < 0){return NULL;} // no keyframe exists

    KeyFrame* pBaseKeyFrame = mKeyFrames[lineNo][baseIndex];
    KeyFrameData* pBaseKeyFrameData = pBaseKeyFrame->mpKeyFrameData;
    if (!pBaseKeyFrameData) {return NULL;} // empty keyframe

    GLSprite::SpriteDescriptor baseSpriteDescriptor = pBaseKeyFrameData->mSpriteDescriptor;
    setFinalAlpha(parentGLSprite, baseSpriteDescriptor);
    setFinalPosition(parentGLSprite, baseSpriteDescriptor);

    // Tween for each attribute
    bool anyTweenFound = false;
    for (int i = 0; i < KeyFrameData::TweenAttribute_COUNT; i++)
    {
        if (copyTweenedAttribute(parentGLSprite, baseSpriteDescriptor, lineNo, frameNo, (KeyFrameData::TweenAttribute)i))
        {
            anyTweenFound = true;
        }
    }

    // start of subanimation
    int subAnimationStartIndex = getSubanimationStartKeyFrameIndex(lineNo, frameNo);

    // frame no for subanimation in the frame
    if (subAnimationStartIndex < 0)
    {
        baseSpriteDescriptor.mFrameNo = 0;
    }
    else
    {
        baseSpriteDescriptor.mFrameNo = frameNo - mKeyFrames[lineNo][subAnimationStartIndex]->mFrameNo;
    }

    bool isTweenCel  = (pBaseKeyFrame->mFrameNo == frameNo);
    return new GLSprite(parentGLSprite, this, lineNo, baseSpriteDescriptor, isTweenCel, lineNo, frameNo);
}

KeyFrame::KeyFramePosition AnimationModel::getCurrentKeyFramePosition()
{
    return mSelectedKeyFramePosition;
}

void AnimationModel::selectCurrentKeyFramePosition(int lineNo, int frameNo)
{
    KeyFrame::KeyFramePosition keyframePosition;
    keyframePosition.mLineNo = lineNo;
    keyframePosition.mFrameNo =  frameNo;

    if (mSelectedKeyFramePosition != keyframePosition)
    {
        mSelectedKeyFramePosition = keyframePosition;

        emit selectedKeyFramePositionChanged(mSelectedKeyFramePosition.mLineNo, mSelectedKeyFramePosition.mFrameNo);
    }
}

// HACK: using animation model as an agent..
void AnimationModel::tellTimeLineToRefresh()
{
    emit refreshTimeLine();
}

bool AnimationModel::saveData()
{
    // Get path with directory info
    QString rootPath = QDir::currentPath();
    rootPath.append("/");
    rootPath.append(ANIMATION_DIR.path());

    // we don't want to store absolute path
    QString saveDirName = rootPath.append(mAnimationDirectory).append("/");
    QDir saveDir = QDir(saveDirName);
    QString savePath = saveDirName.append(mAnimationID).append(".").append(ANIMATION_FORMAT);
    QString originalPath = rootPath.append(mAnimationDirectory).append("/").append(mOriginalAnimationID).append(".").append(ANIMATION_FORMAT);
    QFileInfo info = QFileInfo(savePath);

    QString error = QString("File %0.ani already exists.").arg(mAnimationID);
    // unless you are overriding loaded animation ID, we won't allow to override existing files
    if (mOriginalAnimationID != mAnimationID && info.exists())
    {
        QMessageBox::information(mpParent->window(), tr("File saving error"),
                             tr(error.toStdString().c_str()));
        return false;
    }

    Json::Value root;

    // save animation name
    if (mAnimationName.toStdString() != "")
    {
        root["name"] = mAnimationName.toStdString();
    }

    // save keyframes
    Json::Value keyframesData;
    for (int i = 0; i < MaxLineNo; i++)
    {
        //if no data exists in this line, ignore it.
        if(mKeyFrames[i].count() == 0)
        {
            continue;
        }

        for (int j = 0; j < mKeyFrames[i].count(); j++)
        {
            const KeyFrame* keyframe = mKeyFrames[i][j];

            Json::Value keyframeData;
            keyframeData["frameNo"] = keyframe->mFrameNo;

            if (keyframe->mpKeyFrameData)
            {
                KeyFrameData* pKeyFrameData = keyframe->mpKeyFrameData;
                keyframeData["sourcePath"] = pKeyFrameData->mSpriteDescriptor.mSourcePath.toStdString();

                if (pKeyFrameData->mSpriteDescriptor.mBlur > 0)
                {
                    keyframeData["blur"] = pKeyFrameData->mSpriteDescriptor.mBlur;
                }

                if (pKeyFrameData->mSpriteDescriptor.mFacingOptionType != GLSprite::FacingOptionType_none)
                {
                    keyframeData["facingOption"] = GLSprite::facingOptionTypeSting[pKeyFrameData->mSpriteDescriptor.mFacingOptionType].toStdString();
                }

                if (pKeyFrameData->mSpriteDescriptor.mRelativeToTarget)
                {
                    keyframeData["relativeToTarget"] = pKeyFrameData->mSpriteDescriptor.mRelativeToTarget;
                }

                if (ResourceManager::getFileType(pKeyFrameData->mSpriteDescriptor.mSourcePath) == ResourceManager::FileType_Image)
                {
                    QPixmap* pixmap = getPixmap(pKeyFrameData->mSpriteDescriptor.mSourcePath);

                    if (pixmap->rect() != pKeyFrameData->mSpriteDescriptor.mTextureSrcRect)
                    {
                        Json::Value textureRect;
                        textureRect[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.x();
                        textureRect[1] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.y();
                        textureRect[2] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.width();
                        textureRect[3] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.height();
                        keyframeData["textureRect"] = textureRect;
                    }
                }

                if (pKeyFrameData->mSpriteDescriptor.mBlendType != GLSprite::eBT_Alpha)
                {
                    keyframeData["blendType"] = GLSprite::blendTypeSting[pKeyFrameData->mSpriteDescriptor.mBlendType].toStdString();
                }

                if (pKeyFrameData->mSpriteDescriptor.mCenter != QPoint(0, 0))
                {
                    Json::Value centerPoint;
                    centerPoint[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mCenter.mX;
                    centerPoint[1] = pKeyFrameData->mSpriteDescriptor.mCenter.mY;
                    keyframeData["center"] = centerPoint;
                }

                 if (pKeyFrameData->mSpriteDescriptor.mScale != QPoint(1, 1))
                 {
                    Json::Value scale;
                    scale[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mScale.mX;
                    scale[1] = pKeyFrameData->mSpriteDescriptor.mScale.mY;
                    keyframeData["scale"] = scale;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mPosition != QPoint(0, 0))
                 {
                    Json::Value position;
                    position[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mPosition.mX;
                    position[1] = pKeyFrameData->mSpriteDescriptor.mPosition.mY;
                    keyframeData["position"] = position;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mRotation != 0)
                 {
                    keyframeData["rotation"] = pKeyFrameData->mSpriteDescriptor.mRotation;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mPriority != 0.5)
                 {
                    keyframeData["priority"] = pKeyFrameData->mSpriteDescriptor.mPriority;
                 }

                 if (pKeyFrameData->mSpriteDescriptor.mAlpha < 1)
                 {
                    keyframeData["alpha"] = pKeyFrameData->mSpriteDescriptor.mAlpha;
                 }


                for (int k = 0; k < KeyFrameData::TweenAttribute_COUNT; k++)
                {
                    if (pKeyFrameData->mTweenTypes[k] != KeyFrameData::eTT_Fix)
                    {
                        keyframeData[KeyFrameData::tweenAttributeSting[k].toStdString()] = KeyFrameData::tweenTypeSting[pKeyFrameData->mTweenTypes[k]].toStdString();
                    }
                }
            }
            else
            {
                keyframeData["isEmpty"] = true;
            }

            keyframesData[QString::number(i).toStdString()][j] = keyframeData;
        }
    }

    if (!keyframesData.empty())
    {
        root["keyframes"] = keyframesData;
    }

    // Write event data
    if(mEvents.count() > 0)
    {
        Json::Value events;
        for(QHash<int, AnimationModel::EventList>::iterator iter = mEvents.begin() ; mEvents.end() != iter ; iter++)
        {
            int lineNo = iter.key();
            AnimationModel::EventList eventList = static_cast<AnimationModel::EventList>(*iter);
            Json::Value list;
            for (int i = 0; i < eventList.mList.count(); i++)
            {
                list[static_cast<unsigned int>(i)] = eventList.mList[i].toStdString();
            }

            events[QString::number(lineNo).toStdString()] = list;
        }
        root["events"] = events;
    }

    saveDir.remove(mOriginalAnimationID.append(".").append(ANIMATION_FORMAT));
    Json::StyledWriter writer;
    std::string outputJson = writer.write(root);
    std::ofstream ofs;
    ofs.open(savePath.toStdString().c_str());
    ofs << outputJson << std::endl;

    ofs.close();

    mOriginalAnimationID = mAnimationID;
    emit fileSaved(QModelIndex());

    return true;
}

bool AnimationModel::loadData(QString path)
{
    clearAllKeyFrames();
    mEvents.clear();

    Json::Value root;
    Json::Reader reader;

    std::ifstream ifs(path.toStdString().c_str());
    std::string inputJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    //std::string json_doc = readInputFile(path);
    if(!reader.parse(inputJson, root))
    {
        QString error = QString("File %0 is not correct JSON format").arg(path);
        printf("%s", error.toStdString().c_str());
        return false;
    }

    // Get path without directory info & extension
    QString rootPath = QDir::currentPath();
    rootPath.append("/");
    rootPath.append(ANIMATION_DIR.path());

    QFileInfo fileInfo = QFileInfo(path);

    mAnimationDirectory = fileInfo.absolutePath().replace(rootPath, "");;
    mOriginalAnimationID = fileInfo.baseName();
    setAnimationID(mOriginalAnimationID);

    // save animation name
    setAnimationName(QString::fromStdString(root["name"].asString()));
    root["name"] = mAnimationName.toStdString();

    Json::Value& lines = root["keyframes"];

    for(Json::Value::iterator iter = lines.begin() ; lines.end() != iter ; iter++)
    {
        const char* c_str = iter.memberName();
        int lineNo = atoi(c_str);

        Json::Value& line =  *iter;
        for (unsigned int j = 0; j < line.size(); j++)
        {
            Json::Value& keyframe = line[j];

            int frameNo = keyframe["frameNo"].asInt();
            KeyFrame* pKeyFrame = NULL;
            if (!keyframe["isEmpty"].asBool())
            {
                KeyFrameData* pKeyFrameData = new KeyFrameData();

                // Tween valid data
                for (int k = 0; k < KeyFrameData::TweenAttribute_COUNT; k++)
                {
                    if (keyframe[KeyFrameData::tweenAttributeSting[k].toStdString()].isString())
                    {
                        pKeyFrameData->mTweenTypes[k] = KeyFrameData::getTweenTypeByString(QString::fromStdString(keyframe[KeyFrameData::tweenAttributeSting[k].toStdString()].asString()));
                    }
                }

                if (keyframe["blur"].isInt())
                {
                    pKeyFrameData->mSpriteDescriptor.mBlur = keyframe["blur"].asInt();
                }
                pKeyFrameData->mSpriteDescriptor.mFacingOptionType = GLSprite::getFacingOptionTypeByString(QString::fromStdString(keyframe["facingOption"].asString()));

                if (keyframe["relativeToTarget"].isBool())
                {
                    pKeyFrameData->mSpriteDescriptor.mRelativeToTarget = keyframe["relativeToTarget"].asBool();
                }

                pKeyFrameData->mSpriteDescriptor.mSourcePath = QString::fromStdString(keyframe["sourcePath"].asString());

                // Blend
                if (keyframe["blendType"].isString())
                {
                    pKeyFrameData->mSpriteDescriptor.mBlendType = GLSprite::getBlendTypeByString(QString::fromStdString(keyframe["blendType"].asString()));
                }

                // Alpha
                if (keyframe["alpha"].isNumeric())
                {
                    pKeyFrameData->mSpriteDescriptor.mAlpha = keyframe["alpha"].asDouble();
                }

                // priority data
                if (!keyframe["priority"].isNull())
                {
                    pKeyFrameData->mSpriteDescriptor.mPriority = keyframe["priority"].asDouble();
                }

                // position data
                if (!keyframe["position"].isNull())
                {
                    pKeyFrameData->mSpriteDescriptor.mPosition.mX = keyframe["position"][static_cast<unsigned int>(0)].asInt();
                    pKeyFrameData->mSpriteDescriptor.mPosition.mY = keyframe["position"][1].asInt();
                }

                if (!keyframe["rotation"].isNull())
                {
                    pKeyFrameData->mSpriteDescriptor.mRotation = keyframe["rotation"].asInt();
                }

                if (!keyframe["scale"].isNull())
                {
                    pKeyFrameData->mSpriteDescriptor.mScale.mX = keyframe["scale"][static_cast<unsigned int>(0)].asDouble();
                    pKeyFrameData->mSpriteDescriptor.mScale.mY = keyframe["scale"][1].asDouble();
                }

                if (!keyframe["center"].isNull())
                {
                    pKeyFrameData->mSpriteDescriptor.mCenter.mX = keyframe["center"][static_cast<unsigned int>(0)].asInt();
                    pKeyFrameData->mSpriteDescriptor.mCenter.mY = keyframe["center"][1].asInt();
                }

                if (!keyframe["textureRect"].isNull())
                {
                    pKeyFrameData->mSpriteDescriptor.mTextureSrcRect = QRect(
                            keyframe["textureRect"][static_cast<unsigned int>(0)].asInt(),
                            keyframe["textureRect"][1].asInt(),
                            keyframe["textureRect"][2].asInt(),
                            keyframe["textureRect"][3].asInt()
                            );
                }
                else
                {
                    if (ResourceManager::getFileType(pKeyFrameData->mSpriteDescriptor.mSourcePath) == ResourceManager::FileType_Image)
                    {
                        QPixmap* pixmap = getPixmap(pKeyFrameData->mSpriteDescriptor.mSourcePath);
                        pKeyFrameData->mSpriteDescriptor.mTextureSrcRect = pixmap->rect();
                    }
                    else
                    {
                        pKeyFrameData->mSpriteDescriptor.mTextureSrcRect = DEFAULT_SELECTION_RECT;
                    }
                }

                pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyFrameData);
            }
            else
            {
                pKeyFrame = new KeyFrame(lineNo, frameNo, NULL);
            }
            mKeyFrames[lineNo].push_back(pKeyFrame);
        }
    }


    Json::Value& events = root["events"];
    if (!events.isNull())
    {
        for(Json::Value::iterator iter = events.begin() ; events.end() != iter ; iter++)
        {
            // Get frameNo for the event and add to hash
            const char* c_str = iter.memberName();
            int frameNo = atoi(c_str);
            mEvents.insert(frameNo, EventList());

            // add event from event list
            Json::Value& eventList = *iter;
            for (unsigned int i = 0; i < eventList.size(); i++)
            {
                mEvents[frameNo].mList.push_back(QString::fromStdString(eventList[i].asString()));
            }
        }
    }
    // event data

    mSelectedKeyFramePosition.mFrameNo = 0;
    mSelectedKeyFramePosition.mLineNo = 0;

    emit refreshTimeLine();
    return true;
}

QString AnimationModel::getLoadedAnimationPath() const
{
    QString path = mAnimationDirectory;
    return path.append("/Animations/").append(mAnimationID).append(".").append(ANIMATION_FORMAT);
}
