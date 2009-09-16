#ifndef ROGASKILLEDITOR_H
#define ROGASKILLEDITOR_H

#include <QtGui/QMainWindow>

namespace Ui {
    class RogaSkillEditor;
}

class RogaSkillEditor : public QMainWindow {
    Q_OBJECT
public:
    RogaSkillEditor(QWidget *parent = 0);
    ~RogaSkillEditor();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RogaSkillEditor *m_ui;
};

#endif // ROGASKILLEDITOR_H
