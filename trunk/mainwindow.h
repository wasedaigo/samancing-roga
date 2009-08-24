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
};

#endif // MAINWINDOW_H
