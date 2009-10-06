#ifndef ROGAMonsterEDITOR_H
#define ROGAMonsterEDITOR_H

#include <QtGui/QMainWindow>
#include "json/reader.h"
#include "json/writer.h"
#include <QModelIndex>

namespace Ui {
    class RogaMonsterEditor;
}

class ImageViewer;
class RogaMonsterEditor : public QMainWindow {
    Q_OBJECT
public:
    RogaMonsterEditor(QWidget *parent = 0);
    ~RogaMonsterEditor();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void onAddMonsterDataButtonClicked();
    void onRemoveMonsterDataButtonClicked();
    void onAddMonsterButtonClicked();
    void onRemoveMonsterButtonClicked();

    void onMonsterSelected(QModelIndex index);

    void onApplyButtonClicked();
    void onNewSelected();
    void onSaveAsSelected();
    void onSaveSelected();
    void onOpenSelected();
    void onSetResourceDirSelected();
    void onMonsterGraphicSelected();
private:
    void init();

    void loadUIFaceImage();
    void loadMonsterImage(QString monsterFilePath);
    QString getRelativeMonsterGraphicPath(QString monsterFilePath) const;

    void refreshMonsterList();
    void clearUI();

    void MonsterLoad(int row);
    bool saveMonsterData();
    void loadMonsterData();
    void save(QString filePath);
    void load(QString filePath);

    Ui::RogaMonsterEditor *m_ui;

    ImageViewer *mpFaceIconViewer;
    ImageViewer *mpMonsterGraphicViewer;

    QString mMonsterDataPath;
    QString mSelectedID;
    QString mResourceDirPath;
    
    Json::Value mMonsterDataRoot;

    bool mChanged;
};

#endif // ROGAMonsterEDITOR_H
