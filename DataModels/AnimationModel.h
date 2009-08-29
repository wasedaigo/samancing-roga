#ifndef ANIMATIONMODEL_H
#define ANIMATIONMODEL_H

#include <QHash>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QModelIndex>
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
        MaxLineNo = 8
    };

    struct Event
    {
        int mFrameNo;
        QString mType;
        QString mValue;
    };

    static QPixmap* getPixmap(QString path);

    AnimationModel(QWidget* parent);
    ~AnimationModel();

    void clearPixmapHash();
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
    const QList<GLSprite*> createGLSpriteListAt(int frameNo) const;

    void setSelectedSourcePath(QString sourcePath);
    QString getSelectedSourcePath() const;

    void setTargetSpritePosition(int x, int y);
    GLSprite* getTargetSprite() const;

    GLSprite* getCenterPointSprite() const;

    KeyFrame::KeyFramePosition getCurrentKeyFramePosition();
    void selectCurrentKeyFramePosition(int lineNo, int frameNo);

    void tellTimeLineToRefresh();

    void saveData();
    void loadData(QString path);

    // public member variables
    QString mAnimationName;
    QString mAnimationDirectory;
    QString mAnimationID;
    QString mOriginalAnimationID;

    GLSprite::Rect mSelectedPaletTextureSrcRect;

private:
    QWidget* mpParent;

    void tweenElement(KeyFrameData* keyframeData, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData* startKeyFrameData, KeyFrameData* endKeyFrameData, int frameNo, int startFrameNo, int endFrameNo) const;
    GLSprite* tweenFrame(int lineNo, int frameNo) const;

    // Key Frames
    QList<KeyFrame*> mKeyFrames[MaxLineNo];

    // Animation Events
    QList<Event> mAnimationEventList;

    QString mSelectedSourcePath;

    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;
    KeyFrameData* mpTargetCel;

    QPixmap* mpTargetPixmap;
    GLSprite* mpTargetSprite;
    QPixmap* mpCenterPointPixmap;
    GLSprite* mpCenterPointSprite;

public slots:
    void setAnimationName(QString name);
    void setAnimationID(QString id);

signals:
    void animationNameChanged(QString animationName);
    void animationIDChanged(QString animationName);

    void selectedPaletChanged(QString path);
    void animationDurationChanged(int length);
    void targetPositionMoved(int x, int y);

    void refreshTimeLine();
    void fileSaved(QModelIndex);
    void selectedKeyFramePositionChanged(int lineNo, int frameNo);
};

#endif // ANIMATIONMODEL_H
