#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "ui_AnimationViewer.h"
#include "ImagePaletDialog/ImagePaletDialog.h"
#include "ResourceManager.h"
#include "AnimationViewer/AnimationViewer.h"
#include "QStandardItemModel.h"
#include "QTimelineWidget/QTimelinePanel.h"
#include <QDirModel>
#include "ResourceTree/ResourceTree.h"

void MainWindow::setupConnections()
{
    connect(ui->showPaletButton_1, SIGNAL(clicked()), this, SLOT(onPaletButtonClicked()));

    // connect animation list controls
    connect(ui->addAnimationButton, SIGNAL(clicked()), this, SLOT(onAddAnimationButtonClicked()));
    connect(ui->removeAnimationButton, SIGNAL(clicked()), this, SLOT(onRemoveAnimationButtonClicked()));

    // connec Animation Viewer
    connect(mpAnimationViewer, SIGNAL(playAnimation(bool)), this, SLOT(setEnabled(bool)));
    connect(ui->lineEditAnimationName, SIGNAL(textChanged(QString)), mpAnimationModel, SLOT(setAnimationName(QString)));
    connect(ui->lineEditAnimationID, SIGNAL(textChanged(QString)), mpAnimationModel, SLOT(setAnimationID(QString)));

    connect(mpAnimationModel, SIGNAL(animationNameChanged(QString)), ui->lineEditAnimationName, SLOT(setText(QString)));
    connect(mpAnimationModel, SIGNAL(animationIDChanged(QString)), ui->lineEditAnimationID, SLOT(setText(QString)));

    connect(ui->animationTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
    connect(mpAnimationModel, SIGNAL(fileSaved(QModelIndex)), &mAnimationTreeViewModel, SLOT(refresh(QModelIndex)));
}

void MainWindow::setupModels()
{
    mpAnimationModel = new AnimationModel(this);
    mpAnimationListModel = new QStandardItemModel(0, 1, this);
}

void MainWindow::setupUIModels()
{
    mAnimationTreeViewModel.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    mAnimationTreeViewModel.setReadOnly(true);

    QString rootPath = QDir::currentPath();
    rootPath.append("/");
    rootPath.append(ANIMATION_DIR.path());

    ui->animationTreeView->setAutoScroll(true);
    ui->animationTreeView->setModel(&mAnimationTreeViewModel);
    ui->animationTreeView->setRootIndex(mAnimationTreeViewModel.index(rootPath) );
}
void MainWindow::setupUI()
{
    ui->setupUi(this);

    mpDialog = new ImagePaletDialog(this, mpAnimationModel);

    mpAnimationViewer = new AnimationViewer(this, mpAnimationModel);
    mpAnimationViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->animationViewer->addWidget(mpAnimationViewer);

    mpQTimelinePanel = new QTimelinePanel(mpAnimationModel, this);
    mpQTimelinePanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->timelineContainer->addWidget(mpQTimelinePanel);

    QString rootPath = QDir::currentPath();
    rootPath.append("/");
    rootPath.append(ANIMATION_DIR.path());
    rootPath.append("/test.ani");

    //mpResourceTree->read(rootPath);
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

    setEditEnabled(false);
}

MainWindow::~MainWindow()
{
    delete mpDialog;
    delete mpAnimationModel;
    delete mpAnimationListModel;
    delete ui;
}

void MainWindow::setEditEnabled(bool enabled)
{
    ui->gridInfoBox->setEnabled(enabled);
    mpQTimelinePanel->setEnabled(enabled);
    mpAnimationViewer->setEnabled(enabled);
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

 Animation treeview control

---------------------------------------------------------------------*/
void MainWindow::onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2)
{
    QModelIndexList indexes = item1.indexes();
    QModelIndex index = indexes.takeFirst();

    QString path = mAnimationTreeViewModel.filePath(index);
    QFileInfo fileInfo = QFileInfo (path);
    if (fileInfo.isFile())
    {
        if (mpAnimationModel->loadData(path))
        {
            setEditEnabled(true);
        }
        else
        {
            setEditEnabled(false);
        }
    }

//    QString rootPath = QDir::currentPath();
//    rootPath.append("/");
//    rootPath.append(ANIMATION_DIR.path());
//
//    mpAnimationModel->loadData();
//    // we don't want to store absolute path
//    path.replace(rootPath, "", Qt::CaseInsensitive);
//
//    mpAnimationModel->setSelectedSourcePath(path);
}


void MainWindow::onAddAnimationButtonClicked()
{
    QStandardItem* item = new QStandardItem(QString("%0").arg("NO NAME"));
    item->setEditable(false);
    mpAnimationListModel->insertRow(0, item);
    mpAnimationListModel->sort(0, Qt::AscendingOrder);
}

void MainWindow::onRemoveAnimationButtonClicked()
{
//    QModelIndexList list = ui->animationListView->selectionModel()->selectedRows();
//
//    QList<QModelIndex>::Iterator iter;
//    for (iter = list.begin(); iter != list.end(); iter++)
//    {
//        QModelIndex modelIndex = (QModelIndex)*iter;
//        mpAnimationListModel->removeRow(modelIndex.row(), QModelIndex());
//    }
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
