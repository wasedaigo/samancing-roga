#ifndef ANIMATIONMODEL_H
#define ANIMATIONMODEL_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QDir>

#include "KeyFrameData.h"
#include "GLSprite.h"
#include "DataModels/KeyFrame.h"

class AnimationModel : public QObject
{
Q_OBJECT

public:
    enum
    {
        ImagePaletCount = 4,
        MaxLineNo = 8
    };

    struct Event
    {
        int mFrameNo;
        QString mType;
        QString mValue;
    };

    AnimationModel();
    ~AnimationModel();

    int getMaxFrameCount();

    KeyFrame* getKeyFrame(int lineNo, int frameNo) const;

    int getKeyFrameIndex(int lineNo, int frameNo) const;

    bool isKeyData(KeyFrameData::TweenAttribute tweenAttribute, const KeyFrame* pKeyframe) const;
    int getPreviousKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getNextKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;

    void setKeyFrame(int lineNo, int frameNo, const GLSprite::Point2& position);
    void setKeyFrame(int lineNo, int frameNo, KeyFrameData* pKeyframeData);

    void insertEmptyKeyFrame(int lineNo, int frameNo);
    void addFrameLength(int lineNo, int frameNo, int value);
    void reduceFrameLength(int lineNo, int frameNo);

    void clearFrames(int lineNo, int startFrameNo, int endFrameNo);
    void clearAllKeyFrames();

    const QList<KeyFrame*>& getKeyFrameList(int lineNo) const;
    const QList<KeyFrame*> createKeyFrameListAt(int frameNo) const;

    // Getter / Setter
    QPixmap* getPixmap(int paletNo) const;
    QString getAnimationPaletID(int paletNo) const;
    void setAnimationImagePalet(int paletNo, QString id);

    void setSelectedPaletNo(int paletNo);
    int getSelectedPaletNo() const;

    void setTargetSpritePosition(int x, int y);
    GLSprite* getTargetSprite() const;

    KeyFrame::KeyFramePosition getCurrentKeyFramePosition();
    void selectCurrentKeyFramePosition(int lineNo, int frameNo);

    void tellTimeLineToRefresh();

    void saveData();
    void loadData();

    // public member variables
    QString mAnimationName;
    GLSprite::Rect mSelectedPaletTextureSrcRect;

private:
    void tweenElement(KeyFrameData* keyframeData, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData* startKeyFrameData, KeyFrameData* endKeyFrameData, int frameNo, int startFrameNo, int endFrameNo) const;
    KeyFrame* tweenFrame(int lineNo, int frameNo) const;

    // Key Frames
    QList<KeyFrame*> mKeyFrames[MaxLineNo];

    // Animation Events
    QList<Event> mAnimationEventList;

    QString mImagePalets[ImagePaletCount];
    QPixmap* mpPixmaps[ImagePaletCount];

    int mSelectedPaletNo;
    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;

    KeyFrameData* mpTargetCel;

    QPixmap* mpTargetPixmap;
    GLSprite* mpTargetSprite;

signals:
    void animationImagePaletChanged(int paletNo, QString id);
    void animationDurationChanged(int length);
    void targetPositionMoved(int x, int y);

    void refreshTimeLine();
    void refreshTimeLine(int lineNo);
    void selectedKeyFramePositionChanged(int lineNo, int frameNo);
};

#endif // ANIMATIONMODEL_H
