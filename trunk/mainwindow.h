#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "DataModels/AnimationModel.h"
#include "QDir.h"
#include "ImagePaletDialog/SourceFileDirModel.h"

class AnimationViewer;
class ImagePaletDialog;
class ResourceTree;
class QSignalMapper;
class QStandardItemModel;
class QItemSelection;
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

private slots:
    // animation list control
    void onAddAnimationButtonClicked();
    void onRemoveAnimationButtonClicked();

    void onPaletButtonClicked();
    
    void onAnimationSelected(int index);

    // Animation Panel Control
    void onAnimationDurationChanged(int duration);
    void onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2);

private:
    void setupConnections();
    void setupModels();
    void setupUIModels();
    void setupUI();
    void loadAnimationData(int index);
    void loadConfigFile();

    Ui::MainWindow *ui;

    QStandardItemModel *mpAnimationListModel;

    AnimationModel* mpAnimationModel;
    AnimationViewer* mpAnimationViewer;
    SourceFileDirModel mAnimationTreeViewModel;

    ImagePaletDialog *mpDialog;

    ResourceTree *mpResourceTree;
};

#endif // MAINWINDOW_H
