#ifndef ANIMATIONVIEWER_H
#define ANIMATIONVIEWER_H

#include <QtGui/QWidget>
#include "DataModels/Celmodel.h"

namespace Ui
{
    class AnimationViewer;
}

class AnimationViewerPanel;
class AnimationModel;
class QStandardItemModel;
class AnimationViewer : public QWidget
{
Q_OBJECT
public:
    AnimationViewer(QWidget* parent, AnimationModel* animationModel);
    ~AnimationViewer();

private slots:
    void onCelAdded(CelModel::CelData celData);
    void onCelRemoved(CelModel::CelData celData);
    void onCelSelected(CelModel::CelData* pCelData);
    void onCelUnselected();
    void onCelHashCelClicked(int row, int col);
    void onCurrentFrameNoChanged(int keyFrameNo);

    void onSaveAnimationButtonClicked();
    void onSwitchTextureButtonClicked();

    void onTweenTypeChanged(int tweenType);
    void onSliderValueChanged(int value);
    void onAnimationDurationChanged(int duration);
    void onPlayButtonClicked();
    void onTick();

signals:
    void playAnimation(bool stop);

protected:
    void changeEvent(QEvent *e);

private:
    void clearCels();
    int selectCelIndexOfCelHash(int celNo) const;
    void addCel(int celNo, QString comment);
    void removeCel(int celNo);
    
    Ui::AnimationViewer *m_ui;
    CelModel* mpSelectedCelModel;
    QStandardItemModel* mpTweenModel;
    AnimationViewerPanel* mpAnimationViewerPanel;
    AnimationModel* mpAnimationModel;
    QTimer* mpAnimationPlayTimer;
    int mAnimationStartFrameNo;
};

#endif // ANIMATIONVIEWER_H
