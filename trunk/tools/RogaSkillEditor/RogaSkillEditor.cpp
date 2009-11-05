#include "RogaSkillEditor.h"
#include "ui_RogaSkillEditor.h"

#include <QMessageBox>
#include <QTextStream>
#include <QCloseEvent>
#include <QFileDialog>

#include "FileLoader.h"

#define SKILL_DATA_DIRECTORY QString("GameData/Animations/Battle/Skills/")

//Pose Type setting
enum PoseType
{
    PoseType_Attack,
    PoseType_Magic,
    PoseType_COUNT
};

// Pose type
static QString poseTypeString[PoseType_COUNT] = {
    "Attack", "Magic"
    };

static PoseType getPoseTypeFromString(QString key)
{
    for (int i = 0; i < PoseType_COUNT; i++)
    {
        if (key == poseTypeString[i])
        {
            return (PoseType)i;
        }
    }

    return PoseType_Attack;
}

//Skill Type setting
enum SkillType
{
    SkillType_Knife,
    SkillType_Sword,
    SkillType_Rapier,
    SkillType_Spear,
    SkillType_Mace,
    SkillType_Axe,
    SkillType_Bow,
    SkillType_Shield,
    SkillType_Wind,
    SkillType_Fire,
    SkillType_Thunder,
    SkillType_Earth,
    SkillType_Ice,
    SkillType_Water,
    SkilLType_Monster,

    SkillType_COUNT
};

// Skill type setting
static QString skillTypeString[SkillType_COUNT] = {
    "Knife", "Sword", "Rapier", "Spear", "Mace", "Axe", "Bow", "Shield", "Wind", "Fire", "Thunder", "Earth", "Ice", "Water", "Monster"
    };

static SkillType getSkillTypeFromString(QString key)
{
    for (int i = 0; i < SkillType_COUNT; i++)
    {
        if (key == skillTypeString[i])
        {
            return (SkillType)i;
        }
    }

    return SkillType_Knife;
}

#define DAMAGE_FIELD_COUNT 6
static QString damageFields[DAMAGE_FIELD_COUNT] = {"min", "max", "hit", "critical", "attr", "damagedef"};
static int getIndexOfDamageField(QString key)
{
    int num = 0;
    for (int i = 0; i < DAMAGE_FIELD_COUNT; i++)
    {
        if (key == damageFields[i])
        {
            num = i;
            break;
        }
    }

    return num;
}

#define TARGET_SELECTION_TYPE_COUNT 4
static QString targetSelectionType[TARGET_SELECTION_TYPE_COUNT] = {"single", "side", "all", "field"};
static int getIndexOfTargetSelectionType(QString key)
{
    int num = 0;
    for (int i = 0; i < TARGET_SELECTION_TYPE_COUNT; i++)
    {
        if (key == targetSelectionType[i])
        {
            num = i;
            break;
        }
    }

    return num;
}

#define MULTI_TARGET_TYPE_COUNT 4
static QString multiTargetType[MULTI_TARGET_TYPE_COUNT] = {"aimTarget", "aimGrid", "chain", "drain"};
static int getIndexOfMultiTargetType(QString key)
{
    int num = 0;
    for (int i = 0; i < MULTI_TARGET_TYPE_COUNT; i++)
    {
        if (key == multiTargetType[i])
        {
            num = i;
            break;
        }
    }

    return num;
}

#define AOE_TYPE_COUNT 4
static QString AOETypes[AOE_TYPE_COUNT] = {"OneTile", "OneRow", "OneColumn", "Cross"};
static int getIndexOfAOEType(QString key)
{
    int num = 0;
    for (int i = 0; i < AOE_TYPE_COUNT; i++)
    {
        if (key == AOETypes[i])
        {
            num = i;
            break;
        }
    }

    return num;
}

RogaSkillEditor::RogaSkillEditor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::RogaSkillEditor),
    mSelectedID(QString("")),
    mChanged(false)
{
    m_ui->setupUi(this);

    init();
    refreshSkillList();

    for (int i = 0; i < AOE_TYPE_COUNT; i++)
    {
        m_ui->AOEComboBox->addItem(AOETypes[i]);
    }


    connect(m_ui->radioButtonOne, SIGNAL(clicked(bool)), this, SLOT(refreshTabControl()));
    connect(m_ui->radioButtonTwo, SIGNAL(clicked(bool)), this, SLOT(refreshTabControl()));

    connect(m_ui->addSkillButton, SIGNAL(clicked()), this, SLOT(onAddSkillButtonClicked()));
    connect(m_ui->removeSkillButton, SIGNAL(clicked()), this, SLOT(onRemoveSkillButtonClicked()));

    connect(m_ui->addSkillDataButton, SIGNAL(clicked()), this, SLOT(onAddSkillDataButtonClicked()));
    connect(m_ui->removeSkillDataButton, SIGNAL(clicked()), this, SLOT(onRemoveSkillDataButtonClicked()));
    connect(m_ui->skillListWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(onSkillSelected(QModelIndex)));
    connect(m_ui->applyButton, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
    connect(m_ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewSelected()));
    connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenSelected()));
    connect(m_ui->actionSave, SIGNAL(triggered()), this, SLOT(onSaveSelected()));
    connect(m_ui->actionSave_As, SIGNAL(triggered()), this, SLOT(onSaveAsSelected()));

    // Setup Skill Type combobox

    mpSkillTypeModel = new QStandardItemModel();
    for (int i = 0; i < SkillType_COUNT; i++)
    {
        mpSkillTypeModel->appendRow(new QStandardItem(skillTypeString[i]));
    }
    m_ui->skillTypeCombobox->setModel(mpSkillTypeModel);

    // setup short cuts
    m_ui->actionSave->setShortcut(QKeySequence("Ctrl+s"));

    refreshTabControl();
    m_ui->skillListWidget->setCurrentRow(0);
    skillLoad(0);
}

RogaSkillEditor::~RogaSkillEditor()
{
    delete mpSkillTypeModel;
    delete m_ui;
}

// First time function
void RogaSkillEditor::init()
{
    QString initPath = FileLoader::getInitpath();
    load(initPath);
}

void RogaSkillEditor::refreshSkillList()
{
    int index = 0;
    m_ui->skillListWidget->clear();
    int i = 0;
    for(Json::Value::iterator iter = mSkillDataRoot.begin() ; mSkillDataRoot.end() != iter ; iter++)
    {
        QString keyName = QString::fromStdString(iter.key().asString());
        m_ui->skillListWidget->addItem(keyName);
        if (keyName == mSelectedID)
        {
            index = i;
        }
        i++;
    }
    m_ui->skillListWidget->setCurrentRow(index);
    skillLoad(0);
}

void RogaSkillEditor::closeEvent(QCloseEvent *event)
{
    // Save before quit
    if (mChanged)
    {
        int r = QMessageBox::warning(this,
                    tr("Mail Editor"),
                    tr("Do you want to save the draft "
                       "before closing?"),
                     QMessageBox::Save
                     | QMessageBox::No
                     | QMessageBox::Cancel);
        if (r == QMessageBox::Save) {
            save(mSkillDataPath);
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

void RogaSkillEditor::refreshTabControl()
{
    m_ui->tabWidgetTwo->setEnabled(m_ui->radioButtonTwo->isChecked());
}

void RogaSkillEditor::onAddSkillButtonClicked()
{
    QString tempID = "new_skill%0";
    QString newID = "";
    int index = 0;
    while (true)
    {
        newID = tempID.arg(index);
        if (!mSkillDataRoot.isMember(newID.toStdString()))
        {
            break;
        }
        index++;
    }
    mSkillDataRoot[newID.toStdString()] = "null";
    refreshSkillList();
}

void RogaSkillEditor::onRemoveSkillButtonClicked()
{
    QModelIndexList indexes = m_ui->skillListWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QVariant data = m_ui->skillListWidget->selectedItems().first()->data(Qt::DisplayRole);
        QString key = data.toString();
        mSkillDataRoot.removeMember(key.toStdString());
        refreshSkillList();
    }
}

void RogaSkillEditor::onAddSkillDataButtonClicked()
{
    m_ui->tableWidget->model()->insertRow(m_ui->tableWidget->rowCount());
}

void RogaSkillEditor::onRemoveSkillDataButtonClicked()
{
    QModelIndexList indexes = m_ui->tableWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QModelIndex index = indexes.first();
        m_ui->tableWidget->model()->removeRow(index.row(), QModelIndex());
    }
}

void RogaSkillEditor::skillLoad(int row)
{
    if(m_ui->skillListWidget->item(row))
    {
        QVariant data = m_ui->skillListWidget->item(row)->data(Qt::DisplayRole);
        mSelectedID = data.toString();

        loadSkillData();
    }
}

void RogaSkillEditor::onSkillSelected(QModelIndex index)
{
    QModelIndexList indexes = m_ui->skillListWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QVariant data = m_ui->skillListWidget->selectedItems().first()->data(Qt::DisplayRole);
        mSelectedID = data.toString();

        loadSkillData();
    }
}

void RogaSkillEditor::clearUI()
{
    m_ui->nameEdit->setText("");
    m_ui->IDEdit->setText("");
    m_ui->descEdit->setText("");
    m_ui->castingAnimationEdit->setText("");
    m_ui->skillAnimationEdit->setText("");
    m_ui->multiTargetCombox->setCurrentIndex(0);
    m_ui->AOECheckBox->setChecked(false);
    m_ui->AOEComboBox->setCurrentIndex(0);

    m_ui->radioButtonOne->setChecked(true);
    for(int i = m_ui->tableWidget->rowCount() - 1; i >=0; i--)
    {
         m_ui->tableWidget->removeRow(i);
    }

    m_ui->OneSelectionTypeComboBox->setCurrentIndex(0);
    m_ui->OneAliveCheckBox->setChecked(false);
    m_ui->OneDeadCheckBox->setChecked(false);
    m_ui->OneSelfCheckBox->setChecked(false);
    m_ui->OneOpponentCheckBox->setChecked(false);
    m_ui->OneAllyCheckBox->setChecked(false);

    m_ui->TwoSelectionTypeComboBox->setCurrentIndex(0);
    m_ui->TwoAliveCheckBox->setChecked(false);
    m_ui->TwoDeadCheckBox->setChecked(false);
    m_ui->TwoSelfCheckBox->setChecked(false);
    m_ui->TwoOpponentCheckBox->setChecked(false);
    m_ui->TwoAllyCheckBox->setChecked(false);

    m_ui->chainableCheckbox->setChecked(false);
    m_ui->chainStartFrameCombobox->setValue(0);

    m_ui->skillTypeCombobox->setCurrentIndex(0);
    m_ui->poseTypeCombobox->setCurrentIndex(0);
}

bool RogaSkillEditor::saveSkillData()
{
    // Save Data
    if (mSkillDataRoot.isMember(mSelectedID.toStdString()))
    {
        QString newID =  m_ui->IDEdit->text();
        if (newID != "")
        {
            if (newID != mSelectedID && !mSkillDataRoot[newID.toStdString()].isNull())
            {
                QMessageBox::information(window(), tr("File already exists"),
                                     tr("The ID already exists, please use different ID."));
                return false;
            }

            // Remove old member
            mSkillDataRoot.removeMember(mSelectedID.toStdString());

            // Set new ID
            mSelectedID = newID;

            // Save new data
            std::string skillID = mSelectedID.toStdString();
            mSkillDataRoot.removeMember(skillID);
            mSkillDataRoot[skillID]["name"] = m_ui->nameEdit->text().toStdString();
            mSkillDataRoot[skillID]["desc"] = m_ui->descEdit->toPlainText().toStdString();
            mSkillDataRoot[skillID]["castingAnimation"] = SKILL_DATA_DIRECTORY.append(m_ui->castingAnimationEdit->text()).toStdString();
            mSkillDataRoot[skillID]["skillAnimation"] = SKILL_DATA_DIRECTORY.append(m_ui->skillAnimationEdit->text()).toStdString();

            mSkillDataRoot[skillID]["multiTargetType"] = multiTargetType[m_ui->multiTargetCombox->currentIndex()].toStdString();
            if(m_ui->AOECheckBox->isChecked())
            {
                mSkillDataRoot[skillID]["AOEValue"] = AOETypes[m_ui->AOEComboBox->currentIndex()].toStdString();
            }

            mSkillDataRoot[skillID]["chainable"] = m_ui->chainableCheckbox->isChecked();
            mSkillDataRoot[skillID]["chainStartFrame"] = m_ui->chainStartFrameCombobox->value();


            mSkillDataRoot[skillID]["poseType"] = poseTypeString[m_ui->poseTypeCombobox->currentIndex()].toStdString();
            mSkillDataRoot[skillID]["skillType"] = skillTypeString[m_ui->skillTypeCombobox->currentIndex()].toStdString();

            // radio group 1
            mSkillDataRoot[skillID]["TargetDataOne"]["targetSelectionType"] = targetSelectionType[m_ui->OneSelectionTypeComboBox->currentIndex()].toStdString();
            mSkillDataRoot[skillID]["TargetDataOne"]["aliveCheck"] = m_ui->OneAliveCheckBox->isChecked();
            mSkillDataRoot[skillID]["TargetDataOne"]["deadCheck"] = m_ui->OneDeadCheckBox->isChecked();
            mSkillDataRoot[skillID]["TargetDataOne"]["selfCheck"] = m_ui->OneSelfCheckBox->isChecked();
            mSkillDataRoot[skillID]["TargetDataOne"]["opponentCheck"] = m_ui->OneOpponentCheckBox->isChecked();
            mSkillDataRoot[skillID]["TargetDataOne"]["allyCheck"] = m_ui->OneAllyCheckBox->isChecked();

            if (!m_ui->radioButtonOne->isChecked())
            {
                // radio group 2
                mSkillDataRoot[skillID]["TargetDataTwo"]["targetSelectionType"] = targetSelectionType[m_ui->TwoSelectionTypeComboBox->currentIndex()].toStdString();
                mSkillDataRoot[skillID]["TargetDataTwo"]["aliveCheck"] = m_ui->TwoAliveCheckBox->isChecked();
                mSkillDataRoot[skillID]["TargetDataTwo"]["deadCheck"] = m_ui->TwoDeadCheckBox->isChecked();
                mSkillDataRoot[skillID]["TargetDataTwo"]["selfCheck"] = m_ui->TwoSelfCheckBox->isChecked();
                mSkillDataRoot[skillID]["TargetDataTwo"]["opponentCheck"] = m_ui->TwoOpponentCheckBox->isChecked();
                mSkillDataRoot[skillID]["TargetDataTwo"]["allyCheck"] = m_ui->TwoAllyCheckBox->isChecked();
            }

            Json::Value& damages = mSkillDataRoot[skillID]["Damages"];
            for (int i = 0; i < m_ui->tableWidget->rowCount(); i++)
            {
                for (int j = 0; j < m_ui->tableWidget->columnCount(); j++)
                {
                    QTableWidgetItem* pWidgetItem = m_ui->tableWidget->item(i, j);
                    if (pWidgetItem)
                    {
                        QVariant data = pWidgetItem->data(Qt::EditRole);
                        QString t = data.toString();
                        if (t != "")
                        {
                            damages[static_cast<int>(i)][damageFields[j].toStdString()] = t.toStdString();
                        }
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void RogaSkillEditor::loadSkillData()
{
    clearUI();

    m_ui->IDEdit->setText(mSelectedID);
    std::string skillID = mSelectedID.toStdString();
    if (mSkillDataRoot.isMember(skillID))
    {
        if (!mSkillDataRoot[skillID].isNull())
        {
            Json::Value& skillData = mSkillDataRoot[skillID];
            if(skillData.isObject())
            {
                if(!skillData["name"].isNull())
                {
                     m_ui->nameEdit->setText(QString::fromStdString(mSkillDataRoot[skillID]["name"].asString()));
                }
                if(!skillData["desc"].isNull())
                {
                     m_ui->descEdit->setPlainText(QString::fromStdString(mSkillDataRoot[skillID]["desc"].asString()));
                }
                if(!skillData["castingAnimation"].isNull())
                {
                    QString str = QString::fromStdString(mSkillDataRoot[skillID]["castingAnimation"].asString());
                    str.replace(SKILL_DATA_DIRECTORY, "");
                    m_ui->castingAnimationEdit->setText(str);
                }
                if(!skillData["skillAnimation"].isNull())
                {
                    QString str = QString::fromStdString(mSkillDataRoot[skillID]["skillAnimation"].asString());
                    str.replace(SKILL_DATA_DIRECTORY, "");
                    m_ui->skillAnimationEdit->setText(str);
                }

                if(!skillData["multiTargetType"].isNull())
                {
                     m_ui->multiTargetCombox->setCurrentIndex(getIndexOfMultiTargetType(QString::fromStdString(mSkillDataRoot[skillID]["multiTargetType"].asString())));
                }

                if(!skillData["AOEValue"].isNull())
                {
                    m_ui->AOECheckBox->setChecked(true);
                    m_ui->AOEComboBox->setEnabled(true);
                    m_ui->AOEComboBox->setCurrentIndex(getIndexOfAOEType(QString::fromStdString(mSkillDataRoot[skillID]["AOEValue"].asString())));
                }
                else
                {
                    m_ui->AOECheckBox->setChecked(false);
                    m_ui->AOEComboBox->setEnabled(false);
                }
                if(skillData["chainable"].isBool())
                {
                    m_ui->chainableCheckbox->setChecked(skillData["chainable"].asBool());
                }
                if(skillData["chainStartFrame"].isInt())
                {
                    m_ui->chainStartFrameCombobox->setValue(skillData["chainStartFrame"].asInt());
                }

                if(skillData["poseType"].isString())
                {
                    m_ui->poseTypeCombobox->setCurrentIndex((int)getPoseTypeFromString(QString::fromStdString(skillData["poseType"].asString())));
                }

                if(skillData["skillType"].isString())
                {
                    m_ui->skillTypeCombobox->setCurrentIndex((int)getSkillTypeFromString(QString::fromStdString(skillData["skillType"].asString())));
                }

                if(!skillData["TargetDataOne"].isNull())
                {
                    // radio group 1
                    Json::Value targetOne = mSkillDataRoot[skillID]["TargetDataOne"];
                    if (!targetOne["targetSelectionType"].isNull())
                    {
                        m_ui->OneSelectionTypeComboBox->setCurrentIndex(getIndexOfTargetSelectionType(QString::fromStdString(targetOne["targetSelectionType"].asString())));
                    }
                    if (!targetOne["aliveCheck"].isNull())
                    {
                        m_ui->OneAliveCheckBox->setChecked(targetOne["aliveCheck"].asBool());
                    }
                    if (!targetOne["deadCheck"].isNull())
                    {
                        m_ui->OneDeadCheckBox->setChecked(targetOne["deadCheck"].asBool());
                    }
                    if (!targetOne["selfCheck"].isNull())
                    {
                        m_ui->OneSelfCheckBox->setChecked(targetOne["selfCheck"].asBool());
                    }
                    if (!targetOne["opponentCheck"].isNull())
                    {
                        m_ui->OneOpponentCheckBox->setChecked(targetOne["opponentCheck"].asBool());
                    }
                    if (!targetOne["allyCheck"].isNull())
                    {
                        m_ui->OneAllyCheckBox->setChecked(targetOne["allyCheck"].asBool());
                    }
                }

                if(!skillData["TargetDataTwo"].isNull())
                {
                    m_ui->radioButtonTwo->setChecked(true);
                    // radio group 2
                    Json::Value targetTwo = mSkillDataRoot[skillID]["TargetDataTwo"];
                    if (!targetTwo["targetSelectionType"].isNull())
                    {
                        m_ui->TwoSelectionTypeComboBox->setCurrentIndex(getIndexOfTargetSelectionType(QString::fromStdString(targetTwo["targetSelectionType"].asString())));
                    }
                    if (!targetTwo["aliveCheck"].isNull())
                    {
                        m_ui->TwoAliveCheckBox->setChecked(targetTwo["aliveCheck"].asBool());
                    }
                    if (!targetTwo["deadCheck"].isNull())
                    {
                        m_ui->TwoDeadCheckBox->setChecked(targetTwo["deadCheck"].asBool());
                    }
                    if (!targetTwo["selfCheck"].isNull())
                    {
                        m_ui->TwoSelfCheckBox->setChecked(targetTwo["selfCheck"].asBool());
                    }
                    if (!targetTwo["opponentCheck"].isNull())
                    {
                        m_ui->TwoOpponentCheckBox->setChecked(targetTwo["opponentCheck"].asBool());
                    }
                    if (!targetTwo["allyCheck"].isNull())
                    {
                        m_ui->TwoAllyCheckBox->setChecked(targetTwo["allyCheck"].asBool());
                    }
                }

                Json::Value& damages = mSkillDataRoot[skillID]["Damages"];
                for (unsigned int i = 0; i < damages.size(); i++)
                {
                    Json::Value& skillData = damages[static_cast<int>(i)];
                    Json::ValueIterator iter;

                    // add a row to the bottom
                    m_ui->tableWidget->insertRow(m_ui->tableWidget->rowCount());

                    // set data to the bottom row
                    for (iter = skillData.begin(); iter != skillData.end(); iter++)
                    {
                        QString key = QString::fromStdString(iter.key().asString());
                        int index = getIndexOfDamageField(key);
                        std::string str = ((Json::Value)(*iter)).asString();
                        QString data = QString::fromStdString(str);
                        m_ui->tableWidget->setItem(m_ui->tableWidget->rowCount() - 1, index, new QTableWidgetItem(data));
                    }
                }
            }
        }
    }
}

void RogaSkillEditor::onApplyButtonClicked()
{
    if (saveSkillData())
    {
        mChanged = true;
        m_ui->skillListWidget->selectedItems().first()->setData(Qt::DisplayRole, QString(m_ui->IDEdit->text()));
    }
}

void RogaSkillEditor::onNewSelected()
{
    m_ui->skillListWidget->clear();
    mSkillDataRoot.clear();
    mSkillDataPath = "";
}

void RogaSkillEditor::onSaveAsSelected()
{
    QString filePath = QFileDialog::getSaveFileName
                (
                    this,
                    tr("Save"),
                    mSkillDataPath,
                    tr("Sprites (*.skl)")
                );

    save(filePath);
}

void RogaSkillEditor::onSaveSelected()
{
    if (mSkillDataPath != "")
    {
        save(mSkillDataPath);
    }
    else
    {
        onSaveAsSelected();
    }
}

void RogaSkillEditor::onOpenSelected()
{
    QString filePath = QFileDialog::getOpenFileName
                (
                    this,
                    tr("Open"),
                    mSkillDataPath,
                    tr("Sprites (*.skl)")
                );

    if (filePath!= "")
    {
        load(filePath);
    }
}

void RogaSkillEditor::save(QString filePath)
{
    if (filePath != "")
    {
        saveSkillData();
        FileLoader::save(filePath, mSkillDataRoot);
        mChanged = false;
    }
}

void RogaSkillEditor::load(QString filePath)
{
    mSkillDataRoot = FileLoader::load(filePath);
    mSkillDataPath = filePath;
    loadSkillData();

    refreshSkillList();
}
