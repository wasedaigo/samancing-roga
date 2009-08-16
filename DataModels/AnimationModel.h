#ifndef ANIMATIONMODEL_H
#define ANIMATIONMODEL_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPixmap>

#include "DataModels/CelModel.h"

class AnimationModel : public QObject
{
Q_OBJECT

public:
    enum
    {
        ImagePaletCount = 4
    };

    struct Event
    {
        int mFrameNo;
        QString mType;
        QString mValue;
    };

    struct TweenCelData
    {
        int mStartFrameNo;
        int mEndFrameNo;
        int mCelNo;

        CelModel::TweenType mPositionTweenType;
        CelModel::TweenType mRotationTweenType;
        CelModel::TweenType mScaleTweenType;
        CelModel::TweenType mAlphaTweenType;

        void operator=(const CelModel::CelData& item)
        {
            mCelNo = item.mCelNo;
            mPositionTweenType = item.mPositionTweenType;
            mRotationTweenType = item.mRotationTweenType;
            mScaleTweenType = item.mScaleTweenType;
            mAlphaTweenType = item.mAlphaTweenType;
        }
    };

    struct KeyFrame
    {
        int mDuration;
        QString mComment;
        QHash<int, CelModel::CelData> mCelHash;
        QList<int> mTweenCelIndexList;
    };

    static inline KeyFrame makeEmptyKeyFrame();

    AnimationModel();
    ~AnimationModel();

    int getAnimationDuration();
    void calculateAnimationDuration();
    void resetTweenCelHash();

    // Cel data control
    void addCelData(int keyFrameNo, const GLSprite::Point2& position);
    void setCelData(int keyFrameNo, int celNo, const CelModel::CelData& celData);
    void removeCelData(int keyFrameNo, int celDataID);
    // Key frame control
    void addEmptyKeyFrame();
    void insertEmptyKeyFrame(int index);
    void removeKeyFrame(int index);
    void clearAllKeyFrames();

    // Getter / Setter
    QPixmap* getPixmap(int paletNo) const;
    void setPixmap(int paletNo, QPixmap* pixmap);

    // Frame
    int getFrameNoByKeyFrameNo(int keyFrameNo);
    int getKeyFrameNoByFrameNo(int frameNo);
    bool isKeyFrame(int frameNo);
    int getCurrentFrameNo() const;
    void setCurrentFrameNo(int frameNo);

    // Keyframe
    bool isKeyFrameSelected();
    void setKeyFrameDuration(int index, int duration);
    void setKeyFrameComment(int index, QString comment);

    void setCurrentKeyFrameNo(int keyFrameNo);    
    int getCurrentKeyFrameNo() const;

    void setSelectedPaletNo(int paletNo);
    int getSelectedPaletNo() const;

    QHash<int, CelModel::CelData> getCelHash(int keyFrameNo) const;

    bool tweenFrame(QHash<int, CelModel::CelData>& returnCelHash, CelModel::CelData& startCelData, int startFrameNo, int frameNo, int endKeyFrameNo);
    QHash<int, CelModel::CelData> getCelHashAt(int frameNo);

    CelModel::CelData* getCelDataRef(int keyFrameNo, int celNo);

    // public member variables
    QString mAnimationName;
    GLSprite::Rect mSelectedPaletTextureSrcRect;


private:
    // Key Frames
    QList<KeyFrame> mKeyFrameList;

    // Animation Events
    QList<Event> mAnimationEventList;
    QList<TweenCelData> mTweenCelList;

    QString mImagePalets[ImagePaletCount];
    QPixmap* mpPixmaps[ImagePaletCount];

    int mCurrentKeyFrameNo;
    int mCurrentFrameNo;
    int mSelectedPaletNo;
    int mAnimationDuration;

signals:
    void celAdded(CelModel::CelData celData);
    void celRemoved(CelModel::CelData celData);
    void keyFrameAdded(int index, int duration, QString comment);
    void keyFrameRemoved(int index);
    void keyFrameDurationChanged(int index, int duration);
    void currentFrameNoChanged(int);

    void animationDurationChanged(int duration);
    void paletImageChanged(int paletNo);
};

inline AnimationModel::KeyFrame AnimationModel::makeEmptyKeyFrame()
{
    KeyFrame keyFrame;
    keyFrame.mDuration = 1;
    keyFrame.mComment = "";

    return keyFrame;
}

#endif // ANIMATIONMODEL_H
