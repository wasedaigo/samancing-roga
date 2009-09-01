#include "AnimationViewer.h"
#include "ui_AnimationViewer.h"
#include "AnimationViewer/AnimationViewerPanel.h"
#include "DataModels/AnimationModel.h"
#include "DataModels/CelModel.h"
#include <QTimer>
#include <QStandardItemModel>

AnimationViewer::AnimationViewer(QWidget* parent, AnimationModel* animationModel)
   :QWidget(parent),
    m_ui(new Ui::AnimationViewer),
    mpSelectedCelModel(new CelModel()),
    mpAnimationModel(animationModel)
{
    mSelectedKeyFramePosition.mFrameNo = -1;
    mSelectedKeyFramePosition.mLineNo = -1;

    m_ui->setupUi(this);

    mpTweenModel = new QStandardItemModel();
    // Set Tween types
    mpTweenModel->appendRow(new QStandardItem(QString("None")));
    mpTweenModel->appendRow(new QStandardItem(QString("Fix")));
    mpTweenModel->appendRow(new QStandardItem(QString("Tween")));

    m_ui->alphaTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->positionTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->rotationTweenTypeComboBox->setModel(mpTweenModel);
    m_ui->scaleTweenTypeComboBox->setModel(mpTweenModel);

    mpAnimationPlayTimer = new QTimer(this);
    mpAnimationPlayTimer->setInterval(30);

    m_ui->gridCelDataBox->setEnabled(false);

    // Add animation viewer (OpenGL Screen)
    mpAnimationViewerPanel = new AnimationViewerPanel(this, mpAnimationModel, mpSelectedCelModel);
    mpAnimationViewerPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mpAnimationViewerPanel->setMinimumWidth(480);
    mpAnimationViewerPanel->setMinimumHeight(320);
    m_ui->animationViewerContainer->addWidget(mpAnimationViewerPanel);


    // connect Cel model and controls
    connect(m_ui->alphaSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setAlpha(double)));
    connect(m_ui->alphaTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setAlphaTweenType(int)));
    connect(m_ui->alphaTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->positionXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setPositionX(int)));
    connect(m_ui->positionYSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setPositionY(int)));
    connect(m_ui->positionTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setPositionTweenType(int)));
    connect(m_ui->positionTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->scaleXSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setScaleX(double)));
    connect(m_ui->scaleYSpinBox, SIGNAL(valueChanged(double)), mpSelectedCelModel, SLOT(setScaleY(double)));
    connect(m_ui->scaleTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setScaleTweenType(int)));
    connect(m_ui->scaleTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->rotationXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setRotationX(int)));
    connect(m_ui->rotationTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setRotationTweenType(int)));
    connect(m_ui->rotationTweenTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onTweenTypeChanged(int)));

    connect(m_ui->blendTypeComboBox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setBlendType(int)));
    connect(m_ui->facingOptionCombobox, SIGNAL(currentIndexChanged(int)), mpSelectedCelModel, SLOT(setFacingOptionType(int)));
    connect(m_ui->relativeToTargetCheckBox, SIGNAL(toggled(bool)), mpSelectedCelModel, SLOT(setRelativeToTarget(bool)));
    connect(m_ui->blurSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setBlur(int)));

    connect(m_ui->centerXSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setCenterX(int)));
    connect(m_ui->centerYSpinBox, SIGNAL(valueChanged(int)), mpSelectedCelModel, SLOT(setCenterY(int)));

    connect(mpSelectedCelModel, SIGNAL(alphaChanged(double)), m_ui->alphaSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(positionXChanged(int)), m_ui->positionXSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(positionYChanged(int)), m_ui->positionYSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(scaleXChanged(double)), m_ui->scaleXSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(scaleYChanged(double)), m_ui->scaleYSpinBox, SLOT(setValue(double)));
    connect(mpSelectedCelModel, SIGNAL(rotationXChanged(int)), m_ui->rotationXSpinBox, SLOT(setValue(int)));

    connect(mpSelectedCelModel, SIGNAL(centerXChanged(int)), m_ui->centerXSpinBox, SLOT(setValue(int)));
    connect(mpSelectedCelModel, SIGNAL(centerYChanged(int)), m_ui->centerYSpinBox, SLOT(setValue(int)));

    // Refresh the screen if KeyFrameData changed
    connect(m_ui->positionXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->positionYSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->alphaSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->scaleXSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->scaleYSpinBox, SIGNAL(valueChanged(double)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->rotationXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->blendTypeComboBox, SIGNAL(currentIndexChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->facingOptionCombobox, SIGNAL(currentIndexChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->relativeToTargetCheckBox, SIGNAL(toggled(bool)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->blurSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->centerXSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));
    connect(m_ui->centerYSpinBox, SIGNAL(valueChanged(int)), mpAnimationViewerPanel, SLOT(refresh()));

    // Cel selection
    connect(mpAnimationViewerPanel, SIGNAL(celSelected(KeyFrameData*)), this, SLOT(onCelSelected(KeyFrameData*)));

    // connect save button
    connect(m_ui->saveAnimationButton, SIGNAL(clicked()), this, SLOT(onSaveAnimationButtonClicked()));

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

// Cel selected
void AnimationViewer::onCelSelected(KeyFrameData* pKeyFrameData)
{
    if (pKeyFrameData)
    {
        m_ui->gridCelDataBox->setEnabled(true);
        
        m_ui->alphaSpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mAlpha);
        m_ui->alphaTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_alpha]);

        m_ui->positionXSpinBox->blockSignals(true);
        m_ui->positionXSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mPosition.mX);
        m_ui->positionXSpinBox->blockSignals(false);

        m_ui->positionYSpinBox->blockSignals(true);
        m_ui->positionYSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mPosition.mY);
        m_ui->positionYSpinBox->blockSignals(false);

        m_ui->positionTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_position]);

        m_ui->scaleXSpinBox->blockSignals(true);
        m_ui->scaleXSpinBox->setValue((double)pKeyFrameData->mSpriteDescriptor.mScale.mX);
        m_ui->scaleXSpinBox->blockSignals(false);

        m_ui->scaleYSpinBox->blockSignals(true);
        m_ui->scaleYSpinBox->setValue((double)pKeyFrameData->mSpriteDescriptor.mScale.mY);
        m_ui->scaleYSpinBox->blockSignals(false);

        m_ui->scaleTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_scale]);

        m_ui->rotationXSpinBox->blockSignals(true);
        m_ui->rotationXSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mRotation.mX);
        m_ui->rotationXSpinBox->blockSignals(false);

        m_ui->rotationTweenTypeComboBox->setCurrentIndex(pKeyFrameData->mTweenTypes[KeyFrameData::TweenAttribute_rotation]);

        m_ui->blurSpinBox->blockSignals(true);
        m_ui->blurSpinBox->setValue(pKeyFrameData->mSpriteDescriptor.mBlur);
        m_ui->blurSpinBox->blockSignals(false);

        m_ui->relativeToTargetCheckBox->blockSignals(true);
        m_ui->relativeToTargetCheckBox->setChecked(pKeyFrameData->mSpriteDescriptor.mRelativeToTarget);
        m_ui->relativeToTargetCheckBox->blockSignals(false);

        m_ui->facingOptionCombobox->blockSignals(true);
        m_ui->facingOptionCombobox->setCurrentIndex(pKeyFrameData->mSpriteDescriptor.mFacingOptionType);
        m_ui->facingOptionCombobox->blockSignals(false);

        m_ui->blendTypeComboBox->blockSignals(true);
        m_ui->blendTypeComboBox->setCurrentIndex(pKeyFrameData->mSpriteDescriptor.mBlendType);
        m_ui->blendTypeComboBox->blockSignals(false);

        m_ui->centerXSpinBox->blockSignals(true);
        m_ui->centerXSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mCenter.mX);
        m_ui->centerXSpinBox->blockSignals(false);

        m_ui->centerYSpinBox->blockSignals(true);
        m_ui->centerYSpinBox->setValue((int)pKeyFrameData->mSpriteDescriptor.mCenter.mY);
        m_ui->centerYSpinBox->blockSignals(false);
    }
    else
    {
        m_ui->gridCelDataBox->setEnabled(false);
    }
}

void AnimationViewer::onTweenTypeChanged(int tweenType)
{
    bool alphaTweenEnabled = (m_ui->alphaTweenTypeComboBox->currentIndex() != 0);
    m_ui->alphaSpinBox->setEnabled(alphaTweenEnabled);

    bool scaleTweenEnabled = (m_ui->scaleTweenTypeComboBox->currentIndex() != 0);
    m_ui->scaleXSpinBox->setEnabled(scaleTweenEnabled);
    m_ui->scaleYSpinBox->setEnabled(scaleTweenEnabled);

    bool positionTweenEnabled = (m_ui->positionTweenTypeComboBox->currentIndex() != 0);
    m_ui->positionXSpinBox->setEnabled(positionTweenEnabled);
    m_ui->positionYSpinBox->setEnabled(positionTweenEnabled);
    m_ui->positionZSpinBox->setEnabled(positionTweenEnabled);

    bool rotationTweenEnabled = (m_ui->rotationTweenTypeComboBox->currentIndex() != 0);
    m_ui->rotationXSpinBox->setEnabled(rotationTweenEnabled);

    mpAnimationModel->tellTimeLineToRefresh();
}

void AnimationViewer::onPlayButtonClicked()
{
    mpAnimationPlayTimer->start();
    mpAnimationViewerPanel->playAnimation();
    mpAnimationModel->getCurrentKeyFramePosition();
    mSelectedKeyFramePosition = mpAnimationModel->getCurrentKeyFramePosition();
    mpAnimationModel->selectCurrentKeyFramePosition(mSelectedKeyFramePosition.mLineNo, 0);
    emit playAnimation(false);
}

void AnimationViewer::onTick()
{
    if ( mpAnimationModel->getCurrentKeyFramePosition().mFrameNo == mpAnimationModel->getMaxFrameCount())
    {
        mpAnimationViewerPanel->stopAnimation();
        mpAnimationPlayTimer->stop();
        emit playAnimation(true);
        mpAnimationModel->selectCurrentKeyFramePosition(mSelectedKeyFramePosition.mLineNo, mSelectedKeyFramePosition.mFrameNo);
    }
    else
    {
        mpAnimationViewerPanel->gotoNextFrame();
    }
}


// Save Button
void AnimationViewer::onSaveAnimationButtonClicked()
{
    mpAnimationModel->saveData();
}
