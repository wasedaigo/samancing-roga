#ifndef IMAGEPALETDIALOG_H
#define IMAGEPALETDIALOG_H

#include <QtGui/QDialog>
#include <QDir>

namespace Ui {
    class ImagePaletDialog;
}

class AnimationModel;
class AnimationImagePaletPanel;
class QStandardItemModel;
class ImagePaletDialog : public QDialog {
    Q_OBJECT
public:
    ImagePaletDialog(int paletNo, QWidget *parent, AnimationModel* pAnimationModel, QDir animationImageDirectory, QStandardItemModel* pAnimationImageListModel);
    ~ImagePaletDialog();

private slots:
    void onAnimationImageComboBoxChanged(QString id);
    void snapGridChanged();

protected:
    void changeEvent(QEvent *e);

private:
    void setPaletImage(int paletNo, QString id);

    Ui::ImagePaletDialog *m_ui;
    int mPaletNo;
    AnimationModel* mpAnimationModel;
    QDir mAnimationImageDirectory;
    QStandardItemModel* mpAnimationImageListModel;
    AnimationImagePaletPanel* mpAnimationImagePaletPanel;
};

#endif // IMAGEPALETDIALOG_H
