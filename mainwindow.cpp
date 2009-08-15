#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "ui_AnimationViewer.h"
#include "ImagePaletDialog/ImagePaletDialog.h"
#include "Common.h"
#include "AnimationViewer/AnimationViewer.h"
#include "QSignalMapper.h"
#include "QStandardItemModel.h"

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

    // connect keyframe list controls
    connect(ui->addKeyFrameButton, SIGNAL(clicked()), this, SLOT(onAddKeyFrameButtonClicked()));
    connect(ui->insertKeyFrameButton, SIGNAL(clicked()), this, SLOT(onInsertKeyFrameButtonClicked()));
    connect(ui->removeKeyFrameButton, SIGNAL(clicked()), this, SLOT(onRemoveKeyFrameButtonClicked()));
    connect(mpAnimationModel, SIGNAL(keyFrameAdded(int, int, QString)), this, SLOT(onKeyFrameAdded(int, int, QString)));
    connect(mpAnimationModel, SIGNAL(keyFrameRemoved(int)), this, SLOT(onKeyFrameRemoved(int)));
    connect(ui->keyFramesTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(onKeyFrameCellChanged(int, int)));
    connect(ui->keyFramesTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(onKeyFrameBoxCellClicked(int, int)));
    connect(ui->keyFramesTableWidget->verticalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(onKeyFrameBoxVHeaderSelected(int)) );
    
    connect(mpAnimationModel, SIGNAL(keyFrameDurationChanged(int, int)), this, SLOT(onKeyFrameDurationChanged(int, int)));
    connect(mpAnimationModel, SIGNAL(currentFrameNoChanged(int)), this, SLOT(onCurrentFrameNoChanged(int)));

    // connect event list controls
    connect(ui->addEventButton, SIGNAL(clicked()), this, SLOT(onAddEventButtonClicked()));
    connect(ui->insertEventButton, SIGNAL(clicked()), this, SLOT(onInsertEventButtonClicked()));
    connect(ui->removeEventButton, SIGNAL(clicked()), this, SLOT(onRemoveEventButtonClicked()));

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

    ui->keyFramesTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    ui->keyFramesTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);

    ui->eventTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    ui->eventTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
    ui->eventTableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);

    for (int i = 0; i < AnimationModel::ImagePaletCount; i++)
    {
        mpDialogs[i] = new ImagePaletDialog(i, this, mpAnimationModel, mAnimationImageDirectory, mpAnimationImageNameListModel);
    }

    mpAnimationViewer = new AnimationViewer(this, mpAnimationModel);
    mpAnimationViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->animationViewer->addWidget(mpAnimationViewer);

}

void MainWindow::loadConfigFile()
{
  // todo: load data from config file
  mAnimationDirectory = QDir(QString("GameResource/Animations"));
  mAnimationImageDirectory = QDir(QString("GameResource/Images"));
  mSoundDirectory = QDir(QString("GameResource/Sounds"));

  QStringList list = mAnimationImageDirectory.entryList(QDir::Files, QDir::Name);

  QList<QString>::Iterator iter;

  mpAnimationImageNameListModel->appendRow(new QStandardItem(QString(NONE)));
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

    mpAnimationModel->insertEmptyKeyFrame(0);
    mpAnimationModel->setCurrentKeyFrameNo(0);
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

 Key frame list box control

---------------------------------------------------------------------*/

// Add button click
void MainWindow::onAddKeyFrameButtonClicked()
{
    QList<QTableWidgetSelectionRange> ranges = ui->keyFramesTableWidget->selectedRanges();

    int bottomRowNumber = ui->keyFramesTableWidget->rowCount();
    for (int i = ranges.count() - 1; i >=0; i--)
    {
       QTableWidgetSelectionRange range = ranges.at(i);
       bottomRowNumber = range.bottomRow();
    }

    mpAnimationModel->addEmptyKeyFrame();
}

// Insert button click
void MainWindow::onInsertKeyFrameButtonClicked()
{
    QList<QTableWidgetSelectionRange> ranges = ui->keyFramesTableWidget->selectedRanges();

    int bottomRowNumber = ui->keyFramesTableWidget->rowCount();
    for (int i = ranges.count() - 1; i >=0; i--)
    {
       QTableWidgetSelectionRange range = ranges.at(i);
       bottomRowNumber = range.bottomRow();
    }

    mpAnimationModel->insertEmptyKeyFrame(bottomRowNumber);
}


// Key frame added to the model
void MainWindow::onKeyFrameAdded(int index, int duration, QString comment)
{
    // Insert blank row
    ui->keyFramesTableWidget->insertRow(index);

    // Frame length
    QTableWidgetItem* item = new QTableWidgetItem(QString("%0").arg(duration));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->keyFramesTableWidget->setItem(index, 0, item);

    // Comment
    item = new QTableWidgetItem(comment);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->keyFramesTableWidget->setItem(index, 1, item);
    ui->keyFramesTableWidget->verticalHeader()->setResizeMode(index, QHeaderView::Fixed);
}

// Remove button click
void MainWindow::onRemoveKeyFrameButtonClicked()
{
    QList<QTableWidgetSelectionRange> ranges = ui->keyFramesTableWidget->selectedRanges();

    for (int i = ranges.count() - 1; i >=0; i--)
    {
       QTableWidgetSelectionRange range = ranges.at(i);

       for (int j = range.bottomRow(); j >= range.topRow(); j--)
       {
           mpAnimationModel->removeKeyFrame(j);
       }
    }
}
// Key frame removed from the model
void MainWindow::onKeyFrameRemoved(int index)
{
    ui->keyFramesTableWidget->removeRow(index);
}

void MainWindow::onKeyFrameDurationChanged(int index, int duration)
{
    QTableWidgetItem* pItem = new QTableWidgetItem(QString("%0").arg(duration));
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->keyFramesTableWidget->setItem(index, 0, pItem);
}

void MainWindow::onCurrentFrameNoChanged(int frameNo)
{
    if (this->isEnabled())
    {
        int keyFrameNo = mpAnimationModel->getKeyFrameNoByFrameNo(frameNo);
        if (keyFrameNo >= 0)
        {
            ui->keyFramesTableWidget->selectRow(keyFrameNo);
        }
    }
}

void MainWindow::onKeyFrameBoxCellClicked(int row, int col)
{
    mpAnimationModel->setCurrentKeyFrameNo(row);
}

void MainWindow::onKeyFrameBoxVHeaderSelected(int row)
{
    mpAnimationModel->setCurrentKeyFrameNo(row);
}

void MainWindow::onKeyFrameCellChanged(int row, int col)
{
    QTableWidgetItem* pItem = NULL;
    switch (col)
    {
        case 0:
            pItem = ui->keyFramesTableWidget->item(row, 0);
            int duration = pItem->data(Qt::EditRole ).toInt();
            mpAnimationModel->setKeyFrameDuration(row, duration);
        break;

        case 1:
            pItem = ui->keyFramesTableWidget->item(row, 1);
            if (pItem)
            {
                QString comment = pItem->data(Qt::EditRole ).toString();
                mpAnimationModel->setKeyFrameComment(row, comment);
            }
        break;
    }
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

 Event list box control

---------------------------------------------------------------------*/

void MainWindow::insertEventRow(int row)
{
    // Insert blank row
    ui->eventTableWidget->insertRow(row);

    // Frame No
    QTableWidgetItem* item = new QTableWidgetItem(QString("%0").arg(row));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->eventTableWidget->setItem(row, 0, item);

    // Frame duration
    item = new QTableWidgetItem(QString("1"));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
    ui->eventTableWidget->setItem(row, 1, item);
}

void MainWindow::onAddEventButtonClicked()
{
    insertEventRow(ui->eventTableWidget->rowCount());
}

void MainWindow::onInsertEventButtonClicked()
{
    QList<QStandardItem*> list;

    QList<QTableWidgetSelectionRange> ranges = ui->eventTableWidget->selectedRanges();

    int bottomRowNumber = ui->eventTableWidget->rowCount();
    for (int i = ranges.count() - 1; i >=0; i--)
    {
       QTableWidgetSelectionRange range = ranges.at(i);
       bottomRowNumber = range.bottomRow();
    }

    insertEventRow(bottomRowNumber);
}

void MainWindow::onRemoveEventButtonClicked()
{
    QList<QTableWidgetSelectionRange> ranges = ui->eventTableWidget->selectedRanges();

    for (int i = ranges.count() - 1; i >=0; i--)
    {
       QTableWidgetSelectionRange range = ranges.at(i);

       for (int j = range.bottomRow(); j >= range.topRow(); j--)
       {
        ui->eventTableWidget->removeRow(j);
       }
    }
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
