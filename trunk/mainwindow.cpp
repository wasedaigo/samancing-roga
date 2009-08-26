#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "ui_AnimationViewer.h"
#include "ImagePaletDialog/ImagePaletDialog.h"
#include "Common.h"
#include "AnimationViewer/AnimationViewer.h"
#include "QStandardItemModel.h"
#include "QTimelineWidget/QTimelinePanel.h"
#include <QDirModel>

void MainWindow::setupConnections()
{
    connect(ui->showPaletButton_1, SIGNAL(clicked()), this, SLOT(onPaletButtonClicked()));

    // connect animation list controls
    connect(ui->addAnimationButton, SIGNAL(clicked()), this, SLOT(onAddAnimationButtonClicked()));
    connect(ui->removeAnimationButton, SIGNAL(clicked()), this, SLOT(onRemoveAnimationButtonClicked()));

    // connec Animation Viewer
    connect(mpAnimationViewer, SIGNAL(playAnimation(bool)), this, SLOT(setEnabled(bool)));
}

void MainWindow::setupModels()
{
    mpAnimationModel = new AnimationModel();
    mpAnimationListModel = new QStandardItemModel(0, 1, this);
}

void MainWindow::setupUIModels()
{
    ui->animationListView->setModel(mpAnimationListModel);
    ui->animationComboBox->setModel(mpAnimationListModel);
}
void MainWindow::setupUI()
{
    ui->setupUi(this);

    mpDialog = new ImagePaletDialog(this, mpAnimationModel);

    mpAnimationViewer = new AnimationViewer(this, mpAnimationModel);
    mpAnimationViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->animationViewer->addWidget(mpAnimationViewer);

    QTimelinePanel* pQTimelinePanel = new QTimelinePanel(mpAnimationModel, this);
    pQTimelinePanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->timelineContainer->addWidget(pQTimelinePanel);
}

void MainWindow::loadConfigFile()
{

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    loadConfigFile();
    setupModels();
    setupUI();
    setupUIModels();

    setupConnections();

    mpAnimationModel->selectCurrentKeyFramePosition(0, 0);
    mpAnimationModel->loadData();
}

MainWindow::~MainWindow()
{
    delete mpDialog;
    delete mpAnimationModel;
    delete mpAnimationListModel;
    delete ui;
}

/* -------------------------------------------------------------------

 Animation data control

---------------------------------------------------------------------*/
void MainWindow::loadAnimationData(int index)
{

}

void MainWindow::onAnimationSelected(int index)
{
    loadAnimationData(index);
}

/* -------------------------------------------------------------------

 Animation list box control

---------------------------------------------------------------------*/
void MainWindow::onAddAnimationButtonClicked()
{
    QStandardItem* item = new QStandardItem(QString("%0").arg("NO NAME"));
    item->setEditable(false);
    mpAnimationListModel->insertRow(0, item);
    mpAnimationListModel->sort(0, Qt::AscendingOrder);
}

void MainWindow::onRemoveAnimationButtonClicked()
{
    QModelIndexList list = ui->animationListView->selectionModel()->selectedRows();

    QList<QModelIndex>::Iterator iter;
    for (iter = list.begin(); iter != list.end(); iter++)
    {
        QModelIndex modelIndex = (QModelIndex)*iter;
        mpAnimationListModel->removeRow(modelIndex.row(), QModelIndex());
    }
}


/* -------------------------------------------------------------------

 Image palet choice button control

---------------------------------------------------------------------*/

void MainWindow::onPaletButtonClicked()
{
   if (mpDialog->isHidden())
   {
       mpDialog->move(this->x(), this->y());
       mpDialog->show();
    }
}

/* -------------------------------------------------------------------

 Animation Panel Control

---------------------------------------------------------------------*/
void MainWindow::onAnimationDurationChanged(int duration)
{

}
