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
    static GLSprite* getTargetSprite();
    static GLSprite* getCenterPointSprite();

    AnimationModel(QWidget* parent);
    AnimationModel(QWidget* parent, GLSprite* pParentGLSprite);
    ~AnimationModel();
    
    void setup();

    void setParentSprite(GLSprite* pParentGLSprite);
    const GLSprite* getParentSprite() const;

    void setTargetSpritePosition(int x, int y);

    void clearPixmapHash();
    int getMaxFrameCount();

    KeyFrame* getKeyFrame(int lineNo, int frameNo) const;

    int getKeyFrameIndex(int lineNo, int frameNo) const;

    bool isKeyData(KeyFrameData::TweenAttribute tweenAttribute, const KeyFrame* pKeyframe) const;
    int getPreviousKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getNextKeyFrameIndex(int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;
    int getSubanimationStartKeyFrameIndex(int lineNo, int frameNo) const;

    void setKeyFrame(int lineNo, int frameNo, const GLSprite::Point2& position);
    void setKeyFrame(int lineNo, int frameNo, KeyFrameData* pKeyframeData);

    void insertEmptyKeyFrame(int lineNo, int frameNo);
    void addFrameLength(int lineNo, int frameNo, int value);
    void reduceFrameLength(int lineNo, int frameNo);

    void clearFrames(int lineNo, int startFrameNo, int endFrameNo);
    void clearAllKeyFrames();

    const QList<KeyFrame*>& getKeyFrameList(int lineNo) const;

    GLSprite* createGLSpriteAt(int frameNo, int lineNo) const;
    GLSprite* createGLSpriteAt(QList<KeyFrame::KeyFramePosition>nodePath) const;
    const QList<GLSprite*> createGLSpriteListAt(int frameNo) const;

    void setSelectedSourcePath(QString sourcePath);
    QString getSelectedSourcePath() const;

    KeyFrame::KeyFramePosition getCurrentKeyFramePosition();
    void selectCurrentKeyFramePosition(int lineNo, int frameNo);

    void tellTimeLineToRefresh();
    bool copyTweenedAttribute(GLSprite::SpriteDescriptor& spriteDescriptor, int lineNo, int frameNo, KeyFrameData::TweenAttribute tweenAttribute) const;

    bool saveData();
    bool loadData(QString path);
    QString getLoadedAnimationPath() const;
    // public member variables
    GLSprite::Rect mSelectedPaletTextureSrcRect;

    const QWidget* mpParent;

private:
    GLSprite* mpParentGLSprite;
    QString mAnimationName;
    QString mAnimationDirectory;
    QString mAnimationID;
    QString mOriginalAnimationID;

    void setFinalAlpha(GLSprite::SpriteDescriptor& spriteDescriptor) const;
    void setFinalPosition(GLSprite::SpriteDescriptor& spriteDescriptor) const;
    void setFinalRotation(int lineNo, int frameNo, GLSprite::SpriteDescriptor& spriteDescriptor) const;
    void tweenElement(GLSprite::SpriteDescriptor& spriteDescriptor, KeyFrameData::TweenAttribute tweenAttribute, KeyFrameData::TweenType tweenType, GLSprite::SpriteDescriptor& startDescriptor, GLSprite::SpriteDescriptor& endDescriptor, int lineNo, int frameNo, int startFrameNo, int endFrameNo) const;
    GLSprite* tweenFrame(int lineNo, int frameNo) const;

    // Key Frames
    QList<KeyFrame*> mKeyFrames[MaxLineNo];

    // Animation Events
    QList<Event> mAnimationEventList;

    QString mSelectedSourcePath;

    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;
    KeyFrameData* mpTargetCel;


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
