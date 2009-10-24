#ifndef ROGASKILLEDITOR_H
#define ROGASKILLEDITOR_H

#include <QtGui/QMainWindow>
#include "json/reader.h"
#include "json/writer.h"
#include <QModelIndex>
#include <QStandardItemModel>

namespace Ui {
    class RogaSkillEditor;
}

class RogaSkillEditor : public QMainWindow {
    Q_OBJECT
public:
    RogaSkillEditor(QWidget *parent = 0);
    ~RogaSkillEditor();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void onAddSkillDataButtonClicked();
    void onRemoveSkillDataButtonClicked();
    void onAddSkillButtonClicked();
    void onRemoveSkillButtonClicked();
    void refreshTabControl();
    void onSkillSelected(QModelIndex index);

    void onApplyButtonClicked();
    void onNewSelected();
    void onSaveAsSelected();
    void onSaveSelected();
    void onOpenSelected();

private:
    void init();

    void refreshSkillList();
    void clearUI();

    void skillLoad(int row);
    bool saveSkillData();
    void loadSkillData();
    void save(QString filePath);
    void load(QString filePath);

    Ui::RogaSkillEditor *m_ui;

    QStandardItemModel* mpImmunityRateTypeModel;
    QString mSkillDataPath;
    QString mSelectedID;
    Json::Value mSkillDataRoot;

    bool mChanged;
};

#endif // ROGASKILLEDITOR_H
