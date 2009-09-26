#include "RogaSkillEditor.h"
#include "ui_RogaSkillEditor.h"

#include <QMessageBox>
#include <QTextStream>
#include <QCloseEvent>

#define SKILL_FORMAT ".skl"

#define DAMAGE_FIELD_COUNT 7
static QString damageFields[DAMAGE_FIELD_COUNT] = {"min", "max", "random", "hit", "critical", "attr", "damagedef"};
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

RogaSkillEditor::RogaSkillEditor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::RogaSkillEditor),
    mSelectedID(QString(""))
{
    m_ui->setupUi(this);
    mSkillTreeViewModel.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    mSkillTreeViewModel.setReadOnly(true);

    loadInitFile();
    refreshSkillList();


    connect(m_ui->radioButtonOne, SIGNAL(clicked(bool)), this, SLOT(refreshTabControl()));
    connect(m_ui->radioButtonTwo, SIGNAL(clicked(bool)), this, SLOT(refreshTabControl()));

    connect(m_ui->addSkillButton, SIGNAL(clicked()), this, SLOT(onAddSkillButtonClicked()));
    connect(m_ui->removeSkillButton, SIGNAL(clicked()), this, SLOT(onRemoveSkillButtonClicked()));
    connect(m_ui->saveButton, SIGNAL(clicked()), this, SLOT(onSaveSkillButtonClicked()));
    connect(m_ui->addSkillDataButton, SIGNAL(clicked()), this, SLOT(onAddSkillDataButtonClicked()));
    connect(m_ui->removeSkillDataButton, SIGNAL(clicked()), this, SLOT(onRemoveSkillDataButtonClicked()));
    connect(m_ui->skillListWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(onSkillSelected(QModelIndex)));

    refreshTabControl();
    m_ui->skillListWidget->setCurrentRow(0);
    skillLoad(0);
}

RogaSkillEditor::~RogaSkillEditor()
{
    delete m_ui;
}

std::string RogaSkillEditor::getFileData(QString path)
{
    // Get json file data
    QFile file(path);
    QString fileData;
    if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file );
        while ( !stream.atEnd() ) {
            fileData = stream.readAll();
        }
        file.close();
    }
    return fileData.toStdString();
}

Json::Value RogaSkillEditor::loadJsonFile(QString path)
{
    std::string inputJson = getFileData(path);

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(inputJson, root))
    {
        printf("json parse error");
    }
    return root;
}

void RogaSkillEditor::loadInitFile()
{
    Json::Value root = loadJsonFile(QDir::currentPath().append("/").append("init.json"));
    mSkillDataPath = QString::fromStdString(root["skillResourcePath"].asString());
    mSkillDataRoot = loadJsonFile(mSkillDataPath);
}

void RogaSkillEditor::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
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
}

void RogaSkillEditor::addEmptySkillData()
{
    //QModelIndexList indexes = m_ui->skillListWidget->addItem();
}



void RogaSkillEditor::closeEvent(QCloseEvent *event)
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

    saveToFile();
}

void RogaSkillEditor::onRemoveSkillButtonClicked()
{
    QModelIndexList indexes = m_ui->skillListWidget->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QVariant data = m_ui->skillListWidget->selectedItems().takeFirst()->data(Qt::DisplayRole);
        QString filename = data.toString();
        mSkillDataRoot.removeMember(filename.toStdString());
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
        QModelIndex index = indexes.takeFirst();
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
        QVariant data = m_ui->skillListWidget->selectedItems().takeFirst()->data(Qt::DisplayRole);
        mSelectedID = data.toString();

        loadSkillData();
    }
}

void RogaSkillEditor::onSaveSkillButtonClicked()
{
    saveSkillData();
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
    m_ui->AOESpinBox->setValue(0);
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
}

void RogaSkillEditor::saveToFile()
{
    // Save to file
    Json::StyledWriter writer;
    std::string outputJson = writer.write(mSkillDataRoot);

    QFile file(mSkillDataPath);
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        file.write(outputJson.c_str(), outputJson.length());
        file.close();
    }

    refreshSkillList();
}
void RogaSkillEditor::saveSkillData()
{
    // Save Data
    QString newID =  m_ui->IDEdit->text();
    if (mSelectedID != "" && newID != "")
    {
        if (newID != mSelectedID && !mSkillDataRoot[newID.toStdString()].isNull())
        {
            QMessageBox::information(window(), tr("File already exists"),
                                 tr("The ID already exists, please use different ID."));
            return;
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
        mSkillDataRoot[skillID]["castingAnimation"] = m_ui->castingAnimationEdit->text().toStdString();
        mSkillDataRoot[skillID]["skillAnimation"] = m_ui->skillAnimationEdit->text().toStdString();
        mSkillDataRoot[skillID]["multiTargetType"] = multiTargetType[m_ui->multiTargetCombox->currentIndex()].toStdString();
        if(m_ui->AOECheckBox->isChecked())
        {
            mSkillDataRoot[skillID]["AOEValue"] = m_ui->AOESpinBox->value();
        }

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
                    damages[static_cast<int>(i)][damageFields[j].toStdString()] = data.toString().toStdString();
                }
            }
        }
    }

    saveToFile();
}

void RogaSkillEditor::loadSkillData()
{
    clearUI();

    m_ui->IDEdit->setText(mSelectedID);
    std::string skillID = mSelectedID.toStdString();
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
                 m_ui->castingAnimationEdit->setText(QString::fromStdString(mSkillDataRoot[skillID]["castingAnimation"].asString()));
            }
            if(!skillData["skillAnimation"].isNull())
            {
                 m_ui->skillAnimationEdit->setText(QString::fromStdString(mSkillDataRoot[skillID]["skillAnimation"].asString()));
            }

            if(!skillData["multiTargetType"].isNull())
            {
                 m_ui->multiTargetCombox->setCurrentIndex(getIndexOfMultiTargetType(QString::fromStdString(mSkillDataRoot[skillID]["multiTargetType"].asString())));
            }

            if(!skillData["AOEValue"].isNull())
            {
                m_ui->AOECheckBox->setChecked(true);
                m_ui->AOESpinBox->setValue(mSkillDataRoot[skillID]["AOEValue"].asInt());
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
