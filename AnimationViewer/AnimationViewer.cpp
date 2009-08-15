#include "AnimationViewer.h"
#include "ui_AnimationViewer.h"
#include "AnimationViewer/AnimationViewerPanel.h"
#include "DataModels/AnimationModel.h"
#include <QTimer>
#include <QStandardItemModel>

AnimationViewer::AnimationViewer(QWidget* parent, AnimationModel* animationModel)
   :QWidget(parent),
    m_ui(new Ui::AnimationViewer),
    mpSelectedCelModel(new CelModel()),
    mpAnimationModel(animationModel),
    mAnimationStartFrameNo(0)
{
    m_ui->setupUi(this);

    mpTweenModel = new QStandardItemModel();
    mpTweenModel->appendRow(new QStandardItem(QString("Fix")));
    mpTweenModel->appendRow(new QStandardItem(QString("Tween")));
    m_ui->alphaTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->positionTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->rotationTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->scaleTweenTypeComboBox->setModel(mpTweenModel);

    mpAnimationPlayTimer = new QTimer(this);
    mpAnimationPlayTimer->setInterval(30);

    m_ui->gridCelDataBox->setEnabled(false);
    m_ui->celListTableWidget->setEnabled(false);

    // Add animation viewer (OpenGL Screen)
    mpAnimationViewerPanel = new AnimationViewerPanel(this, mpAnimationModel, mpSelectedCelModel);
    mpAnimationViewerPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpAnimationViewerPanel->setMinimumWidth(480);
    mpAnimationViewerPanel->setMinimumHeight(320);
    m_ui->animationViewerContainer->addWidget(mpAnimationViewerPanel);

    m_ui->celListTableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    //m_ui->celListTableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);

    // connect Cel model and controls
    connect(m_ui->alphaSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setAlpha(double)));
    connect(m_ui->alphaTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setAlphaTweenType(int)));

    connect(m_ui->positionXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setPositionX(int)));
    connect(m_ui->positionYSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setPositionY(int)));
    connect(m_ui->positionTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setPositionTweenType(int)));

    connect(m_ui->scaleXSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setScaleX(double)));
    connect(m_ui->scaleYSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setScaleY(double)));
    connect(m_ui->scaleTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setScaleTweenType(int)));

    connect(m_ui->rotationXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setRotationX(int)));
    connect(m_ui->rotationTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setRotationTweenType(int)));

    connect(m_ui->celNoSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setCelNo(int)));
    connect(m_ui->blendTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setBlendType(int)));
    connect(m_ui->lookAtTargetCheckBox, SIGNAL(toggled(bool)), mpSelectedCelModel, SLOT(setLookAtTarget(bool)));
    connect(m_ui->relativeToTargetCheckBox, SIGNAL(toggled(bool)), mpSelectedCelModel, SLOT(setRelativeToTarget(bool)));
    connect(m_ui->blurTargetCheckBox, SIGNAL(toggled(bool)), mpSelectedCelModel, SLOT(setBlur(bool)));

    connect(mpSelectedCelModel, SIGNAL(alphaChanged(double)), m_ui->alphaSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(positionXChanged(int)), m_ui->positionXSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(positionYChanged(int)), m_ui->positionYSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(scaleXChanged(double)), m_ui->scaleXSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(scaleYChanged(double)), m_ui->scaleYSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(rotationXChanged(int)), m_ui->rotationXSpinBox, SLOT(setValue(int)));

    // Refresh the screen if celdata changed
    connect(m_ui->positionXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->positionYSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->alphaSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->scaleXSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->scaleYSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->rotationXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(mpAnimationModel, SIGNAL(paletImageChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));

    // Cel selection
    connect(mpAnimationViewerPanel, SIGNAL(celSelected(CelModel::CelData*)), this, SLOT(onCelSelected(CelModel::CelData*)));
    connect(mpAnimationViewerPanel, SIGNAL(celUnselected()), this, SLOT(onCelUnselected()));
    connect(m_ui->celListTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(onCelHashCelClicked(int, int)));

    // connect switch texture button
    connect(m_ui->switchTextureButton, SIGNAL(clicked()), this, SLOT(onSwitchTextureButtonClicked()));

    // Connect with animation model
    connect(mpAnimationModel, SIGNAL(currentFrameNoChanged(int)), this, SLOT(onCurrentFrameNoChanged(int)));

    connect(mpAnimationModel, SIGNAL(celAdded(CelModel::CelData)), this, SLOT(onCelAdded(CelModel::CelData)));
    connect(mpAnimationModel, SIGNAL(celRemoved(CelModel::CelData)), this, SLOT(onCelRemoved(CelModel::CelData)));
    connect(mpAnimationModel, SIGNAL(currentFrameNoChanged(int)), this, SLOT(onCurrentFrameNoChanged(int)));

    // connect save button
    connect(m_ui->saveAnimationButton, SIGNAL(clicked()), this, SLOT(onSaveAnimationButtonClicked()));

    // connect LCD number
    connect(m_ui->horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(mpAnimationModel, SIGNAL(animationDurationChanged(int)), m_ui->maxFrameLcdNumber, SLOT(display(int)));
    connect(mpAnimationModel, SIGNAL(animationDurationChanged(int)), this, SLOT(onAnimationDurationChanged(int)));

    // connect Timer
    connect(m_ui->playAnimationButton, SIGNAL(clicked()), this, SLOT(onPlayButtonClicked()));
    connect(mpAnimationPlayTimer, SIGNAL(timeout()), this, SLOT(onTick()));
}

AnimationViewer::~AnimationViewer()
{
    delete mpTweenModel;
    mpAnimationPlayTimer->stop();
    delete mpAnimationPlayTimer;
    delete m_ui;
    delete mpSelectedCelModel;
}

// clear cells from the box
void AnimationViewer::clearCels()
{
    // remove all items
    for (int i = m_ui->celListTableWidget->rowCount(); i >= 0; i--)
    {
       m_ui->celListTableWidget->removeRow(i);
    }
}

int AnimationViewer::selectCelIndexOfCelHash(int celNo) const
{
    for (int i = 0; i < m_ui->celListTableWidget->rowCount(); i++)
    {
        QTableWidgetItem* pItem = m_ui->celListTableWidget->item(i, 0);
        int no = pItem->data(Qt::DisplayRole ).toInt();
        if (celNo == no)
        {
            return i;
        }
    }

    return -1;
}

void AnimationViewer::addCel(int celNo, QString comment)
{
    // Insert blank row to the bottom
    int rowCount = m_ui->celListTableWidget->rowCount();
    m_ui->celListTableWidget->insertRow(rowCount);

    // Cel No
    QTableWidgetItem* item = new QTableWidgetItem(QString("%0").arg(celNo));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    m_ui->celListTableWidget->setItem(rowCount, 0, item);
}

void AnimationViewer::removeCel(int celNo)
{
    int index = selectCelIndexOfCelHash(celNo);
    if (index >= 0)
    {
         m_ui->celListTableWidget->removeRow(index);
    }
}

void AnimationViewer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

// Animation Cel list box control
void AnimationViewer::onCelAdded(CelModel::CelData celData)
{
    addCel(celData.mCelNo, "");
}

void AnimationViewer::onCelRemoved(CelModel::CelData celData)
{
    removeCel(celData.mCelNo);
}

//Cel list Change
void AnimationViewer::onCelHashCelClicked(int row, int col)
{
    QTableWidgetItem* pItem = m_ui->celListTableWidget->item(row, 0);
    int celNo = pItem->data(Qt::EditRole ).toInt();
    mpAnimationViewerPanel->selectCel(celNo);
}

// Cel selected
void AnimationViewer::onCelSelected(CelModel::CelData* pCelData)
{
    m_ui->gridCelDataBox->setEnabled(true);
    if (pCelData)
    {
        m_ui->alphaSpinBox->setValue(pCelData->mSpriteDescriptor.mAlpha);
        m_ui->alphaTweenTypeComboBox->setCurrentIndex(pCelData->mAlphaTweenType);

        m_ui->positionXSpinBox->setValue((int)pCelData->mSpriteDescriptor.mPosition.mX);
        m_ui->positionYSpinBox->setValue((int)pCelData->mSpriteDescriptor.mPosition.mY);
        m_ui->positionTweenTypeComboBox->setCurrentIndex(pCelData->mPositionTweenType);

        m_ui->scaleXSpinBox->setValue((double)pCelData->mSpriteDescriptor.mScale.mX);
        m_ui->scaleYSpinBox->setValue((double)pCelData->mSpriteDescriptor.mScale.mY);
        m_ui->scaleTweenTypeComboBox->setCurrentIndex(pCelData->mScaleTweenType);

        m_ui->rotationXSpinBox->setValue((int)pCelData->mSpriteDescriptor.mRotation.mX);
        m_ui->rotationTweenTypeComboBox->setCurrentIndex(pCelData->mRotationTweenType);

        m_ui->celNoSpinBox->setValue(pCelData->mCelNo);
        m_ui->blurTargetCheckBox->setChecked(pCelData->mBlur);
        m_ui->relativeToTargetCheckBox->setChecked(pCelData->mRelativeToTarget);
        m_ui->lookAtTargetCheckBox->setChecked(pCelData->mLookAtTarget);
        m_ui->blendTypeComboBox->setCurrentIndex(pCelData->mSpriteDescriptor.mBlendType);

        // Select an item from celList
        int index = selectCelIndexOfCelHash(pCelData->mCelNo);
        if (index >= 0)
        {
            m_ui->celListTableWidget->selectRow(index);
        }
    }
}

void AnimationViewer::onCelUnselected()
{
    m_ui->gridCelDataBox->setEnabled(false);
}

//Switch Texture
void AnimationViewer::onSwitchTextureButtonClicked()
{

}

//Change Keyframe no
void AnimationViewer::onCurrentFrameNoChanged(int frameNo)
{
    m_ui->frameLcdNumber->display(frameNo);
    if (this->isEnabled())
    {
        // remove all cels from cellist
        clearCels();

        // Move horizontal slider arrow
        m_ui->horizontalSlider->setValue(frameNo + 1);

        if (mpAnimationModel->isKeyFrame(frameNo))
        {
            m_ui->gridCelDataBox->setEnabled(false);
            m_ui->celListTableWidget->setEnabled(true);

            // Load cels
            if (mpAnimationModel->getCurrentKeyFrameNo() >= 0)
            {
                QHash<int, CelModel::CelData> celList = mpAnimationModel->getCurrentSortedCelHash();
                QHash<int, CelModel::CelData>::Iterator iter = celList.begin();
                while (iter != celList.end())
                {
                    CelModel::CelData celData = (CelModel::CelData)*iter;
                    addCel(celData.mCelNo, "");
                    iter++;
                }
            }
        }
        else
        {
            m_ui->gridCelDataBox->setEnabled(false);
            m_ui->celListTableWidget->setEnabled(false);
        }
    }
}

void AnimationViewer::onSliderValueChanged(int value)
{
    mpAnimationModel->setCurrentFrameNo(value - 1);
}

void AnimationViewer::onAnimationDurationChanged(int duration)
{
    m_ui->horizontalSlider->setMaximum(duration);
}

void AnimationViewer::onPlayButtonClicked()
{
    mpAnimationPlayTimer->start();
    mpAnimationViewerPanel->playAnimation();
    mAnimationStartFrameNo = mpAnimationModel->getCurrentFrameNo();
    mpAnimationModel->setCurrentFrameNo(0);
    emit playAnimation(false);
}

void AnimationViewer::onTick()
{
    if (mpAnimationModel->getCurrentFrameNo() == mpAnimationModel->getAnimationDuration() - 1)
    {
        mpAnimationViewerPanel->stopAnimation();
        mpAnimationPlayTimer->stop();
        emit playAnimation(true);
        mpAnimationModel->setCurrentFrameNo(mAnimationStartFrameNo);
    }
    else
    {
        mpAnimationViewerPanel->gotoNextFrame();
    }
}


// Save Button
void AnimationViewer::onSaveAnimationButtonClicked()
{

}
