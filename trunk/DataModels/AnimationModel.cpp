#include "AnimationModel.h"
#include <QPixmap>
#include "Macros.h"

#define TweenValue(startCelData, endCelData, field, frameNo, startFrameNo, endFrameNo)\
celData.m##field = LERP(startCelData.m##field, endCelData.m##field, frameNo, startFrameNo, endFrameNo)


#define Tween(tweenType, startCelData, endCelData, field, frameNo, startFrameNo, endFrameNo)\
{\
 if (startCelData.m##tweenType == CelModel::eTT_Linear)\
    {\
        TweenValue(startCelData, endCelData, field, frameNo, startFrameNo, endFrameNo);\
    }\
    else\
    {\
        celData.m##field = startCelData.m##field;\
    }\
}


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

//Celdata control

// Add passed celdata to the list, it will set unique celNo.
void AnimationModel::addCelData(int keyFrameNo, const GLSprite::Point2& position)
{
    CelModel::CelData celData = makeDefaultCelData();
    celData.mSpriteDescriptor.mPosition = position;
    celData.mSpriteDescriptor.mTextureSrcRect = mSelectedPaletTextureSrcRect;
    celData.mTextureID = mSelectedPaletNo;

    QList<CelModel::CelData> celList = getSelectedSortedCelList(keyFrameNo);
    QList<CelModel::CelData>::Iterator iter = celList.begin();

    // Get new celNo
    int celNo = 0;
    while (iter != celList.end())
    {
        CelModel::CelData celData = (CelModel::CelData)*iter;
        if (celData.mCelNo != celNo) { break; }
        celNo++;
        iter++;
    }

    // Add new cel data to the list
    celData.mCelNo = celNo;
    getCurrentKeyFrame().mCelList.insert(0, celData);

    emit celAdded(celData);
}


// Set new celData
void AnimationModel::setCelDataInCurrentKeyFrame(int celNo, const CelModel::CelData& celData)
{
    QList<CelModel::CelData>& celList = mKeyFrameList[getCurrentKeyFrameNo()].mCelList;

    int celIndex = getCelDataIndexInCurrentKeyFrame(celNo);

    if (celIndex >= 0)
    {
        celList[celIndex] = celData;
    }
}

// Remove cel data
void AnimationModel::removeCelData(int keyFrameNo, int celNo)
{

    // Get CelList inside keyframe...
    QList<CelModel::CelData>& celList = getCurrentKeyFrame().mCelList;

    // Remove a Celdata of passed celNo
    QList<CelModel::CelData>::Iterator iter = celList.begin();
    while (iter != celList.end())
    {
        const CelModel::CelData& celData = (CelModel::CelData)*iter;
        if (celData.mCelNo == celNo)
        {
            celList.erase(iter);
            emit celRemoved(celData);
            break;
        }
        iter++;
    }
}

//Keyframe control
// Add new keyframe
void AnimationModel::addEmptyKeyFrame()
{
    const KeyFrame keyframe = AnimationModel::makeEmptyKeyFrame();
    mKeyFrameList.push_back(keyframe);
    emit keyFrameAdded(mKeyFrameList.count() - 1, keyframe.mDuration, keyframe.mComment);

    calculateAnimationDuration();
}

void AnimationModel::insertEmptyKeyFrame(int index)
{
    const KeyFrame keyframe = AnimationModel::makeEmptyKeyFrame();
    mKeyFrameList.insert(index, keyframe);
    emit keyFrameAdded(index, keyframe.mDuration, keyframe.mComment);

    calculateAnimationDuration();
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

void AnimationModel::setKeyFrameComment(int index, QString comment)
{
    KeyFrame& keyFrame = mKeyFrameList[index];
    keyFrame.mComment = comment;
}

AnimationModel::KeyFrame& AnimationModel::getCurrentKeyFrame()
{
    return mKeyFrameList[getCurrentKeyFrameNo()];
}

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

void AnimationModel::setSelectedPaletNo(int paletNo)
{
    mSelectedPaletNo = paletNo;
}

int AnimationModel::getSelectedPaletNo() const
{
    return mSelectedPaletNo;
}

QList<CelModel::CelData> AnimationModel::getSelectedSortedCelList(int keyFrameNo) const
{
    QList<CelModel::CelData> celList;

    if (keyFrameNo >= 0)
    {
        celList = mKeyFrameList[keyFrameNo].mCelList;
        qSort(celList.begin(), celList.end(), CelModel::celNoLessThan);
    }

    return celList;
}

// Get list of cels from current keyframe
QList<CelModel::CelData> AnimationModel::getCurrentSortedCelList() const
{
    return getSelectedSortedCelList(mCurrentKeyFrameNo);
}

// Return true if it find a cel to tween, if not return false;
bool AnimationModel::tweenFrame(QList<CelModel::CelData>& returnCelList, CelModel::CelData& startCelData, int startFrameNo, int frameNo, int endKeyFrameNo)
{
    int endFrameNo = getFrameNoByKeyFrameNo(endKeyFrameNo);
    bool match = false;
    QList<CelModel::CelData>& endCelList = mKeyFrameList[endKeyFrameNo].mCelList;
    for (int j = 0; j < endCelList.count(); j++)
    {
        CelModel::CelData& endCelData = endCelList[j];
        if (startCelData.mCelNo == endCelData.mCelNo)
        {
            CelModel::CelData celData;
            celData.mSpriteDescriptor.mCenter = startCelData.mSpriteDescriptor.mCenter;
            celData.mSpriteDescriptor.mTextureSrcRect = startCelData.mSpriteDescriptor.mTextureSrcRect;
            celData.mCelNo = startCelData.mCelNo;
            celData.mTextureID = startCelData.mTextureID;
            celData.mLookAtTarget = startCelData.mLookAtTarget;
            celData.mRelativeToTarget = startCelData.mRelativeToTarget;
            celData.mBlur = startCelData.mBlur;

            Tween(AlphaTweenType, startCelData, endCelData, SpriteDescriptor.mAlpha, frameNo, startFrameNo, endFrameNo);
            Tween(PositionTweenType, startCelData, endCelData, SpriteDescriptor.mPosition.mX, frameNo, startFrameNo, endFrameNo);
            Tween(PositionTweenType, startCelData, endCelData, SpriteDescriptor.mPosition.mY, frameNo, startFrameNo, endFrameNo);
            Tween(RotationTweenType, startCelData, endCelData, SpriteDescriptor.mRotation.mX, frameNo, startFrameNo, endFrameNo);
            Tween(ScaleTweenType, startCelData, endCelData, SpriteDescriptor.mScale.mX, frameNo, startFrameNo, endFrameNo);
            Tween(ScaleTweenType, startCelData, endCelData, SpriteDescriptor.mScale.mY, frameNo, startFrameNo, endFrameNo);

            celData.mSpriteDescriptor.mBlendType = startCelData.mSpriteDescriptor.mBlendType;
            returnCelList.push_front(celData);
            match = true;
        }
    }
    
    return match;
}

// Get list of cels from current keyframe
QList<CelModel::CelData> AnimationModel::getCelListAt(int frameNo)
{
    int currentKeyFrameNo = getKeyFrameNoByFrameNo(frameNo);
    int nextKeyFrameNo = currentKeyFrameNo + 1;
    if (nextKeyFrameNo < mKeyFrameList.count())
    {
        int startFrameNo = getFrameNoByKeyFrameNo(currentKeyFrameNo);

        QList<CelModel::CelData> &startCelList = mKeyFrameList[currentKeyFrameNo].mCelList;
        QList<CelModel::CelData> returnCelList;
        for (int i = 0; i < startCelList.count(); i++)
        {
            bool match = false;
            for (int j = nextKeyFrameNo; j < mKeyFrameList.count(); j++)
            {
                if (tweenFrame(returnCelList, startCelList[i], startFrameNo, frameNo, j))
                {
                    match = true;
                    break;
                }
            }
            if (!match)
            {
                CelModel::CelData celData = startCelList[i];
                returnCelList.push_front(celData);
            }
        }

        return returnCelList;
    }

    // Just return current cellist
    return getCurrentSortedCelList();
}

int AnimationModel::getCelDataIndexInCurrentKeyFrame(int celNo)
{
    const QList<CelModel::CelData>& celList = getCurrentKeyFrame().mCelList;

    for (int i = 0; i < celList.count(); i++)
    {
        if(celList[i].mCelNo== celNo)
        {
            return i;
        }
    }

    return -1;
}

CelModel::CelData* AnimationModel::getCelDataRefInCurrentKeyFrame(int celNo)
{
    int index = getCelDataIndexInCurrentKeyFrame(celNo);
    if (index < 0 || index >= getCurrentKeyFrame().mCelList.count())
    {
       return NULL;
    }
    else
    {
      return &getCurrentKeyFrame().mCelList[index];
    }
}
