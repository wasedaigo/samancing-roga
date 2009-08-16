#include "AnimationModel.h"
#include <QPixmap>
#include "Macros.h"

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

inline void AnimationModel::addTweenData(int keyFrameNo, int celNo, CelModel::TweenAttribute tweenAttribute)
{
    TweenData tweenData;
    tweenData.mTweenAttribute = tweenAttribute;
    tweenData.mCelNo = celNo;
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
    for (int i = keyFrameNo + 1; i < mKeyFrameList.count(); i++)
    {
        CelModel::CelData* endCelData = getCelDataRef(i, celNo);
        if (endCelData && endCelData->mTweenTypes[tweenAttribute] != CelModel::eTT_None)
        {
            tweenData.mEndKeyFrameNo = i;
            break;
        }
        else
        {
            // Add reference to previous tween data
            mKeyFrameList[i].mTweenIDList.push_front(tweenCelID);
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
                addTweenData(i, celData.mCelNo, (CelModel::TweenAttribute)j);
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

// Set new celData
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

void AnimationModel::setPixmap(int paletNo, QPixmap* pixmap)
{
    delete mpPixmaps[paletNo];
    mpPixmaps[paletNo] = pixmap;
    emit paletImageChanged(paletNo);
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
        CelModel::CelData celData;
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

        return returnCelHash.unite(keyFrameCelHash);
    }
    else
    {
        return returnCelHash;
    }
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
