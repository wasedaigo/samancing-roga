#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "ui_AnimationViewer.h"
#include "ImagePaletDialog/ImagePaletDialog.h"
#include "Common.h"
#include "AnimationViewer/AnimationViewer.h"
#include "QSignalMapper.h"
#include "QStandardItemModel.h"
#include "QTimelineWidget/QTimelinePanel.h"

void MainWindow::setupConnections()
{
    // connect palet display controls
    mpPaletButtonSignalMapper->setMapping(ui->showPaletButton_1, 0);
    mpPaletButtonSignalMapper->setMapping(ui->showPaletButton_2, 1);
    mpPaletButtonSignalMapper->setMapping(ui->showPaletButton_3, 2);
    mpPaletButtonSignalMapper->setMapping(ui->showPaletButton_4, 3);

    connect(ui->showPaletButton_1, SIGNAL(clicked()), mpPaletButtonSignalMapper, SLOT(map()));
    connect(ui->showPaletButton_2, SIGNAL(clicked()), mpPaletButtonSignalMapper, SLOT(map()));
    connect(ui->showPaletButton_3, SIGNAL(clicked()), mpPaletButtonSignalMapper, SLOT(map()));
    connect(ui->showPaletButton_4, SIGNAL(clicked()), mpPaletButtonSignalMapper, SLOT(map()));
    connect(mpPaletButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(onPaletButtonClicked(int)));

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

    for (int i = 0; i < AnimationModel::ImagePaletCount; i++)
    {
        mpDialogs[i] = new ImagePaletDialog(i, this, mpAnimationModel, mpAnimationImageNameListModel);
    }

    mpAnimationViewer = new AnimationViewer(this, mpAnimationModel);
    mpAnimationViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->animationViewer->addWidget(mpAnimationViewer);

    QTimelinePanel* pQTimelinePanel = new QTimelinePanel(mpAnimationModel, this);
    ui->timelineContainer->addWidget(pQTimelinePanel);
}

void MainWindow::loadConfigFile()
{
  // todo: load data from config file

  QStringList list = ANIMATION_IMAGE_DIR.entryList(QDir::Files, QDir::Name);

  QList<QString>::Iterator iter;

  for (iter = list.begin(); iter != list.end(); iter++)
  {
    QFileInfo fileInfo = QFileInfo((QString)*iter);

    mpAnimationImageNameListModel->appendRow(new QStandardItem(fileInfo.baseName()));
  }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mpPaletButtonSignalMapper(new QSignalMapper(this)),
      mpAnimationImageNameListModel(new QStandardItemModel())
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
    for (int i = 0; i < AnimationModel::ImagePaletCount; i++)
    {
        delete mpDialogs[i];
    }
    delete mpPaletButtonSignalMapper;
    delete mpAnimationModel;
    delete mpAnimationListModel;
    delete mpAnimationImageNameListModel;
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

void MainWindow::onPaletButtonClicked(int no)
{
   if(mpAnimationModel->getSelectedPaletNo() != -1)
   {
    mpDialogs[mpAnimationModel->getSelectedPaletNo()]->close();
   }

    mpDialogs[no]->move(this->x(), this->y());
   mpDialogs[no]->show();

   mpAnimationModel->setSelectedPaletNo(no);
}

/* -------------------------------------------------------------------

 Animation Panel Control

---------------------------------------------------------------------*/
void MainWindow::onAnimationDurationChanged(int duration)
{

}
