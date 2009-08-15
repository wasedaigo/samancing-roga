#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "DataModels/AnimationModel.h"
#include "QDir.h"

class AnimationViewer;
class ImagePaletDialog;
class QSignalMapper;
class QStandardItemModel;
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    // key frame list control
    void onAddKeyFrameButtonClicked();
    void onInsertKeyFrameButtonClicked();
    void onKeyFrameAdded(int index, int duration, QString comment);
    void onRemoveKeyFrameButtonClicked();
    void onKeyFrameRemoved(int index);
    void onKeyFrameCellChanged(int row, int col);
    void onKeyFrameBoxCellClicked(int row, int col);
    void onKeyFrameBoxVHeaderSelected(int row);
    void onKeyFrameDurationChanged(int index, int duration);
    void onCurrentFrameNoChanged(int frameNo);

    // event list control
    void insertEventRow(int row);
    void onAddEventButtonClicked();
    void onInsertEventButtonClicked();
    void onRemoveEventButtonClicked();

    // animation list control
    void onAddAnimationButtonClicked();
    void onRemoveAnimationButtonClicked();

    void onPaletButtonClicked(int no);
    
    void onAnimationSelected(int index);

    // Animation Panel Control
    void onAnimationDurationChanged(int duration);

private:
    void setupConnections();
    void setupModels();
    void setupUIModels();
    void setupUI();
    void loadAnimationData(int index);
    void loadConfigFile();

    Ui::MainWindow *ui;
    QSignalMapper *mpPaletButtonSignalMapper;
    QStandardItemModel *mpAnimationListModel;

    AnimationModel* mpAnimationModel;
    AnimationViewer* mpAnimationViewer;
    QStandardItemModel *mpAnimationImageNameListModel;

    ImagePaletDialog *mpDialogs[AnimationModel::ImagePaletCount];

    QDir mAnimationDirectory;
    QDir mAnimationImageDirectory;
    QDir mSoundDirectory;
};

#endif // MAINWINDOW_H
