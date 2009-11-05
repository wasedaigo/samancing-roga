#include "RogaMonsterEditor.h"
#include "ui_RogaMonsterEditor.h"

#include <QMessageBox>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>
#include <QPainter>
#include <QPoint>
#include <QStandardItemModel>

#include "ImageViewer.h"

#include "FileLoader.h"

// Size of one face icon
#define FACE_ICON_WIDTH 32
#define FACE_ICON_HEIGHT 48
#define FACE_ICON_PATH "Images/UI/UI_Faces.png"



// Immunity setting
enum ImmunityRate
{
    ImmunityRate_SuperWeak,
    ImmunityRate_Weak,
    ImmunityRate_None,
    ImmunityRate_Resist,
    ImmunityRate_Invulnerable,
    ImmunityRate_Absorb,

    ImmunityRate_COUNT
};

static int immunityRate[ImmunityRate_COUNT] = {-250, -100, 0, 50, 100, 200};
static QString immunityRateType[ImmunityRate_COUNT] = {"Super weak", "Weak", "None", "Resist", "Invulnerable", "Absorb"};
static int getIndexOfImmunityRateType(QString key)
{
    int num = 0;
    for (int i = 0; i < ImmunityRate_COUNT; i++)
    {
        if (key == immunityRateType[i])
        {
            num = i;
            break;
        }
    }

    return num;
}

// Equip Skill setting
#define EQUIP_SKILL_FIELD_COUNT 2
static QString equipSkillFields[EQUIP_SKILL_FIELD_COUNT] = {"SkillID", "count"};
static int getIndexOfEquipSkillsField(QString key)
{
    int num = 0;
    for (int i = 0; i < EQUIP_SKILL_FIELD_COUNT; i++)
    {
        if (key == equipSkillFields[i])
        {
            num = i;
            break;
        }
    }

    return num;
}

RogaMonsterEditor::RogaMonsterEditor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::RogaMonsterEditor),
    mpFaceIconViewer(NULL),
    mSelectedID(QString("")),
    mResourceDirPath(""),
    mChanged(false)
{
    m_ui->setupUi(this);

    connect(m_ui->addMonsterButton, SIGNAL(clicked()), this, SLOT(onAddMonsterButtonClicked()));
    connect(m_ui->removeMonsterButton, SIGNAL(clicked()), this, SLOT(onRemoveMonsterButtonClicked()));
    connect(m_ui->monsterGraphicSelectButton, SIGNAL(clicked()), this, SLOT(onMonsterGraphicSelected()));
    connect(m_ui->addMonsterDataButton, SIGNAL(clicked()), this, SLOT(onAddMonsterDataButtonClicked()));
    connect(m_ui->removeMonsterDataButton, SIGNAL(clicked()), this, SLOT(onRemoveMonsterDataButtonClicked()));
    connect(m_ui->monsterListWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(onMonsterSelected(QModelIndex)));
    connect(m_ui->applyButton, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
    connect(m_ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewSelected()));
    connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenSelected()));
    connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(onSaveSelected()));
    connect(m_ui->actionSave_As, SIGNAL(triggered()), this, SLOT(onSaveAsSelected()));
    connect(m_ui->actionSet_Resource_Dir, SIGNAL(triggered()), this, SLOT(onSetResourceDirSelected()));

    mpFaceIconViewer = new ImageViewer();
    m_ui->faceIconBox->layout()->addWidget(mpFaceIconViewer);

    mpMonsterGraphicViewer = new ImageViewer();
    m_ui->monsterGraphicBox->layout()->addWidget(mpMonsterGraphicViewer);

    // setup short cuts
    m_ui->actionSave->setShortcut(QKeySequence("Ctrl+s"));

    init();
}

RogaMonsterEditor::~RogaMonsterEditor()
{
    delete mpImmunityRateTypeModel;
    delete mpFaceIconViewer;
    delete mpMonsterGraphicViewer;
    delete m_ui;
}

// First time function
void RogaMonsterEditor::init()
{

    QString initPath = FileLoader::getInitpath();

    Json::Value initData = FileLoader::getInitOptionData();
    mResourceDirPath = QString::fromStdString(initData["resourceDirPath"].asString());

    load(initPath);

    loadUIFaceImage();
    m_ui->monsterListWidget->setCurrentRow(0);
    MonsterLoad(0);

    // Setup Immunity Comboboxes
    mpImmunityRateTypeModel = new QStandardItemModel();
    for (int i = 0; i < ImmunityRate_COUNT; i++)
    {
        mpImmunityRateTypeModel->appendRow(new QStandardItem(immunityRateType[i]));
    }
    m_ui->undeadCombobox->setModel(mpImmunityRateTypeModel);
    m_ui->undeadCombobox->setCurrentIndex(ImmunityRate_None);
}

// Helper function
QString RogaMonsterEditor::getRelativeMonsterGraphicPath(QString monsterFilePath) const
{
    QString replacePath = mResourceDirPath;
    replacePath.append("/");
    monsterFilePath.replace(replacePath, "");
    return monsterFilePath;
}

//
// Image viewer related
//
void RogaMonsterEditor::loadUIFaceImage()
{
    if(mResourceDirPath != "")
    {
        QString filename = mResourceDirPath;
        filename.append("/").append(FACE_ICON_PATH);

        mpFaceIconViewer->setImage(filename);
        mpFaceIconViewer->setGridX(FACE_ICON_WIDTH);
        mpFaceIconViewer->setGridY(FACE_ICON_HEIGHT);
    }
}

void RogaMonsterEditor::loadMonsterImage(QString monsterFilePath)
{
        QString filename = mResourceDirPath;
        filename.append("/").append(monsterFilePath);
        mpMonsterGraphicViewer->setImage(filename);
}

// Delegate method
void RogaMonsterEditor::closeEvent(QCloseEvent *event)
{
    // Save before quit
    if (mChanged)
    {
        int r = QMessageBox::warning(this,
                    tr("Monster Editor"),
                    tr("Do you want to save the draft "
                       "before closing?"),
                     QMessageBox::Save
                     | QMessageBox::No
                     | QMessageBox::Cancel);
        if (r == QMessageBox::Save) {
            save(mMonsterDataPath);
            event->accept();
        } else if (r == QMessageBox::No) {
            event->accept();
        } else {
            event->ignore();
        }
    }
    else
    {
        event->accept();
    }
}

// Reconstruct the monster list(on the left side)
void RogaMonsterEditor::refreshMonsterList()
{
    m_ui->monsterListWidget->clear();

    int index = 0;
    int i = 0;

    Json::Value& data = mMonsterDataRoot["data"];
    for(Json::Value::iterator iter = data.begin() ; data.end() != iter ; iter++)
    {
        QString keyName = QString::fromStdString(iter.key().asString());
        m_ui->monsterListWidget->addItem(keyName);
        if (keyName == mSelectedID)
        {
            index = i;
        }
        i++;
    }
    m_ui->monsterListWidget->setCurrentRow(index);
    MonsterLoad(0);
}

void RogaMonsterEditor::MonsterLoad(int row)
{
    if(m_ui->monsterListWidget->item(row))
    {
        QVariant data = m_ui->monsterListWidget->item(row)->data(Qt::DisplayRole);
        mSelectedID = data.toString();

        loadMonsterData();
    }
}

void RogaMonsterEditor::clearUI()
{
     for(int i = m_ui->tableWidget->rowCount() - 1; i >=0; i--)
    {
         m_ui->tableWidget->removeRow(i);
    }
    m_ui->nameEdit->setText("");
    m_ui->IDEdit->setText("");
    m_ui->attackSpinBox->setValue(0);
    m_ui->defenseSpinBox->setValue(0);
    m_ui->lifeSpinBox->setValue(1);
    m_ui->waitSpinBox->setValue(0);
    mpMonsterGraphicViewer->clear();
}

//
// Events
//
void RogaMonsterEditor::onAddMonsterButtonClicked()
{
    QString tempID = "new_Monster%0";
    QString newID = "";
    int index = 0;
    while (true)
    {
        newID = tempID.arg(index);
        if (!mMonsterDataRoot["data"].isMember(newID.toStdString()))
        {
            break;
        }
        index++;
    }
    mMonsterDataRoot["data"][newID.toStdString()] = "null";
    refreshMonsterList();
}

void RogaMonsterEditor::onRemoveMonsterButtonClicked()
{
    QModelIndexList indexes = m_ui->monsterListWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QVariant data = m_ui->monsterListWidget->selectedItems().first()->data(Qt::DisplayRole);
        QString key = data.toString();
        mMonsterDataRoot["data"].removeMember(key.toStdString());
        refreshMonsterList();
    }
}

void RogaMonsterEditor::onMonsterGraphicSelected()
{
    QString filePath = QFileDialog::getOpenFileName
                (
                    this,
                    tr("Open"),
                    mResourceDirPath,
                    tr("PNG files (*.png)")
                );

    loadMonsterImage(getRelativeMonsterGraphicPath(filePath));
}

void RogaMonsterEditor::onAddMonsterDataButtonClicked()
{
    m_ui->tableWidget->model()->insertRow(m_ui->tableWidget->rowCount());
}

void RogaMonsterEditor::onRemoveMonsterDataButtonClicked()
{
    QModelIndexList indexes = m_ui->tableWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QModelIndex index = indexes.first();
        m_ui->tableWidget->model()->removeRow(index.row(), QModelIndex());
    }
}

void RogaMonsterEditor::onMonsterSelected(QModelIndex index)
{
    QModelIndexList indexes = m_ui->monsterListWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QVariant data = m_ui->monsterListWidget->selectedItems().first()->data(Qt::DisplayRole);
        mSelectedID = data.toString();

        loadMonsterData();
    }
}

void RogaMonsterEditor::onApplyButtonClicked()
{
    if (saveMonsterData())
    {
        mChanged = true;
        m_ui->monsterListWidget->selectedItems().first()->setData(Qt::DisplayRole, QString(m_ui->IDEdit->text()));
    }
}

void RogaMonsterEditor::onNewSelected()
{
    mMonsterDataRoot.clear();
    mMonsterDataPath = "";
    m_ui->monsterListWidget->clear();
    clearUI();
}

void RogaMonsterEditor::onSaveAsSelected()
{
    QString filePath = QFileDialog::getSaveFileName
                (
                    this,
                    tr("Save"),
                    mMonsterDataPath,
                    tr("Sprites (*.mon)")
                );

    save(filePath);
}

void RogaMonsterEditor::onSaveSelected()
{
    if (mMonsterDataPath != "")
    {
        save(mMonsterDataPath);
    }
    else
    {
        onSaveAsSelected();
    }
}

void RogaMonsterEditor::onOpenSelected()
{
    QString filePath = QFileDialog::getOpenFileName
                (
                    this,
                    tr("Open"),
                    mMonsterDataPath,
                    tr("Sprites (*.mon)")
                );

    if (filePath!= "")
    {
        load(filePath);
    }
}

void RogaMonsterEditor::onSetResourceDirSelected()
{
    mResourceDirPath = QFileDialog::getExistingDirectory
                    (
                        this,
                        tr("Open"),
                        ""
                    );

    loadUIFaceImage();
}

//
// Save and Load
//
bool RogaMonsterEditor::saveMonsterData()
{
    // Save Data
    Json::Value& data = mMonsterDataRoot["data"];
    if (data.isMember(mSelectedID.toStdString()))
    {
        QString newID =  m_ui->IDEdit->text();
        if (newID != "")
        {
            if (newID != mSelectedID && !data[newID.toStdString()].isNull())
            {
                QMessageBox::information(window(), tr("File already exists"),
                                     tr("The ID already exists, please use different ID."));
                return false;
            }

            // Remove old member
            data.removeMember(mSelectedID.toStdString());

            // Set new ID
            mSelectedID = newID;

            // Save new data
            std::string MonsterID = mSelectedID.toStdString();

            // Get Monster Data
            Json::Value& monsterData = data[MonsterID];

            monsterData.removeMember(MonsterID);

            monsterData["name"] = m_ui->nameEdit->text().toStdString();
            monsterData["attack"] = m_ui->attackSpinBox->value();
            monsterData["defense"] = m_ui->defenseSpinBox->value();
            monsterData["life"] = m_ui->lifeSpinBox->value();
            monsterData["wait"] = m_ui->waitSpinBox->value();

            // Face icon pos
            QPoint point = mpFaceIconViewer->getSelectedGrid();
            Json::Value faceIconPos;
            faceIconPos[static_cast<unsigned int>(0)] = point.x();
            faceIconPos[1] = point.y();
            monsterData["faceIconPos"] = faceIconPos;

            // Monster graphic name
            QString monsterFilePath = getRelativeMonsterGraphicPath(mpMonsterGraphicViewer->getLoadedFilePath());
            monsterData["monsterGraphic"] = monsterFilePath.toStdString();

            // List of equip skills
            Json::Value& equipSkills = monsterData["equipSkills"];
            for (int i = 0; i < m_ui->tableWidget->rowCount(); i++)
            {
                for (int j = 0; j < m_ui->tableWidget->columnCount(); j++)
                {
                    QTableWidgetItem* pWidgetItem = m_ui->tableWidget->item(i, j);
                    if (pWidgetItem)
                    {
                        QVariant data = pWidgetItem->data(Qt::EditRole);
                        equipSkills[static_cast<int>(i)][equipSkillFields[j].toStdString()] = data.toString().toStdString();
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void RogaMonsterEditor::loadMonsterData()
{
    clearUI();

    m_ui->IDEdit->setText(mSelectedID);
    std::string MonsterID = mSelectedID.toStdString();
    
    Json::Value& data = mMonsterDataRoot["data"];
    if (data.isMember(MonsterID))
    {
        if (!data[MonsterID].isNull())
        {
            Json::Value& MonsterData = data[MonsterID];
            if(MonsterData.isObject())
            {
                if(!MonsterData["name"].isNull())
                {
                     m_ui->nameEdit->setText(QString::fromStdString(MonsterData["name"].asString()));
                }
                if(MonsterData["attack"].isInt())
                {
                      m_ui->attackSpinBox->setValue(MonsterData["attack"].asInt());
                }
                if(MonsterData["defense"].isInt())
                {
                     m_ui->defenseSpinBox->setValue(MonsterData["defense"].asInt());
                }
                if(MonsterData["life"].isInt())
                {
                     m_ui->lifeSpinBox->setValue(MonsterData["life"].asInt());
                }
                if(MonsterData["wait"].isInt())
                {
                     m_ui->waitSpinBox->setValue(MonsterData["wait"].asInt());
                }

                // Face Icon
                if(MonsterData["faceIconPos"].isArray())
                {
                    QPoint point = mpFaceIconViewer->getSelectedGrid();
                    Json::Value faceIconPos = MonsterData["faceIconPos"];
                    int x = faceIconPos[static_cast<unsigned int>(0)].asInt();
                    int y = faceIconPos[1].asInt();
                    mpFaceIconViewer->setSelectedGrid(x, y);
                }
                else
                {
                    mpFaceIconViewer->setSelectedGrid(0, 0);
                }

                // Monster Image
                if(!MonsterData["monsterGraphic"].isNull())
                {
                    QString monsterFilePath = QString::fromStdString(MonsterData["monsterGraphic"].asString());
                    loadMonsterImage(monsterFilePath);
                }

                Json::Value& equipSkills = MonsterData["equipSkills"];
                for (unsigned int i = 0; i < equipSkills.size(); i++)
                {
                    Json::Value& MonsterData = equipSkills[static_cast<int>(i)];
                    Json::ValueIterator iter;

                    // add a row to the bottom
                    m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());

                    // set data to the bottom row
                    for (iter = MonsterData.begin(); iter != MonsterData.end(); iter++)
                    {
                        QString key = QString::fromStdString(iter.key().asString());
                        int index = getIndexOfEquipSkillsField(key);
                        std::string str = ((Json::Value)(*iter)).asString();
                        QString data = QString::fromStdString(str);
                        m_ui->tableWidget->setItem(m_ui->tableWidget->rowCount() - 1, index, new QTableWidgetItem(data));
                    }
                }
            }
        }
    }
}


void RogaMonsterEditor::save(QString filePath)
{
    if (filePath != "")
    {
        saveMonsterData();

        mMonsterDataRoot["faceIconWidth"] = FACE_ICON_WIDTH;
        mMonsterDataRoot["faceIconHeight"] = FACE_ICON_HEIGHT;
        mMonsterDataRoot["faceIconPath"] = FACE_ICON_PATH;

        Json::Value optionalData;
        optionalData["resourceDirPath"] = mResourceDirPath.toStdString();
        FileLoader::saveInitData(filePath, optionalData);
        FileLoader::save(filePath, mMonsterDataRoot);
        mChanged = false;
    }
}

void RogaMonsterEditor::load(QString filePath)
{
    mMonsterDataRoot = FileLoader::load(filePath);
    mMonsterDataPath = filePath;
    loadMonsterData();

    refreshMonsterList();
}
