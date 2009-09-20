#ifndef ROGASKILLEDITOR_H
#define ROGASKILLEDITOR_H

#include <QtGui/QMainWindow>
#include "SourceFileDirModel.h"
#include "json/reader.h"
#include "json/writer.h"

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
    void closeEvent(QCloseEvent *event);


private slots:
    void onAddSkillDataButtonClicked();
    void onRemoveSkillDataButtonClicked();
    void onAddSkillButtonClicked();
    void onRemoveSkillButtonClicked();
    void onSaveSkillButtonClicked();
    void refreshTabControl();
    void onSkillSelected(QModelIndex index);

private:
    std::string RogaSkillEditor::getFileData(QString path);

    Json::Value RogaSkillEditor::loadJsonFile(QString path);
    void loadInitFile();
    void refreshSkillList();

    void addEmptySkillData();

    void clearUI();

    void skillLoad(int row);
    void saveSkillData();
    void loadSkillData();
    void saveToFile();

    Ui::RogaSkillEditor *m_ui;
    SourceFileDirModel mSkillTreeViewModel;
    QString mSkillDataPath;
    QString mSelectedID;
    Json::Value mSkillDataRoot;
};

#endif // ROGASKILLEDITOR_H
