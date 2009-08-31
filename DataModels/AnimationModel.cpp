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
#define DEFAULT_SELECTION_RECT GLSprite::Rect(0, 0, 32, 32);

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
    : mAnimationName(QString("")),
      mAnimationDirectory(QString("")),
      mAnimationID(QString("")),
      mOriginalAnimationID(QString("")),
      mpParent(parent)
{
    mSelectedSourcePath = "";

    mSelectedKeyFramePosition.mFrameNo = -1;
    mSelectedKeyFramePosition.mLineNo = -1;

    // Target sprite
    if (spTargetPixmap == NULL)
    {
        spTargetPixmap = new QPixmap(":/resource/target.png");
        GLSprite::SpriteDescriptor spriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();

        spriteDescriptor.mTextureSrcRect.mWidth = 32;
        spriteDescriptor.mTextureSrcRect.mHeight = 32;
        spriteDescriptor.mCenter.mX = 16;
        spriteDescriptor.mCenter.mY = 16;
        spriteDescriptor.mPosition.mX = -100;

        spTargetSprite = new GLSprite(-1, spriteDescriptor, false, spTargetPixmap);
    }

    // Center sprite
    if (spCenterPointPixmap == NULL)
    {
        spCenterPointPixmap = new QPixmap(":/resource/center_point.png");
        GLSprite::SpriteDescriptor spriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();
        spriteDescriptor.mTextureSrcRect.mWidth = 8;
        spriteDescriptor.mTextureSrcRect.mHeight = 8;
        spriteDescriptor.mCenter.mX = 4;
        spriteDescriptor.mCenter.mY = 4;

        spCenterPointSprite = new GLSprite(-1, spriteDescriptor, false, spCenterPointPixmap);
    }
}

AnimationModel::~AnimationModel()
{
}

void AnimationModel::setTargetSpritePosition(int x, int y)
{
    if (spTargetSprite->mSpriteDescriptor.mPosition.mX != x || spTargetSprite->mSpriteDescriptor.mPosition.mY != y)
    {
        spTargetSprite->mSpriteDescriptor.mPosition.mX = x;
        spTargetSprite->mSpriteDescriptor.mPosition.mY = y;

        emit targetPositionMoved(x, y);
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

int AnimationModel::getMaxFrameCount()
{
    int max = 0;
    for (int i = 0; i < MaxLineNo; i++)
    {
        if (mKeyFrames[i].count() > 0)
        {
            int t = mKeyFrames[i].last()->mFrameNo + 1;
            if (t > max)
            {
                max = t;
            }
        }
    }

    return max;
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
    if (tweenAttribute == KeyFrameData::TweenAttribute_any && !pKeyframe->mpKeyFrameData->allAttributesNone()) {return true;}

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

// set new key frame
void AnimationModel::setKeyFrame(int lineNo, int frameNo, const GLSprite::Point2& position)
{
    // if a keframe already exists, don't add any keyframe
    if (getKeyFrameIndex(lineNo, frameNo) == -1)
    {
        int index = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

        KeyFrameData* pKeyframeData = new KeyFrameData();
        pKeyframeData->mSpriteDescriptor.mPosition = position;
        pKeyframeData->mSpriteDescriptor.mTextureSrcRect = mSelectedPaletTextureSrcRect;
        pKeyframeData->mSpriteDescriptor.mCenter.mX = mSelectedPaletTextureSrcRect.mWidth / 2;
        pKeyframeData->mSpriteDescriptor.mCenter.mY = mSelectedPaletTextureSrcRect.mHeight / 2;
        pKeyframeData->mSpriteDescriptor.mSourcePath = mSelectedSourcePath;

        KeyFrame* pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyframeData);
        mKeyFrames[lineNo].insert(index + 1, pKeyFrame);

        emit refreshTimeLine();
    }
}

void AnimationModel::setKeyFrame(int lineNo, int frameNo, KeyFrameData* pKeyframeData)
{
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

GLSprite* AnimationModel::createGLSpriteAt(int frameNo, int lineNo, const GLSprite* pParentSprite) const
{
    GLSprite* pGLSprite = tweenFrame(lineNo, frameNo, pParentSprite);
    return pGLSprite;
}

const QList<GLSprite*> AnimationModel::createGLSpriteListAt(int frameNo, const GLSprite* pParentSprite) const
{
    QList<GLSprite*> glSpriteList;
    for (int lineNo = 0; lineNo < MaxLineNo; lineNo++)
    {
        GLSprite* pGLSprite = createGLSpriteAt(frameNo, lineNo, pParentSprite);
        if (pGLSprite)
        {
            glSpriteList.push_back(pGLSprite);
        }
    }

    return glSpriteList;
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

QString AnimationModel::getSelectedSourcePath() const
{
    return mSelectedSourcePath;
}

void AnimationModel::tweenElement(GLSprite::SpriteDescriptor& spriteDescriptor, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData::TweenType tweenType, GLSprite::SpriteDescriptor& startDescriptor, GLSprite::SpriteDescriptor& endDescriptor, int frameNo, int startFrameNo, int endFrameNo, const GLSprite* pParentSprite) const
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
            Tween(tweenType, startDescriptor.mRotation.mX, endDescriptor.mRotation.mX, spriteDescriptor.mRotation.mX, frameNo, startFrameNo, endFrameNo);
            if(startDescriptor.mLookAtTarget)
            {
                int dPosX = 0;
                int dPosY = 0;
                if (pParentSprite)
                {
                    dPosX = pParentSprite->mSpriteDescriptor.mPosition.mX;
                    dPosY = pParentSprite->mSpriteDescriptor.mPosition.mY;
                }
                int dx = spTargetSprite->mSpriteDescriptor.mPosition.mX - spriteDescriptor.mPosition.mX - dPosX;
                int dy = spTargetSprite->mSpriteDescriptor.mPosition.mY - spriteDescriptor.mPosition.mY - dPosY;
                int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                spriteDescriptor.mRotation.mX += angleOffset;
            }
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
void AnimationModel::setFinalAlpha(GLSprite::SpriteDescriptor& spriteDescriptor, const GLSprite* pParentSprite) const
{
    float dAlpha = 1;
    if (pParentSprite)
    {
        dAlpha = pParentSprite->mSpriteDescriptor.mAlpha;
    }
    spriteDescriptor.mAlpha *= dAlpha;
}

void AnimationModel::setFinalPosition(GLSprite::SpriteDescriptor& spriteDescriptor, const GLSprite* pParentSprite) const
{
    if(spriteDescriptor.mRelativeToTarget)
    {
        int dPosX = 0;
        int dPosY = 0;
        if (pParentSprite)
        {
            dPosX = pParentSprite->mSpriteDescriptor.mPosition.mX;
            dPosY = pParentSprite->mSpriteDescriptor.mPosition.mY;
        }
        spriteDescriptor.mPosition.mX += spTargetSprite->mSpriteDescriptor.mPosition.mX - dPosX;
        spriteDescriptor.mPosition.mY += spTargetSprite->mSpriteDescriptor.mPosition.mY- dPosY;
    }
}

void AnimationModel::setFinalRotation(int lineNo, int frameNo, GLSprite::SpriteDescriptor& spriteDescriptor, const GLSprite* pParentSprite) const
{
        if(spriteDescriptor.mLookAtTarget)
        {
            int dPosX = 0;
            int dPosY = 0;
            if (pParentSprite)
            {
                dPosX = pParentSprite->mSpriteDescriptor.mPosition.mX;
                dPosY = pParentSprite->mSpriteDescriptor.mPosition.mY;
            }
            int dx = spTargetSprite->mSpriteDescriptor.mPosition.mX - spriteDescriptor.mPosition.mX - dPosX;
            int dy = spTargetSprite->mSpriteDescriptor.mPosition.mY - spriteDescriptor.mPosition.mY - dPosY;
            if (dx == 0 && dy == 0)
            {
                GLSprite::SpriteDescriptor tempSpriteDescriptor = spriteDescriptor;
                // In order to face to correct direction, it needs to calculate the position first
                copyTweenedAttribute(tempSpriteDescriptor, lineNo, frameNo - 1, KeyFrameData::TweenAttribute_position, pParentSprite);
                copyTweenedAttribute(tempSpriteDescriptor, lineNo, frameNo - 1, KeyFrameData::TweenAttribute_rotation, pParentSprite);
                spriteDescriptor.mRotation.mX = tempSpriteDescriptor.mRotation.mX;
            }
            else
            {
                int angleOffset = (int)floor((180 * atan2(dy, dx)) / PI);
                spriteDescriptor.mRotation.mX += angleOffset;
            }
        }
}

bool AnimationModel::copyTweenedAttribute(GLSprite::SpriteDescriptor& spriteDescriptor, int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute, const GLSprite* pParentSprite) const
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
        setFinalAlpha(startDescriptor, pParentSprite);
        setFinalPosition(startDescriptor, pParentSprite);

        GLSprite::SpriteDescriptor endDescriptor;
        if (pEndKeyFrame->mpKeyFrameData)
        {
            endDescriptor = pEndKeyFrame->mpKeyFrameData->mSpriteDescriptor;
            setFinalAlpha(endDescriptor, pParentSprite);
            setFinalPosition(endDescriptor, pParentSprite);
        }
        else
        {
            endDescriptor = startDescriptor;
        }

        tweenElement(spriteDescriptor, tweenAttribute, pStartKeyFrame->mpKeyFrameData->mTweenTypes[tweenAttribute], startDescriptor, endDescriptor, frameNo, pStartKeyFrame->mFrameNo, pEndKeyFrame->mFrameNo, pParentSprite);
    }
    else if (endIndex == startIndex)
    {
        tweenFound = true;
        if (tweenAttribute == KeyFrameData::TweenAttribute_rotation)
        {
            setFinalRotation(lineNo, frameNo, spriteDescriptor, pParentSprite);
        }
    }

    return tweenFound;
}

// Return true if it find a cel to tween, if not return false;
GLSprite* AnimationModel::tweenFrame(int lineNo, int frameNo, const GLSprite* pParentSprite) const
{
    if (mKeyFrames[lineNo].count() == 0) {return NULL;}

    // Set up base for keyframe.(inherit textureID etc from previous keyframe
    int baseIndex = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

    // It will return when the same source file path was first used, this is needed for subanimation playback
    int subAnimationStartIndex = getSubanimationStartKeyFrameIndex(lineNo, frameNo);

    // No valid keyframe has been found before specified cel position
    if (baseIndex < 0){return NULL;}

    KeyFrame* pBaseKeyFrame = mKeyFrames[lineNo][baseIndex];
    KeyFrameData* pBaseKeyFrameData = pBaseKeyFrame->mpKeyFrameData;

    // If the keyframe is empty, don't generate any temporaly keyframe
    if (!pBaseKeyFrameData) {return NULL;}
    bool isTweenCel  = (pBaseKeyFrame->mFrameNo == frameNo);

    GLSprite::SpriteDescriptor baseSpriteDescriptor = pBaseKeyFrameData->mSpriteDescriptor;
    setFinalAlpha(baseSpriteDescriptor, pParentSprite);
    setFinalPosition(baseSpriteDescriptor, pParentSprite);

    // Tween for each attribute
    bool anyTweenFound = false;
    for (int i = 0; i < KeyFrameData::TweenAttribute_COUNT; i++)
    {
        if (copyTweenedAttribute(baseSpriteDescriptor, lineNo, frameNo, (KeyFrameData::TweenAttribute)i, pParentSprite))
        {
            anyTweenFound = true;
        }
    }

    // frame no for subanimation in the frame
    if (subAnimationStartIndex < 0)
    {
        baseSpriteDescriptor.mFrameNo = 0;
    }
    else
    {
        baseSpriteDescriptor.mFrameNo = frameNo - mKeyFrames[lineNo][subAnimationStartIndex]->mFrameNo;
    }

    if (anyTweenFound)
    {
        return new GLSprite(lineNo, baseSpriteDescriptor, isTweenCel);
    }
    else
    {
        return NULL;
    }
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
    root["name"] = mAnimationName.toStdString();

    // save keyframes
    Json::Value& keyframesData = root["keyframes"];
    keyframesData.resize(MaxLineNo);
    for (int i = 0; i < MaxLineNo; i++)
    {
        keyframesData[i].resize(mKeyFrames[i].count());

        for (int j = 0; j < mKeyFrames[i].count(); j++)
        {
            const KeyFrame* keyframe = mKeyFrames[i][j];

            Json::Value keyframeData;
            keyframeData["lineNo"] = keyframe->mLineNo;
            keyframeData["frameNo"] = keyframe->mFrameNo;

            if (keyframe->mpKeyFrameData)
            {
                KeyFrameData* pKeyFrameData = keyframe->mpKeyFrameData;
                keyframeData["sourcePath"] = pKeyFrameData->mSpriteDescriptor.mSourcePath.toStdString();
                keyframeData["blur"] = pKeyFrameData->mSpriteDescriptor.mBlur;
                keyframeData["lookAtTarget"] = pKeyFrameData->mSpriteDescriptor.mLookAtTarget;
                keyframeData["relativeToTarget"] = pKeyFrameData->mSpriteDescriptor.mRelativeToTarget;

                Json::Value textureRect;
                textureRect[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mX;
                textureRect[1] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mY;
                textureRect[2] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mWidth;
                textureRect[3] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mHeight;
                keyframeData["textureRect"] = textureRect;

                keyframeData["blendType"] = GLSprite::blendTypeSting[pKeyFrameData->mSpriteDescriptor.mBlendType].toStdString();

                Json::Value centerPoint;
                centerPoint[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mCenter.mX;
                centerPoint[1] = pKeyFrameData->mSpriteDescriptor.mCenter.mY;
                keyframeData["center"] = centerPoint;

                Json::Value scale;
                scale[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mScale.mX;
                scale[1] = pKeyFrameData->mSpriteDescriptor.mScale.mY;
                keyframeData["scale"] = scale;

                Json::Value position;
                position[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mPosition.mX;
                position[1] = pKeyFrameData->mSpriteDescriptor.mPosition.mY;
                position[2] = pKeyFrameData->mSpriteDescriptor.mPosition.mZ;
                keyframeData["position"] = position;

                Json::Value rotation;
                rotation[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mRotation.mX;
                rotation[1] = pKeyFrameData->mSpriteDescriptor.mRotation.mY;
                rotation[2] = pKeyFrameData->mSpriteDescriptor.mRotation.mZ;
                keyframeData["rotation"] = rotation;

                keyframeData["alpha"] = pKeyFrameData->mSpriteDescriptor.mAlpha;

                keyframeData["alphaTween"] = KeyFrameData::tweenTypeSting[ pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_alpha]].toStdString();
                keyframeData["positionTween"] = KeyFrameData::tweenTypeSting[ pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_position]].toStdString();
                keyframeData["rotationTween"] = KeyFrameData::tweenTypeSting[ pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_rotation]].toStdString();
                keyframeData["scaleTween"] = KeyFrameData::tweenTypeSting[ pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_scale]].toStdString();
                keyframeData["isEmpty"] = false;
            }
            else
            {
                keyframeData["isEmpty"] = true;
            }
            keyframesData[i][j] = keyframeData;
        }
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

    Json::Value root;
    Json::Reader reader;

    std::ifstream ifs(path.toStdString().c_str());
    std::string inputJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    //std::string json_doc = readInputFile(path);
    if(!reader.parse(inputJson, root))
    {
        QString error = QString("File %0 is not correct JSON format").arg(path);
        printf("%s", error.toStdString().c_str());
        //QMessageBox::information(mpParent->window(), tr("File loading error"), tr(error.toStdString().c_str()));
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
    for (unsigned int i = 0; i < MaxLineNo; i++)
    {
        for (unsigned int j = 0; j < lines[i].size(); j++)
        {
            Json::Value& keyframe = lines[i][j];

            int frameNo = keyframe["frameNo"].asInt();
            int lineNo = keyframe["lineNo"].asInt();
            KeyFrame* pKeyFrame = NULL;
            if (!keyframe["isEmpty"].asBool())
            {
                KeyFrameData* pKeyFrameData = new KeyFrameData();
                pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_alpha] = KeyFrameData::getTweenTypeByString(QString::fromStdString(keyframe["alphaTween"].asString()));
                pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_position] = KeyFrameData::getTweenTypeByString(QString::fromStdString(keyframe["positionTween"].asString()));
                pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_rotation] = KeyFrameData::getTweenTypeByString(QString::fromStdString(keyframe["rotationTween"].asString()));
                pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_scale] = KeyFrameData::getTweenTypeByString(QString::fromStdString(keyframe["scaleTween"].asString()));

                pKeyFrameData->mSpriteDescriptor.mBlur = keyframe["blur"].asBool();
                pKeyFrameData->mSpriteDescriptor.mLookAtTarget = keyframe["lookAtTarget"].asBool();
                pKeyFrameData->mSpriteDescriptor.mRelativeToTarget = keyframe["relativeToTarget"].asBool();

                pKeyFrameData->mSpriteDescriptor.mSourcePath = QString::fromStdString(keyframe["sourcePath"].asString());
                pKeyFrameData->mSpriteDescriptor.mBlendType = GLSprite::getBlendTypeByString(QString::fromStdString(keyframe["blendType"].asString()));

                pKeyFrameData->mSpriteDescriptor.mAlpha = keyframe["alpha"].asDouble();

                pKeyFrameData->mSpriteDescriptor.mPosition.mX = keyframe["position"][static_cast<unsigned int>(0)].asInt();
                pKeyFrameData->mSpriteDescriptor.mPosition.mY = keyframe["position"][1].asInt();
                pKeyFrameData->mSpriteDescriptor.mPosition.mZ = keyframe["position"][2].asInt();

                pKeyFrameData->mSpriteDescriptor.mRotation.mX = keyframe["rotation"][static_cast<unsigned int>(0)].asInt();
                pKeyFrameData->mSpriteDescriptor.mRotation.mY = keyframe["rotation"][1].asInt();
                pKeyFrameData->mSpriteDescriptor.mRotation.mZ = keyframe["rotation"][2].asInt();

                pKeyFrameData->mSpriteDescriptor.mScale.mX = keyframe["scale"][static_cast<unsigned int>(0)].asDouble();
                pKeyFrameData->mSpriteDescriptor.mScale.mY = keyframe["scale"][1].asDouble();

                pKeyFrameData->mSpriteDescriptor.mCenter.mX = keyframe["center"][static_cast<unsigned int>(0)].asInt();
                pKeyFrameData->mSpriteDescriptor.mCenter.mY = keyframe["center"][1].asInt();

                pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mX = keyframe["textureRect"][static_cast<unsigned int>(0)].asInt();
                pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mY = keyframe["textureRect"][1].asInt();
                pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mWidth = keyframe["textureRect"][2].asInt();
                pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mHeight = keyframe["textureRect"][3].asInt();

                pKeyFrame = new KeyFrame(lineNo, frameNo, pKeyFrameData);
            }
            else
            {
                pKeyFrame = new KeyFrame(lineNo, frameNo, NULL);
            }
            mKeyFrames[i].push_back(pKeyFrame);
        }
    }

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
