#ifndef AnimationViewerPanel_H
#define AnimationViewerPanel_H

#include <QWidget>
#include "DataModels/CelModel.h"

class AnimationModel;
class GLSprite;
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
    // cel list control
    void refresh();
    void onCurrentFrameNoChanged(int frameNo);
    void onCelAdded(CelModel::CelData celData);

signals:
    void celSelected(CelModel::CelData* pCelData);
    void celUnselected();

private slots:
    void addNewCel(QPoint& relativePressedPosition);
    void pickCel(QPoint& relativePressedPosition);
    void timerEvent();


private:
    void unselectCels();
    QPoint getCenterPoint() const;

    void clearSprites();
    void addCelSprite(const CelModel::CelData& celData);
    void removeCelSprite(const CelModel::CelData& celData);

    QList<GLSprite*> mGlSpriteList;

    void setFrame(int frameNo);

    AnimationModel* mpAnimationModel;
    int mCurrentFrameNo;
    QColor mClearColor;

    QPoint mSelectedOffset;
    CelModel* const mpSelectedCelModel;
    QTimer* mpTimer;
    bool mIsAnimationPlaying;
};

#endif // AnimationViewerPanel_H
