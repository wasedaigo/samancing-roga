#ifndef AnimationViewerPanel_H
#define AnimationViewerPanel_H

#include <QWidget>

class AnimationModel;
class CelModel;
class GLSprite;
class KeyFrame;
class KeyFrameData;
class AnimationViewerPanel : public QWidget
{
Q_OBJECT
public:
    AnimationViewerPanel(QWidget* parent, AnimationModel* pAnimationModel, CelModel* const pSelectedCelModel);
    ~AnimationViewerPanel();

    void selectCel(int celNo);
    void playAnimation();
    void stopAnimation();
    void gotoNextFrame();

protected:
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void refresh();

signals:
    void celSelected(KeyFrameData* pKeyFrameData);

private slots:
    void addNewCel(QPoint& relativePressedPosition);
    void pickCel(QPoint& relativePressedPosition);
    void timerEvent();


private:
    QPoint getCenterPoint() const;

    void renderCross(QPainter& painter);
    void renderCelSprites(const QPoint& centerPoint, QPainter& painter);
    void renderTargetSprite(const QPoint& centerPoint, QPainter& painter);
    void clearSprites();
    void addCelSprite(const KeyFrame* pKeyFrame);

    QList<GLSprite*> mGlSpriteList;

    void setFrame(int frameNo);

    AnimationModel* mpAnimationModel;
    int mCurrentFrameNo;
    QColor mClearColor;

    QPoint mSelectedOffset;
    CelModel* const mpSelectedCelModel;
    QTimer* mpTimer;
    bool mIsAnimationPlaying;
    bool mCelGrabbed;
    bool mTargetGrabbed;
};

#endif // AnimationViewerPanel_H
