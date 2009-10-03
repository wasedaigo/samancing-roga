#ifndef SpriteEditor_H
#define SpriteEditor_H

#include <QtGui/QMainWindow>
#include "json/reader.h"
#include "json/writer.h"

#include "SpriteData.h"
#include "SourceFileDirModel.h"

namespace Ui {
    class SpriteEditor;
}

class ImageViewer;
class QItemSelection;
class SpriteEditor : public QMainWindow
{
    Q_OBJECT
public:
    SpriteEditor(QWidget *parent = 0);
    ~SpriteEditor();


protected:
    void closeEvent(QCloseEvent *event);
    ImageViewer *mpImageViewer;

private slots:
    void onNewSelected();
    void onSaveSelected();
    void onSaveAsSelected();
    void onOpenSelected();
    void onSetResourceDirSelected();
    void onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2);
    void onUpdateSpritesSelected();

private:
    void init();

    void load(QString filePath);
    void save(QString filePath);
    void resetResourceDirectory();

    Ui::SpriteEditor *m_ui;

    SpriteData mSpriteData;

    SourceFileDirModel mResourceTreeViewModel;
};

#endif // SpriteEditor_H
