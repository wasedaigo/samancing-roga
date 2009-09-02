#ifndef ANIMATIONVIEWER_H
#define ANIMATIONVIEWER_H

#include <QtGui/QWidget>
#include "DataModels/KeyFrameData.h"
#include "DataModels/KeyFrame.h"
namespace Ui
{
    class AnimationViewer;
}

class AnimationViewerPanel;
class AnimationModel;
class QStandardItemModel;
class CelModel;
class AnimationViewer : public QWidget
{
Q_OBJECT
public:
    AnimationViewer(QWidget* parent, AnimationModel* animationModel);
    ~AnimationViewer();

private slots:
    void onCelSelected(KeyFrameData* pKeyFrameData);

    void onSaveAnimationButtonClicked();

    void onTweenTypeChanged(int tweenType);
    void onPlayButtonClicked();
    void onTick();

signals:
    void playAnimation(bool stop);
    
protected:
    void changeEvent(QEvent *e);

private:
    void blockSignals(bool block);
    Ui::AnimationViewer *m_ui;
    CelModel* mpSelectedCelModel;
    QStandardItemModel* mpTweenModel;
    AnimationViewerPanel* mpAnimationViewerPanel;
    AnimationModel* mpAnimationModel;
    QTimer* mpAnimationPlayTimer;
    
    KeyFrame::KeyFramePosition mSelectedKeyFramePosition;
};

#endif // ANIMATIONVIEWER_H
