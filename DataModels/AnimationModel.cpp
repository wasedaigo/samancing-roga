#include "AnimationModel.h"
#include <QPixmap>
#include "Common.h"
#include "Macros.h"
#include "json/writer.h"
#include "json/reader.h"
#include <fstream>
#include <sstream>
#include <QPixmap>

// Macros
// Tween fields
#define TweenValue(celData, startCelData, endCelData, field, frameNo, startFrameNo, endFrameNo)\
celData.m##field = LERP(startCelData.m##field, endCelData.m##field, frameNo, startFrameNo, endFrameNo)

#define Tween(celData, tweenType, startCelData, endCelData, field, frameNo, startFrameNo, endFrameNo)\
{\
 switch (startCelData.m##tweenType){\
    case CelModel::eTT_Linear:\
        TweenValue(celData, startCelData, endCelData, field, frameNo, startFrameNo, endFrameNo);\
    break;\
    case CelModel::eTT_Fix:\
        celData.m##field = startCelData.m##field;\
    break;\
    default:\
    break;\
    }\
}
////
AnimationModel::AnimationModel()
    : mAnimationName(QString("")),
      mCurrentKeyFrameNo(-1),
      mCurrentFrameNo(-1),
      mSelectedPaletNo(-1),
      mAnimationDuration(0)

{
    for (int i = 0; i < ImagePaletCount; i++)
    {
        mImagePalets[i] = QString("");
        mpPixmaps[i] = NULL;
    }
}

AnimationModel::~AnimationModel()
{
    for (int i = 0; i < ImagePaletCount; i++)
    {
        delete mpPixmaps[i];
    }
}

int AnimationModel::getAnimationDuration()
{
    int total = 0;
    QList<KeyFrame>::Iterator iter = mKeyFrameList.begin();
    while (iter != mKeyFrameList.end())
    {
        KeyFrame keyFrame = (KeyFrame)*iter;
        total += keyFrame.mDuration;

        iter++;
    }

    return total;
}

void AnimationModel::calculateAnimationDuration()
{
    int total = getAnimationDuration();

    if (mAnimationDuration != total)
    {
        mAnimationDuration = total;
        emit animationDurationChanged(total);
    }
}

// Clear all Tween data
void AnimationModel::clearTweenHash()
{
    for (int i = 0; i < mKeyFrameList.count(); i++)
    {
        mKeyFrameList[i].mTweenIDList.clear();
    }
    mTweenHash.clear();
}

inline void AnimationModel::addTweenData(int keyFrameNo, const CelModel::CelData& celData, CelModel::TweenAttribute tweenAttribute)
{
    TweenData tweenData;
    tweenData.mTweenAttribute = tweenAttribute;
    tweenData.mCelNo = celData.mCelNo;
    tweenData.mStartKeyFrameNo = keyFrameNo;
    tweenData.mEndKeyFrameNo = -1;

    // Get next available unique tweenCelID
    int tweenCelID = 0;
    while (mTweenHash.contains(tweenCelID))
    {
        tweenCelID++;
    }

    // Add tween data to keyframe itself
    mKeyFrameList[keyFrameNo].mTweenIDList.push_front(tweenCelID);

    // Iterate through keyframes till it finds a cel of the same cel no
    // Add reference to previous tween data
    if (celData.mTweenTypes[tweenAttribute] != CelModel::eTT_Fix)
    {
        bool hitEnd = false;
        for (int i = keyFrameNo + 1; i < mKeyFrameList.count(); i++)
        {
            CelModel::CelData* endCelData = getCelDataRef(i, celData.mCelNo);
            if (endCelData && endCelData->mTweenTypes[tweenAttribute] != CelModel::eTT_None)
            {
                tweenData.mEndKeyFrameNo = i;
                hitEnd = true;
                break;
            }
            else
            {
                // Add reference to previous tween data
                mKeyFrameList[i].mTweenIDList.push_front(tweenCelID);
            }
        }

        // If it reached the end without hitting target tween keyframe
        // Remove all data
        if (!hitEnd)
        {
            for (int i = keyFrameNo + 1; i < mKeyFrameList.count(); i++)
            {
                mKeyFrameList[i].mTweenIDList.removeOne(tweenCelID);
            }
        }
    }
    mTweenHash.insert(tweenCelID, tweenData);
}

// Reset all Tween data
void AnimationModel::resetTweenHash()
{
    clearTweenHash();
    // Iterate through keyframes
    for (int i = 0; i < mKeyFrameList.count(); i++)
    {
        QHash<int, CelModel::CelData>& celHash = mKeyFrameList[i].mCelHash;
        QHash<int, CelModel::CelData>::Iterator iter = celHash.begin();
        // Iterate through cels
        while (iter != celHash.end())
        {
            CelModel::CelData& celData = iter.value();

            // Add keyframe for tweens for each attribute
            for (int j = 0; j < CelModel::TweenAttribute_COUNT; j++)
            {
                if (celData.mTweenTypes[j] == CelModel::eTT_None) {continue;}
                addTweenData(i, celData, (CelModel::TweenAttribute)j);
            }
            iter++;
        }
    }
}

//Celdata control

// Add passed celdata to the list, it will set unique celNo.
void AnimationModel::addCelData(int keyFrameNo, const GLSprite::Point2& position)
{
    CelModel::CelData celData = makeDefaultCelData();
    celData.mSpriteDescriptor.mPosition = position;
    celData.mSpriteDescriptor.mTextureSrcRect = mSelectedPaletTextureSrcRect;
    celData.mTextureID = mSelectedPaletNo;

    QHash<int, CelModel::CelData>& celHash = mKeyFrameList[keyFrameNo].mCelHash;
    int celNo = 0;
    while (celHash.contains(celNo))
    {
        celNo++;
    }
    celData.mCelNo = celNo;

    celHash.insert(celNo, celData);

    resetTweenHash();
    emit celAdded(celData);
}

int AnimationModel::changeCelNo(int keyFrameNo, int prevCelNo, int newCelNo)
{
    // nothing has changed
    if (newCelNo == prevCelNo) { return prevCelNo; }

    // reference to selected celNo
    CelModel::CelData* pCelData = getCelDataRef(keyFrameNo, prevCelNo);
    QHash<int, CelModel::CelData>& celHash = mKeyFrameList[keyFrameNo].mCelHash;

    // increment or decrement?
    int d = 0;
    if (newCelNo > prevCelNo)
    {
        d = 1;
    }
    else
    {
        d = -1;
    }

    // Search for unique celNo in this keyFrame
    int celNo = newCelNo;
    while (celNo >= -1 && celHash.contains(celNo))
    {
        celNo += d;
    }

    // nothing has changed
    if (celNo == -1 || prevCelNo == celNo){ return prevCelNo; }

    // change celNo
    pCelData->mCelNo = celNo;

    // Change the key of celHash
    celHash.remove(prevCelNo);
    celHash.insert(celNo, *pCelData);

    return celNo;
}

void AnimationModel::setCelData(int keyFrameNo, int celNo, const CelModel::CelData& celData)
{
    if (keyFrameNo >= 0 && keyFrameNo < mKeyFrameList.count())
    {
        mKeyFrameList[getCurrentKeyFrameNo()].mCelHash.insert(celNo, celData);

        resetTweenHash();
    }
}

// Remove cel data
void AnimationModel::removeCelData(int keyFrameNo, int celNo)
{
    QHash<int, CelModel::CelData>& celHash = mKeyFrameList[keyFrameNo].mCelHash;
    
    if (celHash.contains(celNo))
    {
        CelModel::CelData celData = celHash[celNo];
        celHash.remove(celNo);
        emit celRemoved(celData);
    }

    resetTweenHash();
}

//Keyframe control
// Add new keyframe
void AnimationModel::addEmptyKeyFrame()
{
    const KeyFrame keyframe = AnimationModel::makeEmptyKeyFrame();
    mKeyFrameList.push_back(keyframe);
    emit keyFrameAdded(mKeyFrameList.count() - 1, keyframe.mDuration, keyframe.mComment);

    calculateAnimationDuration();
    resetTweenHash();
}

void AnimationModel::insertEmptyKeyFrame(int index)
{
    const KeyFrame keyframe = AnimationModel::makeEmptyKeyFrame();
    mKeyFrameList.insert(index, keyframe);
    emit keyFrameAdded(index, keyframe.mDuration, keyframe.mComment);

    calculateAnimationDuration();
    resetTweenHash();
}

void AnimationModel::removeKeyFrame(int index)
{
    if (mKeyFrameList.count() > 1)
    {
        mKeyFrameList.removeAt(index);
        emit keyFrameRemoved(index);

        if (mKeyFrameList.count() == 0)
        {
            setCurrentKeyFrameNo(-1);
        }
        else
        {
            // in order to emit frameNo change signal
            mCurrentFrameNo = -1;
            int newIndex = std::min(index, mKeyFrameList.count() - 1);
            setCurrentKeyFrameNo(newIndex);
        }
        resetTweenHash();
    }

    calculateAnimationDuration();
}

void AnimationModel::clearAllKeyFrames()
{
    mKeyFrameList.clear();
}

// Setter / Getter

QPixmap* AnimationModel::getPixmap(int paletNo) const
{
    if (paletNo < 0 || paletNo >= ImagePaletCount)
    {
        return NULL;
    }
    else
    {
        return mpPixmaps[paletNo];
    }
}

QString AnimationModel::getAnimationPaletID(int paletNo) const
{
    return mImagePalets[paletNo];
}

void AnimationModel::setAnimationImagePalet(int paletNo, QString id)
{
    if (id != "")
    {
        mImagePalets[paletNo] = id;
        QString filename = ANIMATION_IMAGE_DIR.filePath(QString("%0.%1").arg(id, IMAGE_FORMAT));
        mpPixmaps[paletNo] = new QPixmap(filename);

        emit animationImagePaletChanged(paletNo, id);
    }
}

//
// Frame control
//
bool AnimationModel::isKeyFrame(int frameNo)
{
    int keyFrameNo = getKeyFrameNoByFrameNo(frameNo);
    return frameNo == getFrameNoByKeyFrameNo(keyFrameNo);
}

int AnimationModel::getFrameNoByKeyFrameNo(int keyFrameNo)
{
    int no = 0;
    int total = 0;
    QList<KeyFrame>::Iterator iter = mKeyFrameList.begin();
    while (iter != mKeyFrameList.end())
    {
        if (keyFrameNo == no)
        {
            break;
        }
        KeyFrame keyFrame = (KeyFrame)*iter;
        total += keyFrame.mDuration;

        iter++;
        no++;
    }
    return total;
}


int AnimationModel::getKeyFrameNoByFrameNo(int frameNo)
{
    int keyFrameNo = mCurrentKeyFrameNo;
    int no = 0;
    int total = 0;
    QList<KeyFrame>::Iterator iter = mKeyFrameList.begin();
    while (iter != mKeyFrameList.end())
    {

        KeyFrame keyFrame = (KeyFrame)*iter;
        int nextTotal = total + keyFrame.mDuration;
        if (total <= frameNo && frameNo < nextTotal)
        {
            keyFrameNo = no;
            break;
        }
        total = nextTotal;
        iter++;
        no++;
    }
    return keyFrameNo;
}

int AnimationModel::getCurrentFrameNo() const
{
    return mCurrentFrameNo;
}

void AnimationModel::setCurrentFrameNo(int frameNo)
{
    if (mCurrentFrameNo != frameNo)
    {
        mCurrentFrameNo = frameNo;

        int keyFrameNo = getKeyFrameNoByFrameNo(frameNo);

        mCurrentKeyFrameNo = keyFrameNo;
        emit currentFrameNoChanged(frameNo);
    }
}

//
// Key Frame control
//
bool AnimationModel::isKeyFrameSelected()
{
    return isKeyFrame(mCurrentFrameNo);
}

// KeyFrame duration
void AnimationModel::setKeyFrameDuration(int index, int duration)
{
    KeyFrame& keyFrame = mKeyFrameList.begin()[index];
    if (keyFrame.mDuration != duration)
    {
        // illegal character will be automatically casted to 0
        // in that case we will force the item to change
        if (duration < 1)
        {
            duration = 1;
        }
        keyFrame.mDuration = duration;
        emit keyFrameDurationChanged(index, duration);
        calculateAnimationDuration();
    }
}

// KeyFrame comment
void AnimationModel::setKeyFrameComment(int index, QString comment)
{
    KeyFrame& keyFrame = mKeyFrameList[index];
    keyFrame.mComment = comment;
}

// KeyFrameNo (set / get)
int AnimationModel::getCurrentKeyFrameNo() const
{
    return mCurrentKeyFrameNo;
}
void AnimationModel::setCurrentKeyFrameNo(int keyFrameNo)
{
    int frameNo = getFrameNoByKeyFrameNo(keyFrameNo);
    setCurrentFrameNo(frameNo);

    if (mCurrentKeyFrameNo != keyFrameNo)
    {
        mCurrentKeyFrameNo = keyFrameNo;
    }
}

// Palet No (set / get)
void AnimationModel::setSelectedPaletNo(int paletNo)
{
    mSelectedPaletNo = paletNo;
}

int AnimationModel::getSelectedPaletNo() const
{
    return mSelectedPaletNo;
}

void AnimationModel::tweenElement(CelModel::CelData& celData, CelModel::TweenAttribute tweenAttribute, CelModel::CelData& startCelData, CelModel::CelData& endCelData, int frameNo, int startFrameNo, int endFrameNo)
{
    switch(tweenAttribute)
    {
        case CelModel::TweenAttribute_alpha:
            Tween(celData, TweenTypes[CelModel::TweenAttribute_alpha], startCelData, endCelData, SpriteDescriptor.mAlpha, frameNo, startFrameNo, endFrameNo);
            break;
        case CelModel::TweenAttribute_position:
            Tween(celData, TweenTypes[CelModel::TweenAttribute_position], startCelData, endCelData, SpriteDescriptor.mPosition.mX, frameNo, startFrameNo, endFrameNo);
            Tween(celData, TweenTypes[CelModel::TweenAttribute_position], startCelData, endCelData, SpriteDescriptor.mPosition.mY, frameNo, startFrameNo, endFrameNo);
            break;
        case CelModel::TweenAttribute_rotation:
            Tween(celData, TweenTypes[CelModel::TweenAttribute_rotation], startCelData, endCelData, SpriteDescriptor.mRotation.mX, frameNo, startFrameNo, endFrameNo);
            break;
        case CelModel::TweenAttribute_scale:
            Tween(celData, TweenTypes[CelModel::TweenAttribute_scale], startCelData, endCelData, SpriteDescriptor.mScale.mX, frameNo, startFrameNo, endFrameNo);
            Tween(celData, TweenTypes[CelModel::TweenAttribute_scale], startCelData, endCelData, SpriteDescriptor.mScale.mY, frameNo, startFrameNo, endFrameNo);
            break;
        default:
            break;
    }
}

// Return true if it find a cel to tween, if not return false;
void AnimationModel::tweenFrame(QHash<int, CelModel::CelData>& returnCelHash, CelModel::TweenAttribute tweenAttribute, int celNo, int startFrameNo, int frameNo, int startKeyFrameNo, int endKeyFrameNo)
{
    CelModel::CelData& startCelData = mKeyFrameList[startKeyFrameNo].mCelHash[celNo];

    int endFrameNo = -1;
    // endFrameNo = -1 is not going to be used
    if(endKeyFrameNo < 0)
    {
        endKeyFrameNo = startKeyFrameNo;
    }
    else
    {
        endFrameNo = getFrameNoByKeyFrameNo(endKeyFrameNo);
    }

    CelModel::CelData& endCelData = mKeyFrameList[endKeyFrameNo].mCelHash[celNo];

    // If specified celNo is already processed, tweak that instance
    if (returnCelHash.contains(celNo))
    {
        CelModel::CelData& celData = returnCelHash[celNo];
        tweenElement(celData, tweenAttribute, startCelData, endCelData, frameNo, startFrameNo, endFrameNo);
    }
    else
    {
        CelModel::CelData celData = makeDefaultCelData();
        celData.mSpriteDescriptor.mCenter = startCelData.mSpriteDescriptor.mCenter;
        celData.mSpriteDescriptor.mTextureSrcRect = startCelData.mSpriteDescriptor.mTextureSrcRect;
        celData.mCelNo = startCelData.mCelNo;
        celData.mTextureID = startCelData.mTextureID;
        celData.mLookAtTarget = startCelData.mLookAtTarget;
        celData.mRelativeToTarget = startCelData.mRelativeToTarget;
        celData.mBlur = startCelData.mBlur;
        celData.mIsTweenCel = true;
        celData.mSpriteDescriptor.mBlendType = startCelData.mSpriteDescriptor.mBlendType;
        tweenElement(celData, tweenAttribute, startCelData, endCelData, frameNo, startFrameNo, endFrameNo);
        returnCelHash.insert(celData.mCelNo, celData);
    }
}

// Cel Hash
QHash<int, CelModel::CelData> AnimationModel::getCelHashAt(int frameNo)
{
    int keyFrameNo = getKeyFrameNoByFrameNo(frameNo);
    QList<int>& tweenIDList = mKeyFrameList[keyFrameNo].mTweenIDList;
    QHash<int, CelModel::CelData> returnCelHash;

    for (int i = 0; i < tweenIDList.count(); i++)
    {
        TweenData& tweenData = mTweenHash[tweenIDList[i]];
        int startFrameNo = getFrameNoByKeyFrameNo(tweenData.mStartKeyFrameNo);

        // No tween data needed for tween starting keyframe   
        tweenFrame(returnCelHash, tweenData.mTweenAttribute, tweenData.mCelNo, startFrameNo, frameNo, tweenData.mStartKeyFrameNo, tweenData.mEndKeyFrameNo);
    }

    // Unite cels from keyframe
    if (isKeyFrame(frameNo))
    {
        QHash<int, CelModel::CelData> keyFrameCelHash = getCelHash(keyFrameNo);
        QHash<int, CelModel::CelData>::Iterator iter = keyFrameCelHash.begin();
        while(iter != keyFrameCelHash.end())
        {
            CelModel::CelData& celData = iter.value();
            if (returnCelHash.contains(celData.mCelNo))
            {
                for (int i = 0; i < CelModel::TweenAttribute_COUNT; i++)
                {
                    if(celData.mTweenTypes[i] == CelModel::eTT_None)
                    {
                        celData.copyAttribute((CelModel::TweenAttribute)i, returnCelHash[celData.mCelNo]);
                    }
                }
                returnCelHash.remove(celData.mCelNo);
            }

            returnCelHash.insert(celData.mCelNo, celData);
            iter++;
        }
    }

    return returnCelHash;
}

// Cel Hash
QHash<int, CelModel::CelData> AnimationModel::getCelHash(int keyFrameNo) const
{
    QHash<int, CelModel::CelData> celHash;
    if (keyFrameNo >= 0 && keyFrameNo < mKeyFrameList.count())
    {
        celHash = mKeyFrameList[keyFrameNo].mCelHash;
    }

    return celHash;
}

CelModel::CelData* AnimationModel::getCelDataRef(int keyFrameNo, int celNo)
{
    CelModel::CelData* celData = NULL;
     if (keyFrameNo >= 0 && keyFrameNo < mKeyFrameList.count())
    {
        if (mKeyFrameList[keyFrameNo].mCelHash.contains(celNo))
        {
            celData = &mKeyFrameList[keyFrameNo].mCelHash[celNo];
        }
    }
    return celData;
}

void AnimationModel::saveData()
{
    Json::Value root;

    // save animation name
    root["name"] = "TEST";

    // save animation image palet filenames
    Json::Value palets;
    palets.resize(ImagePaletCount);
    for (int i = 0; i < ImagePaletCount; i++)
    {
        palets[i] = mImagePalets[i].toStdString();
    }
    root["palets"] = palets;

    // save keyframes
    Json::Value& keyframes = root["keyframes"];
    keyframes.resize(mKeyFrameList.count());
    for (int i = 0; i < mKeyFrameList.count(); i++)
    {
        keyframes[i]["comment"] = mKeyFrameList[i].mComment.toStdString();
        keyframes[i]["duration"] = mKeyFrameList[i].mDuration;

        QHash<int, CelModel::CelData>& celHash = mKeyFrameList[i].mCelHash;
        keyframes[i]["cels"].resize(celHash.count());
        QHash<int, CelModel::CelData>::Iterator celIter = celHash.begin();
        int j = 0;
        while (celIter != celHash.end())
        {
            CelModel::CelData& celData = celIter.value();

            Json::Value cel;
            cel["celNo"] = celData.mCelNo;
            cel["textureID"] = celData.mTextureID;
            cel["blur"] = celData.mBlur;
            cel["lookAtTarget"] = celData.mLookAtTarget;
            cel["relativeToTarget"] = celData.mRelativeToTarget;

            Json::Value textureRect;
            textureRect[static_cast<unsigned int>(0)] = celData.mSpriteDescriptor.mTextureSrcRect.mX;
            textureRect[1] = celData.mSpriteDescriptor.mTextureSrcRect.mY;
            textureRect[2] = celData.mSpriteDescriptor.mTextureSrcRect.mWidth;
            textureRect[3] = celData.mSpriteDescriptor.mTextureSrcRect.mHeight;
            cel["textureRect"] = textureRect;

            cel["blendType"] = celData.mSpriteDescriptor.mBlendType;

            Json::Value centerPoint;
            centerPoint[static_cast<unsigned int>(0)] = celData.mSpriteDescriptor.mCenter.mX;
            centerPoint[1] = celData.mSpriteDescriptor.mCenter.mY;
            cel["centerPoint"] = centerPoint;

            Json::Value scale;
            scale[static_cast<unsigned int>(0)] = celData.mSpriteDescriptor.mScale.mX;
            scale[1] = celData.mSpriteDescriptor.mScale.mY;
            cel["scale"] = scale;

            Json::Value position;
            position[static_cast<unsigned int>(0)] = celData.mSpriteDescriptor.mPosition.mX;
            position[1] = celData.mSpriteDescriptor.mPosition.mY;
            position[2] = celData.mSpriteDescriptor.mPosition.mZ;
            cel["position"] = position;

            Json::Value rotation;
            rotation[static_cast<unsigned int>(0)] = celData.mSpriteDescriptor.mRotation.mX;
            rotation[1] = celData.mSpriteDescriptor.mRotation.mY;
            rotation[2] = celData.mSpriteDescriptor.mRotation.mZ;
            cel["rotation"] = rotation;

            cel["alpha"] = celData.mSpriteDescriptor.mAlpha;

            cel["alphaTween"] = celData.mTweenTypes[CelModel::TweenAttribute_alpha];
            cel["positionTween"] = celData.mTweenTypes[CelModel::TweenAttribute_position];
            cel["rotationTween"] = celData.mTweenTypes[CelModel::TweenAttribute_rotation];
            cel["scaleTween"] = celData.mTweenTypes[CelModel::TweenAttribute_scale];

            keyframes[i]["cels"][j] = cel;

            celIter++;
            j++;
        }
    }

    Json::StyledWriter writer;
    std::string outputJson = writer.write(root);
    std::ofstream ofs;
    ofs.open("test.json");
    ofs << outputJson << std::endl;

    ofs.close();
}

void AnimationModel::loadData()
{
    mKeyFrameList.clear();
    emit clearKeyframes();

    Json::Value root;
    Json::Reader reader;

    std::ifstream ifs("test.json");
    std::string inputJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    //std::string json_doc = readInputFile(path);
    if(!reader.parse(inputJson, root))
    {
        std::string error_message = reader.getFormatedErrorMessages();
        printf("JSON error:%s\n", error_message.c_str());
        exit(EXIT_FAILURE);
    }

    Json::Value& palets = root["palets"];
    for (int i = 0; i < ImagePaletCount; i++)
    {
        QString id = QString::fromStdString(palets[i].asString());
        setAnimationImagePalet(i, id);
    }

    Json::Value& keyframes = root["keyframes"];
    unsigned int keyFrameCount = keyframes.size();
    for (unsigned int i = 0; i < keyFrameCount; i++)
    {
        KeyFrame keyframe;

        keyframe.mComment = QString::fromStdString(keyframes[i]["comment"].asString());
        keyframe.mDuration = keyframes[i]["duration"].asInt();
        Json::Value& cels = keyframes[i]["cels"];
        for (unsigned int j = 0; j < cels.size(); j++)
        {
            Json::Value& cel = cels[j];
            CelModel::CelData celData;
            celData.mCelNo = cel["celNo"].asInt();
            celData.mTextureID = cel["textureID"].asInt();
            celData.mBlur = cel["blur"].asBool();
            celData.mLookAtTarget = cel["lookAtTarget"].asBool();
            celData.mRelativeToTarget = cel["relativeToTarget"].asBool();

            celData.mTweenTypes[CelModel::TweenAttribute_alpha] = (CelModel::TweenType)(cel["alphaTween"].asInt());
            celData.mTweenTypes[CelModel::TweenAttribute_position] = (CelModel::TweenType)(cel["positionTween"].asInt());
            celData.mTweenTypes[CelModel::TweenAttribute_rotation] = (CelModel::TweenType)(cel["rotationTween"].asInt());
            celData.mTweenTypes[CelModel::TweenAttribute_scale] = (CelModel::TweenType)(cel["scaleTween"].asInt());

            celData.mSpriteDescriptor.mBlendType = (GLSprite::BlendType)(cel["blendType"].asInt());

            celData.mSpriteDescriptor.mAlpha = cel["alpha"].asDouble();

            celData.mSpriteDescriptor.mPosition.mX = cel["position"][static_cast<unsigned int>(0)].asDouble();
            celData.mSpriteDescriptor.mPosition.mY = cel["position"][1].asDouble();
            celData.mSpriteDescriptor.mPosition.mZ = cel["position"][2].asDouble();

            celData.mSpriteDescriptor.mRotation.mX = cel["rotation"][static_cast<unsigned int>(0)].asDouble();
            celData.mSpriteDescriptor.mRotation.mY = cel["rotation"][1].asDouble();
            celData.mSpriteDescriptor.mRotation.mZ = cel["rotation"][2].asDouble();

            celData.mSpriteDescriptor.mScale.mX = cel["scale"][static_cast<unsigned int>(0)].asDouble();
            celData.mSpriteDescriptor.mScale.mY = cel["scale"][1].asDouble();

            celData.mSpriteDescriptor.mCenter.mX = cel["center"][static_cast<unsigned int>(0)].asDouble();
            celData.mSpriteDescriptor.mCenter.mY = cel["center"][1].asDouble();

            celData.mSpriteDescriptor.mTextureSrcRect.mX = cel["textureRect"][static_cast<unsigned int>(0)].asInt();
            celData.mSpriteDescriptor.mTextureSrcRect.mY = cel["textureRect"][1].asInt();
            celData.mSpriteDescriptor.mTextureSrcRect.mWidth = cel["textureRect"][2].asInt();
            celData.mSpriteDescriptor.mTextureSrcRect.mHeight = cel["textureRect"][3].asInt();
            celData.mIsTweenCel = false;

            keyframe.mCelHash.insert(celData.mCelNo, celData);
        }

        mKeyFrameList.push_back(keyframe);
        emit keyFrameAdded(mKeyFrameList.count() - 1, keyframe.mDuration, keyframe.mComment);
    }

    resetTweenHash();   
    calculateAnimationDuration();

    // make it emit frame change event
    mCurrentFrameNo = -1;
    setCurrentKeyFrameNo(0);
}
