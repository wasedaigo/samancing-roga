#include "RogaSkillEditor.h"
#include "ui_RogaSkillEditor.h"
#include "json/reader.h"
#include <QTextStream>

RogaSkillEditor::RogaSkillEditor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::RogaSkillEditor)
{
    m_ui->setupUi(this);
    mSkillTreeViewModel.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    mSkillTreeViewModel.setReadOnly(true);

    loadInitFile();

    m_ui->skillTreeView->setAutoScroll(true);
    m_ui->skillTreeView->setModel(&mSkillTreeViewModel);
    m_ui->skillTreeView->setRootIndex(mSkillTreeViewModel.index(mSkillDataPath) );
}

RogaSkillEditor::~RogaSkillEditor()
{
    delete m_ui;
}

bool RogaSkillEditor::loadInitFile()
{
    // Get json file data
    QFile file(QDir::currentPath().append("/init.json"));
    QString fileData;
    if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream stream( &file );
        while ( !stream.atEnd() ) {
            fileData = stream.readAll();
        }
        file.close();
    }
    std::string inputJson = fileData.toStdString();

    Json::Reader reader;
    Json::Value root;
    if(!reader.parse(inputJson, root))
    {
        printf("json parse error");
        return false;
    }

    mSkillDataPath = (QString::fromStdString(root["skillDataPath"].asString()).toUtf8());
    return true;
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
