#include "AnimationModel.h"
#include <QPixmap>
#include "Common.h"
#include "Macros.h"
#include "json/writer.h"
#include "json/reader.h"
#include <fstream>
#include <sstream>
#include "KeyFrameData.h"

// Macros
// Tween fields
#define TweenValue(field, startValue, endValue, frameNo, startFrameNo, endFrameNo)\
field = LERP(startValue, endValue, frameNo, startFrameNo, endFrameNo)

#define Tween(keyframeData, tweenType, startValue, endValue, field, frameNo, startFrameNo, endFrameNo)\
{\
 switch (startKeyFrameData->m##tweenType){\
    case KeyFrameData::eTT_Linear:\
        TweenValue(keyframeData->m##field, startValue, endValue, frameNo, startFrameNo, endFrameNo);\
    break;\
    case KeyFrameData::eTT_Fix:\
        keyframeData->m##field = startKeyFrameData->m##field;\
    break;\
    default:\
    break;\
    }\
}
////
AnimationModel::AnimationModel()
    : mAnimationName(QString(""))

{
    mSelectedSourcePath = "";

    mSelectedKeyFramePosition.mFrameNo = -1;
    mSelectedKeyFramePosition.mLineNo = -1;

    // Target sprite
    mpTargetPixmap = new QPixmap(":/resource/target.png");
    GLSprite::SpriteDescriptor spriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();

    spriteDescriptor.mTextureSrcRect.mWidth = 32;
    spriteDescriptor.mTextureSrcRect.mHeight = 32;
    spriteDescriptor.mCenter.mX = 16;
    spriteDescriptor.mCenter.mY = 16;
    spriteDescriptor.mPosition.mX = -100;

    mpTargetSprite = new GLSprite(-1, spriteDescriptor, false, 0, mpTargetPixmap);

    // Center point Sprite
    mpCenterPointPixmap = new QPixmap(":/resource/center_point.png");
    spriteDescriptor = GLSprite::makeDefaultSpriteDescriptor();
    spriteDescriptor.mTextureSrcRect.mWidth = 8;
    spriteDescriptor.mTextureSrcRect.mHeight = 8;
    spriteDescriptor.mCenter.mX = 4;
    spriteDescriptor.mCenter.mY = 4;

    mpCenterPointSprite = new GLSprite(-1, spriteDescriptor, false, 0, mpCenterPointPixmap);
}

AnimationModel::~AnimationModel()
{
    clearPixmapHash();

    delete mpTargetSprite;
    delete mpTargetPixmap;

    delete mpCenterPointSprite;
    delete mpCenterPointPixmap;
}

void AnimationModel::setAnimationName(QString name)
{
    if (mAnimationName != name)
    {
        emit animationNameChanged(name);
    }
}

void AnimationModel::clearPixmapHash()
{
    QHash<QString, QPixmap*>::Iterator iter = mSourceImageHash.begin();
    while(iter != mSourceImageHash.end())
    {
        delete iter.value();
        iter++;
    }
    mSourceImageHash.clear();
}

int AnimationModel::getMaxFrameCount()
{
    int max = 0;
    for (int i = 0; i < MaxLineNo; i++)
    {
        if (mKeyFrames[i].count() > 0)
        {
            int t = mKeyFrames[i].last()->mFrameNo;
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

        emit refreshTimeLine(lineNo);
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

        emit refreshTimeLine(lineNo);
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

        emit refreshTimeLine(lineNo);
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

        emit refreshTimeLine(lineNo);
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

        emit refreshTimeLine(lineNo);
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

    emit refreshTimeLine(lineNo);
}

void AnimationModel::clearAllKeyFrames()
{
    for (int i = 0; i < mKeyFrames[0].count(); i++)
    {
        for (int j = mKeyFrames[i].count() - 1; i >= 0; j--)
        {
            delete mKeyFrames[i][j];
            mKeyFrames[i].removeAt(j);
        }
    }
    emit refreshTimeLine();
}



const QList<KeyFrame*>& AnimationModel::getKeyFrameList(int lineNo) const
{
    return mKeyFrames[lineNo];
}

const QList<KeyFrame*> AnimationModel::createKeyFrameListAt(int frameNo) const
{
    QList<KeyFrame*> keyframeList;
    for (int lineNo = 0; lineNo < MaxLineNo; lineNo++)
    {
        KeyFrame* pKeyFrame = tweenFrame(lineNo, frameNo);
        if (pKeyFrame)
        {
            keyframeList.push_back(pKeyFrame);
        }
    }

    return keyframeList;
}

// Palets
QPixmap* AnimationModel::getPixmap(QString path)
{
    if (!mSourceImageHash.contains(path))
    {
        QString rootPath = QDir::currentPath();
        QString fullPath = rootPath.append("/").append(ROOT_RESOURCE_DIR.path()).append(path);
        try
        {
            mSourceImageHash.insert(path, new QPixmap(fullPath));
        }
        catch(long)
        {
            printf("File:%s couldn't be loaded.", path.toStdString().c_str());
            return NULL;
        }
    };

    return mSourceImageHash[path];
}

// Palet No (set / get)
void AnimationModel::setSelectedSourcePath(QString path)
{
    if (mSelectedSourcePath != path)
    {
        mSelectedSourcePath = path;
        emit selectedPaletChanged(path);
    }

}

QString AnimationModel::getSelectedSourcePath() const
{
    return mSelectedSourcePath;
}

void AnimationModel::setTargetSpritePosition(int x, int y)
{
    if (mpTargetSprite->mSpriteDescriptor.mPosition.mX != x || mpTargetSprite->mSpriteDescriptor.mPosition.mY != y)
    {
        mpTargetSprite->mSpriteDescriptor.mPosition.mX = x;
        mpTargetSprite->mSpriteDescriptor.mPosition.mY = y;

        emit targetPositionMoved(x, y);
    }
}

GLSprite* AnimationModel::getTargetSprite() const
{
    return mpTargetSprite;
}

GLSprite* AnimationModel::getCenterPointSprite() const
{
    return mpCenterPointSprite;
}

void AnimationModel::tweenElement(KeyFrameData* keyframeData, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData* startKeyFrameData, KeyFrameData* endKeyFrameData, int frameNo, int startFrameNo, int endFrameNo) const
{
    GLSprite::SpriteDescriptor& startDescriptor = startKeyFrameData->mSpriteDescriptor;
    GLSprite::SpriteDescriptor& endDescriptor = endKeyFrameData->mSpriteDescriptor;

    switch(tweenAttribute)
    {
        case KeyFrameData::TweenAttribute_alpha:
            Tween(keyframeData, TweenTypes[KeyFrameData::TweenAttribute_alpha], startDescriptor.mAlpha, endDescriptor.mAlpha, SpriteDescriptor.mAlpha, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_position:
            float startX = startDescriptor.mPosition.mX;
            float startY = startDescriptor.mPosition.mY;
            float endX = endDescriptor.mPosition.mX;
            float endY = endDescriptor.mPosition.mY;
            if (!startDescriptor.mRelativeToTarget && endDescriptor.mRelativeToTarget)
            {
                endX += mpTargetSprite->mSpriteDescriptor.mPosition.mX;
                endY += mpTargetSprite->mSpriteDescriptor.mPosition.mY;
            }
            else if (startDescriptor.mRelativeToTarget && !endDescriptor.mRelativeToTarget)
            {
                endX -= mpTargetSprite->mSpriteDescriptor.mPosition.mX;
                endY -= mpTargetSprite->mSpriteDescriptor.mPosition.mY;
            }

            Tween(keyframeData, TweenTypes[KeyFrameData::TweenAttribute_position], startX, endX, SpriteDescriptor.mPosition.mX, frameNo, startFrameNo, endFrameNo);
            Tween(keyframeData, TweenTypes[KeyFrameData::TweenAttribute_position], startY, endY, SpriteDescriptor.mPosition.mY, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_rotation:
            float startRotationX = startDescriptor.mRotation.mX;
            float endRotationX = endDescriptor.mRotation.mX;
            if (endDescriptor.mLookAtTarget)
            {
                endRotationX += mpTargetSprite->mSpriteDescriptor.mRotation.mX;
            }

            Tween(keyframeData, TweenTypes[KeyFrameData::TweenAttribute_rotation], startRotationX, endRotationX, SpriteDescriptor.mRotation.mX, frameNo, startFrameNo, endFrameNo);
            break;
        case KeyFrameData::TweenAttribute_scale:
            Tween(keyframeData, TweenTypes[KeyFrameData::TweenAttribute_scale], startDescriptor.mScale.mX, endDescriptor.mScale.mX, SpriteDescriptor.mScale.mX, frameNo, startFrameNo, endFrameNo);
            Tween(keyframeData, TweenTypes[KeyFrameData::TweenAttribute_scale], startDescriptor.mScale.mY, endDescriptor.mScale.mY, SpriteDescriptor.mScale.mY, frameNo, startFrameNo, endFrameNo);
            break;
        default:
            break;
    }
}

// Return true if it find a cel to tween, if not return false;
KeyFrame* AnimationModel::tweenFrame(int lineNo, int frameNo) const
{
    if (mKeyFrames[lineNo].count() == 0) {return NULL;}

    // Set up base for keyframe.(inherit textureID etc from previous keyframe
    KeyFrameData* pKeyFrameData = new KeyFrameData();
    int baseIndex = getPreviousKeyFrameIndex(lineNo, frameNo, KeyFrameData::TweenAttribute_any);

    // No valid keyframe has been found before specified cel position
    if (baseIndex < 0){return NULL;}

    KeyFrame* pBaseKeyFrame = mKeyFrames[lineNo][baseIndex];
    KeyFrameData* pBaseKeyFrameData = pBaseKeyFrame->mpKeyFrameData;

    // If the keyframe is empty, don't generate any temporaly keyframe
    if (!pBaseKeyFrameData) {return NULL;}

    pKeyFrameData->mIsTweenCel = (pBaseKeyFrame->mFrameNo != frameNo);
    pKeyFrameData->mSpriteDescriptor = pBaseKeyFrameData->mSpriteDescriptor;

    // Tween for each attribute
    bool noTweenFound = true;
    for (int i = 0; i < KeyFrameData::TweenAttribute_COUNT; i++)
    {
        KeyFrameData::TweenAttribute tweenAttribute = (KeyFrameData::TweenAttribute)i;
        int startIndex = getPreviousKeyFrameIndex(lineNo, frameNo, (KeyFrameData::TweenAttribute)i);
        int endIndex = getNextKeyFrameIndex(lineNo, frameNo, tweenAttribute);
        if (endIndex >= 0) { noTweenFound = false; }
        if (startIndex >= 0 && endIndex > startIndex)
        {
            KeyFrame* pStartKeyFrame = mKeyFrames[lineNo][startIndex];
            KeyFrame* pEndKeyFrame = mKeyFrames[lineNo][endIndex];
            tweenElement(pKeyFrameData, tweenAttribute, pStartKeyFrame->mpKeyFrameData, pEndKeyFrame->mpKeyFrameData, frameNo, pStartKeyFrame->mFrameNo, pEndKeyFrame->mFrameNo);
        }
    }

    if (noTweenFound)
    {
        delete pKeyFrameData;
        return NULL;
    }
    else
    {
        return new KeyFrame(lineNo, frameNo, pKeyFrameData);
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

void AnimationModel::saveData()
{
//    Json::Value root;
//
//    // save animation name
//    root["name"] = mAnimationName;
//
//    // save keyframes
//    Json::Value& keyframesData = root["keyframes"];
//    keyframesData.resize(MaxLineNo);
//    for (int i = 0; i < MaxLineNo; i++)
//    {
//        keyframesData[i].resize(mKeyFrames[i].count());
//
//        for (unsigned int j = 0; j < mKeyFrames[i].count(); j++)
//        {
//            const KeyFrame& keyframe = mKeyFrames[i][j];
//
//            Json::Value keyframeData;
//            keyframeData["lineNo"] = keyframe.mLineNo;
//            keyframeData["frameNo"] = keyframe.mFrameNo;
//
//            if (keyframe.mpKeyFrameData)
//            {
//                KeyFrameData* pKeyFrameData = keyframe.mpKeyFrameData;
//                keyframeData["textureID"] = pKeyFrameData->mTextureID;
//                keyframeData["blur"] = pKeyFrameData->mBlur;
//                keyframeData["lookAtTarget"] = pKeyFrameData->mLookAtTarget;
//                keyframeData["relativeToTarget"] = pKeyFrameData->mRelativeToTarget;
//
//                Json::Value textureRect;
//                textureRect[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mX;
//                textureRect[1] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mY;
//                textureRect[2] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mWidth;
//                textureRect[3] = pKeyFrameData->mSpriteDescriptor.mTextureSrcRect.mHeight;
//                keyframeData["textureRect"] = textureRect;
//
//                keyframeData["blendType"] = pKeyFrameData->mSpriteDescriptor.mBlendType;
//
//                Json::Value centerPoint;
//                centerPoint[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mCenter.mX;
//                centerPoint[1] = pKeyFrameData->mSpriteDescriptor.mCenter.mY;
//                keyframeData["centerPoint"] = centerPoint;
//
//                Json::Value scale;
//                scale[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mScale.mX;
//                scale[1] = pKeyFrameData->mSpriteDescriptor.mScale.mY;
//                keyframeData["scale"] = scale;
//
//                Json::Value position;
//                position[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mPosition.mX;
//                position[1] = pKeyFrameData->mSpriteDescriptor.mPosition.mY;
//                position[2] = pKeyFrameData->mSpriteDescriptor.mPosition.mZ;
//                keyframeData["position"] = position;
//
//                Json::Value rotation;
//                rotation[static_cast<unsigned int>(0)] = pKeyFrameData->mSpriteDescriptor.mRotation.mX;
//                rotation[1] = pKeyFrameData->mSpriteDescriptor.mRotation.mY;
//                rotation[2] = pKeyFrameData->mSpriteDescriptor.mRotation.mZ;
//                keyframeData["rotation"] = rotation;
//
//                keyframeData["alpha"] = pKeyFrameData->mSpriteDescriptor.mAlpha;
//
//                keyframeData["alphaTween"] = pKeyFrameData->mTweenTypes[KeyFrame::TweenAttribute_alpha];
//                keyframeData["positionTween"] = pKeyFrameData->mTweenTypes[KeyFrame::TweenAttribute_position];
//                keyframeData["rotationTween"] = pKeyFrameData->mTweenTypes[KeyFrame::TweenAttribute_rotation];
//                keyframeData["scaleTween"] = pKeyFrameData->mTweenTypes[KeyFrame::TweenAttribute_scale];
//                keyframesData[i][j] = keyframeData;
//            }
//        }
//    }
//
//    Json::StyledWriter writer;
//    std::string outputJson = writer.write(root);
//    std::ofstream ofs;
//    ofs.open("test.json");
//    ofs << outputJson << std::endl;
//
//    ofs.close();
}

void AnimationModel::loadData()
{
//    mKeyFrames.clear();
//    emit clearKeyframes();
//
//    Json::Value root;
//    Json::Reader reader;
//
//    std::ifstream ifs("test.json");
//    std::string inputJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
//
//    //std::string json_doc = readInputFile(path);
//    if(!reader.parse(inputJson, root))
//    {
//        std::string error_message = reader.getFormatedErrorMessages();
//        printf("JSON error:%s\n", error_message.c_str());
//        exit(EXIT_FAILURE);
//    }
//
//    Json::Value& palets = root["palets"];
//    for (int i = 0; i < ImagePaletCount; i++)
//    {
//        QString id = QString::fromStdString(palets[i].asString());
//        setAnimationImagePalet(i, id);
//    }
//
//    Json::Value& lines = root["lines"];
//    unsigned int lineCount = lines.size();
//    for (unsigned int i = 0; i < lineCount; i++)
//    {
//        KeyFrame keyframe;
//        for (unsigned int j = 0; j < keyframes[i].size(); j++)
//        {
//            Json::Value& keyframe = keyframes[i][j];
//            KeyFrame keyframe;
//            keyframe.mFrameNo = keyframe["frameNo"].asInt();
//            keyframe.mLineNo = keyframe["lineNo"].asInt();
//            keyframe.mTextureID = keyframe["textureID"].asInt();
//            keyframe.mBlur = keyframe["blur"].asBool();
//            keyframe.mLookAtTarget = keyframe["lookAtTarget"].asBool();
//            keyframe.mRelativeToTarget = keyframe["relativeToTarget"].asBool();
//
//            keyframe.mTweenTypes[KeyFrame::TweenAttribute_alpha] = (KeyFrame::TweenType)(cel["alphaTween"].asInt());
//            keyframe.mTweenTypes[KeyFrame::TweenAttribute_position] = (KeyFrame::TweenType)(cel["positionTween"].asInt());
//            keyframe.mTweenTypes[KeyFrame::TweenAttribute_rotation] = (KeyFrame::TweenType)(cel["rotationTween"].asInt());
//            keyframe.mTweenTypes[KeyFrame::TweenAttribute_scale] = (KeyFrame::TweenType)(cel["scaleTween"].asInt());
//
//            keyframe.mSpriteDescriptor.mBlendType = (GLSprite::BlendType)(cel["blendType"].asInt());
//
//            keyframe.mSpriteDescriptor.mAlpha = cel["alpha"].asDouble();
//
//            keyframe.mSpriteDescriptor.mPosition.mX = cel["position"][static_cast<unsigned int>(0)].asDouble();
//            keyframe.mSpriteDescriptor.mPosition.mY = cel["position"][1].asDouble();
//            keyframe.mSpriteDescriptor.mPosition.mZ = cel["position"][2].asDouble();
//
//            keyframe.mSpriteDescriptor.mRotation.mX = cel["rotation"][static_cast<unsigned int>(0)].asDouble();
//            keyframe.mSpriteDescriptor.mRotation.mY = cel["rotation"][1].asDouble();
//            keyframe.mSpriteDescriptor.mRotation.mZ = cel["rotation"][2].asDouble();
//
//            keyframe.mSpriteDescriptor.mScale.mX = cel["scale"][static_cast<unsigned int>(0)].asDouble();
//            keyframe.mSpriteDescriptor.mScale.mY = cel["scale"][1].asDouble();
//
//            keyframe.mSpriteDescriptor.mCenter.mX = cel["center"][static_cast<unsigned int>(0)].asDouble();
//            keyframe.mSpriteDescriptor.mCenter.mY = cel["center"][1].asDouble();
//
//            keyframe.mSpriteDescriptor.mTextureSrcRect.mX = cel["textureRect"][static_cast<unsigned int>(0)].asInt();
//            keyframe.mSpriteDescriptor.mTextureSrcRect.mY = cel["textureRect"][1].asInt();
//            keyframe.mSpriteDescriptor.mTextureSrcRect.mWidth = cel["textureRect"][2].asInt();
//            keyframe.mSpriteDescriptor.mTextureSrcRect.mHeight = cel["textureRect"][3].asInt();
//            keyframe.mIsTweenCel = false;
//
//            keyframe.mCelHash.insert(keyframe.mLineNo, keyframe);
//        }
//
//        mKeyFrames.push_back(keyframe);
//        emit keyFrameAdded(MaxLineNo - 1, keyframe.mComment);
//    }
//
//    resetTweenHash();
//    calculateAnimationLength();
//
//    // make it emit frame change event
//    mCurrentFrameNo = -1;
//    setCurrentKeyFrameNo(0);
}
