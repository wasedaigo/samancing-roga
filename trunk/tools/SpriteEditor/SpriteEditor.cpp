//
//  This is an editor for exporting sprite definition data
//

#include "SpriteEditor.h"
#include "ui_SpriteEditor.h"

#include "FileLoader.h"

#include "ImageViewer.h"

#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QItemSelection>
#include <QDirIterator>

SpriteEditor::SpriteEditor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::SpriteEditor)
{
    m_ui->setupUi(this);

    m_ui->infoBox->setEnabled(false);

    // Set up tree view model
    mResourceTreeViewModel.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    mResourceTreeViewModel.setReadOnly(true);
    m_ui->resourceTreeView->setAutoScroll(true);
    m_ui->resourceTreeView->setModel(&mResourceTreeViewModel);

    // Set up Image view
    mpImageViewer = new ImageViewer();
    mpImageViewer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ui->scrollArea->setWidget(mpImageViewer);

    // Setup connections
    connect(m_ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewSelected()));
    connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(onSaveSelected()));
    connect(m_ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(onSaveAsSelected()));
    connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenSelected()));
    connect(m_ui->actionSetResourceDir, SIGNAL(triggered()), this, SLOT(onSetResourceDirSelected()));
    connect(m_ui->actionUpdate_Sprites, SIGNAL(triggered()), this, SLOT(onUpdateSpritesSelected()));

    connect(m_ui->resourceTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
    this, SLOT(onSelectionChanged(QItemSelection, QItemSelection)));

    connect(m_ui->gridXSpinBox, SIGNAL(valueChanged(int)), mpImageViewer, SLOT(setGridX(int)));
    connect(m_ui->gridXSpinBox, SIGNAL(valueChanged(int)), &mSpriteData, SLOT(setGridX(int)));

    connect(m_ui->gridYSpinBox, SIGNAL(valueChanged(int)), mpImageViewer, SLOT(setGridY(int)));
    connect(m_ui->gridYSpinBox, SIGNAL(valueChanged(int)), &mSpriteData, SLOT(setGridY(int)));

    connect(m_ui->centerXSpinBox, SIGNAL(valueChanged(int)), mpImageViewer, SLOT(setCenterX(int)));
    connect(m_ui->centerXSpinBox, SIGNAL(valueChanged(int)), &mSpriteData, SLOT(setCenterX(int)));

    connect(m_ui->centerYSpinBox, SIGNAL(valueChanged(int)), mpImageViewer, SLOT(setCenterY(int)));
    connect(m_ui->centerYSpinBox, SIGNAL(valueChanged(int)), &mSpriteData, SLOT(setCenterY(int)));

    connect(mpImageViewer, SIGNAL(gridXChange(int)), m_ui->gridXSpinBox, SLOT(setValue(int)));
    connect(mpImageViewer, SIGNAL(gridYChange(int)), m_ui->gridYSpinBox, SLOT(setValue(int)));
    connect(mpImageViewer, SIGNAL(centerXChange(int)), m_ui->centerXSpinBox, SLOT(setValue(int)));
    connect(mpImageViewer, SIGNAL(centerYChange(int)), m_ui->centerYSpinBox, SLOT(setValue(int)));

    // setup short cuts
    m_ui->actionSave->setShortcut(QKeySequence("Ctrl+s"));

    // First time init
    init();
}

SpriteEditor::~SpriteEditor()
{
    delete mpImageViewer;
    delete m_ui;
}

// First time function
void SpriteEditor::init()
{
    QString initPath = FileLoader::getInitpath();
    load(initPath);
}

// Quit event
void SpriteEditor::closeEvent(QCloseEvent *event)
{
    // Save before quit
    if (true)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

//
// Receiver of Menu Actions
//
void SpriteEditor::onNewSelected()
{
    mSpriteData.clear();
    resetResourceDirectory();
}

void SpriteEditor::onOpenSelected()
{
    QString filePath = QFileDialog::getOpenFileName
                (
                    this,
                    tr("Open"),
                    "",
                    tr("Sprites (*.sp)")
                );

    if (filePath!= "")
    {
        load(filePath);
    }
}

void SpriteEditor::onSetResourceDirSelected()
{
    mSpriteData.mResourceDirPath = QFileDialog::getExistingDirectory
                    (

                        this,
                        tr("Open"),
                        ""
                    );

   resetResourceDirectory();
}

void SpriteEditor::onSaveAsSelected()
{
    QString filePath = QFileDialog::getSaveFileName
                (
                    this,
                    tr("Save"),
                    "",
                    tr("Sprites (*.sp)")
                );

    save(filePath);
}

void SpriteEditor::onSaveSelected()
{
    if (mSpriteData.mFilePath != "")
    {
        save(mSpriteData.mFilePath);
    }
    else
    {
        onSaveAsSelected();
    }
}

// Parse all images in the tree and genarate sprite data
void SpriteEditor::onUpdateSpritesSelected()
{
    if (mSpriteData.mResourceDirPath != "")
    {
        QStringList filters;
        filters<<"*.png";
        QDirIterator dirIterator(mSpriteData.mResourceDirPath,
                                 filters,
                                 QDir::Files|QDir::NoSymLinks,
                                 QDirIterator::Subdirectories);

        mSpriteData.refreshSpriteData();
        while(dirIterator.hasNext())
        {
            // make sure to call next, failing todo so will result in infinite loop
            dirIterator.next();

            QString relativeFilepath = dirIterator.filePath();
            relativeFilepath.replace(mSpriteData.mResourceDirPath, "");

            mSpriteData.loadKey(relativeFilepath);
        }
    }
}

void SpriteEditor::onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2)
{
    QModelIndexList indexes = item1.indexes();
    QModelIndex index = indexes.takeFirst();

    QString path = mResourceTreeViewModel.filePath(index);
    QFileInfo fileInfo = QFileInfo (path);
    if (fileInfo.isFile())
    {
        QString filePath = fileInfo.absoluteFilePath();
        filePath.replace(mSpriteData.mResourceDirPath, "");

        mSpriteData.selectSpriteDef(filePath);

        m_ui->infoBox->setEnabled(true);
        QString imagePath = mSpriteData.mResourceDirPath;
        imagePath.append("/").append(mSpriteData.getSelectedImageID());

        mpImageViewer->setGridX(mSpriteData.getGridX());
        mpImageViewer->setGridY(mSpriteData.getGridY());
        mpImageViewer->setCenterX(mSpriteData.getCenterX());
        mpImageViewer->setCenterY(mSpriteData.getCenterY());
        mpImageViewer->setImage(imagePath);
    }
}

// I/O functions
void SpriteEditor::save(QString filePath)
{
    FileLoader::save(filePath, mSpriteData.exportData());
}

void SpriteEditor::load(QString filePath)
{
    mSpriteData.importData(FileLoader::load(filePath));
    mSpriteData.mFilePath = filePath;
    resetResourceDirectory();
}

void SpriteEditor::resetResourceDirectory()
{
    m_ui->resourceTreeView->setRootIndex(mResourceTreeViewModel.index(mSpriteData.mResourceDirPath));
}
