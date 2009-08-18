#include "imagepaletdialog.h"
#include "ui_imagepaletdialog.h"
#include "DataModels/AnimationModel.h"
#include "ImagePaletDialog/AnimationImagePaletPanel.h"
#include "Common.h"
#include <QStandardItemModel>
#include <QPixmap>
#include <QString>

ImagePaletDialog::ImagePaletDialog(int paletNo, QWidget *parent, AnimationModel* pAnimationModel, QStandardItemModel* pAnimationImageListModel)
    : QDialog(parent),
    m_ui(new Ui::ImagePaletDialog),
    mPaletNo(paletNo),
    mpAnimationModel(pAnimationModel),
    mpAnimationImageListModel(pAnimationImageListModel)
{
    m_ui->setupUi(this);
    mpAnimationImagePaletPanel = new AnimationImagePaletPanel(paletNo, pAnimationModel);
    mpAnimationImagePaletPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ui->scrollArea->setWidget(mpAnimationImagePaletPanel);
    m_ui->animationImageComboBox->setModel(mpAnimationImageListModel);

    connect(m_ui->animationImageComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onAnimationImageComboBoxChanged(QString)));

    connect(m_ui->spinBoxGridX, SIGNAL(valueChanged(int)), this, SLOT(snapGridChanged()));
    connect(m_ui->spinBoxGridY, SIGNAL(valueChanged(int)), this, SLOT(snapGridChanged()));
    connect(m_ui->snapGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(snapGridChanged()));
    
    connect(mpAnimationModel, SIGNAL(animationImagePaletChanged(int, QString)), this, SLOT(onAnimationImagePaletChanged(int, QString)));
    snapGridChanged();
}

ImagePaletDialog::~ImagePaletDialog()
{
    delete m_ui;
}

void ImagePaletDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ImagePaletDialog::snapGridChanged()
{
    mpAnimationImagePaletPanel->setSnapGrid(m_ui->spinBoxGridX->value(), m_ui->spinBoxGridY->value(), m_ui->snapGridCheckBox->isChecked());
}

void ImagePaletDialog::onAnimationImagePaletChanged(int paletNo, QString id)
{
    if (mPaletNo == paletNo)
    {
        int index = m_ui->animationImageComboBox->findText(id, Qt::MatchCaseSensitive);
        m_ui->animationImageComboBox->setCurrentIndex(index);
    }
}

void ImagePaletDialog::setPaletImage(int paletNo, QString id)
{
    if (id != NONE)
    {
        mpAnimationModel->setAnimationImagePalet(paletNo, id);
    }
}

void ImagePaletDialog::onAnimationImageComboBoxChanged(QString id)
{
    setPaletImage(mPaletNo, id);
}
