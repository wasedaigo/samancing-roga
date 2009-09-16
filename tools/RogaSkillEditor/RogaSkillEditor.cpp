#include "RogaSkillEditor.h"
#include "ui_RogaSkillEditor.h"

RogaSkillEditor::RogaSkillEditor(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::RogaSkillEditor)
{
    m_ui->setupUi(this);
}

RogaSkillEditor::~RogaSkillEditor()
{
    delete m_ui;
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
