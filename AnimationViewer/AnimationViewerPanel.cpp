#include "AnimationViewerPanel.h"
#include "DataModels/AnimationModel.h"
#include "GLSprite.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QColor>

#define TARGET_SCREEN_WIDTH 320
#define TARGET_SCREEN_HEIGHT 240

AnimationViewerPanel::AnimationViewerPanel(QWidget* parent, AnimationModel* pAnimationModel, CelModel* const pSelectedCelModel)
        : QWidget(parent),
          mpAnimationModel(pAnimationModel),
          mCurrentFrameNo(),
          mClearColor(Qt::black),
          mSelectedOffset(QPoint(0, 0)),
          mpSelectedCelModel(pSelectedCelModel),
          mIsAnimationPlaying(false)
{
    setAutoFillBackground(false);

    connect(mpAnimationModel, SIGNAL(currentFrameNoChanged(int)), this, SLOT(onCurrentFrameNoChanged(int)));
    connect(mpAnimationModel, SIGNAL(celAdded(CelModel::CelData)), this, SLOT(onCelAdded(CelModel::CelData)));

    //QTimer* mpTimer = new QTimer();
    //mpTimer->start(10);
    //connect(mpTimer,SIGNAL(timeout()), this , SLOT(timerEvent()));
}

AnimationViewerPanel::~AnimationViewerPanel()
{
    //delete mpTimer;
}

void AnimationViewerPanel::playAnimation()
{
    mIsAnimationPlaying = true;
}

void AnimationViewerPanel::stopAnimation()
{
    mIsAnimationPlaying = false;
}

void AnimationViewerPanel::gotoNextFrame()
{
    int frameNo =  mpAnimationModel->getCurrentFrameNo();
    if (frameNo < mpAnimationModel->getAnimationDuration() - 1)
    {
       mpAnimationModel->setCurrentFrameNo(frameNo + 1);
       refresh();
    }
}

void AnimationViewerPanel::unselectCels()
{
    if (mpSelectedCelModel->getCelDataRef())
    {
        mpSelectedCelModel->setCelDataRef(NULL);
        emit celUnselected();
    }
}

void AnimationViewerPanel::setFrame(int frameNo)
{
    while (!mGlSpriteList.isEmpty())
    {
        delete mGlSpriteList.takeFirst();
    }
}

void AnimationViewerPanel::resizeEvent(QResizeEvent *event)
{
    repaint();
}

QPoint AnimationViewerPanel::getCenterPoint() const
{
    return QPoint(
        (width() - TARGET_SCREEN_WIDTH) / 2,
        (height() - TARGET_SCREEN_HEIGHT) / 2
    );
}

void AnimationViewerPanel::keyPressEvent (QKeyEvent* e)
{
    // Delete selected cel
    if (e->key() == Qt::Key_Delete)
    {
        if (mpSelectedCelModel->getCelDataRef())
        {
            mpAnimationModel->removeCelData(
                    mpAnimationModel->getCurrentKeyFrameNo(),
                    mpSelectedCelModel->getCelDataRef()->mCelNo
                    );

            unselectCels();
            refresh();
        }
    }
}

void AnimationViewerPanel::paintEvent(QPaintEvent *event)
{
    // Get center point, all cel position should be relative to this
    QPoint centerPoint = getCenterPoint();

    // Start painter
    QPainter painter(this);

    // clear screen
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.fillRect(QRect(0, 0, width() - 1, height() - 1), Qt::SolidPattern);

    // Antialiazing will make them look nicer
    painter.setRenderHint(QPainter::Antialiasing);

    // Color of helper lines
    painter.setPen(QColor(120, 150, 200));

    // Draw Screen Size Indicator
    painter.drawRect(QRect(
                    centerPoint.x(),
                    centerPoint.y(),
                    TARGET_SCREEN_WIDTH,
                    TARGET_SCREEN_HEIGHT
                    )
    );

    // Draw cross
    painter.drawLine(QPoint(0, height() / 2), QPoint(width(), height() / 2));
    painter.drawLine(QPoint(width() / 2, 0), QPoint(width() / 2, height()));

    // Render all sprites
    QList<GLSprite*>::Iterator iter = mGlSpriteList.begin();
    while (iter != mGlSpriteList.end())
    {
        GLSprite* glSprite = (GLSprite*)*iter;
        if (!mIsAnimationPlaying && mpAnimationModel->isKeyFrame(mpAnimationModel->getCurrentFrameNo()) && !glSprite->isSelectable())
        {
            painter.setOpacity(glSprite->mSpriteDescriptor.mAlpha * 0.5);
        }
        else
        {
            painter.setOpacity(glSprite->mSpriteDescriptor.mAlpha);
        }
        // render sprite
        glSprite->render(centerPoint, painter);

        // Don't render when playing the animation
        if (!mIsAnimationPlaying)
        {
            if (!glSprite->isSelectable())
            {
                painter.setPen(QColor(100, 100, 200));
                painter.setOpacity(0.5);
            }
            else if (mpSelectedCelModel->getCelDataRef() && glSprite->mID == mpSelectedCelModel->getCelDataRef()->mCelNo)
            {
                painter.setPen(Qt::yellow);
                painter.setOpacity(1.0);
            }
            else
            {
                painter.setPen(Qt::white);
                painter.setOpacity(0.5);
            }

            // Draw image border rectanble
            // Calculate draw position and draw
            if(mpAnimationModel->isKeyFrameSelected())
            {
                QRect rect = glSprite->getRect();
                rect.translate(centerPoint);
                painter.drawRect(rect);

                // Draw Text
                painter.drawText(QRect(
                                        (int)glSprite->mSpriteDescriptor.mPosition.mX + centerPoint.x(),
                                        (int)glSprite->mSpriteDescriptor.mPosition.mY + centerPoint.y(),
                                        16,
                                        16
                                       ),
                                 Qt::AlignCenter, QString("%0").arg(glSprite->mID));
            }
        }

        iter++;
    }
    painter.end();
}

void AnimationViewerPanel::clearSprites()
{
    while (!mGlSpriteList.empty())
    {
        delete mGlSpriteList.takeFirst();
    }
}

void AnimationViewerPanel::addCelSprite(const CelModel::CelData& celData)
{
    mGlSpriteList.push_front(
            new GLSprite(
                    celData.mCelNo,
                    mpAnimationModel->getPixmap(celData.mTextureID),
                    celData.mSpriteDescriptor,
                    !celData.mIsTweenCel
            )
    );
}

void AnimationViewerPanel::removeCelSprite(const CelModel::CelData& celData)
{

}

void AnimationViewerPanel::refresh()
{
    clearSprites();
    QHash<int, CelModel::CelData> celHash = mpAnimationModel->getCelHashAt(mpAnimationModel->getCurrentFrameNo());
    QHash<int, CelModel::CelData>::Iterator iter = celHash.begin();
    while (iter != celHash.end())
    {
        CelModel::CelData& celData = iter.value();
        addCelSprite(celData);
        iter++;
    }

    repaint();
}

/* -------------------------------------------------------------------

 Cel list change event

---------------------------------------------------------------------*/
void AnimationViewerPanel::onCurrentFrameNoChanged(int frameNo)
{
    unselectCels();

    refresh();
}

void AnimationViewerPanel::onCelAdded(CelModel::CelData celData)
{
    addCelSprite(celData);
    repaint();
}

void AnimationViewerPanel::addNewCel(QPoint& relativePressedPosition)
{
    QPixmap* pixmap = mpAnimationModel->getPixmap(mpAnimationModel->getSelectedPaletNo());
    if (pixmap != NULL)
    {
        GLSprite::Point2 position;
        position.mX = relativePressedPosition.x();
        position.mY = relativePressedPosition.y();
        mpAnimationModel->addCelData(mpAnimationModel->getCurrentKeyFrameNo(), position);
    }
}

void AnimationViewerPanel::selectCel(int celNo)
{
    mpSelectedCelModel->setCelDataRef(mpAnimationModel->getCelDataRef(mpAnimationModel->getCurrentKeyFrameNo(), celNo));
    if (mpSelectedCelModel->getCelDataRef())
    {
        emit celSelected(mpSelectedCelModel->getCelDataRef());
    }
    repaint();
}

void AnimationViewerPanel::pickCel(QPoint& relativePressedPosition)
{
    // Select a cel, if mouse has clicked on it
    QList<GLSprite*>::Iterator iter = mGlSpriteList.begin();
    while (iter != mGlSpriteList.end())
    {
        GLSprite* glSprite = (GLSprite*)*iter;
        if (glSprite->isSelectable() && glSprite->contains(relativePressedPosition))
        {
            mSelectedOffset = glSprite->getRect().topLeft() - relativePressedPosition;
            selectCel(glSprite->mID);
            break;
        }
        iter++;
    }
}

void AnimationViewerPanel::mousePressEvent(QMouseEvent *event)
{
    if(mpAnimationModel->isKeyFrameSelected())
    {
        unselectCels();
        this->setFocus();

        // Get center point here
        QPoint centerPoint = getCenterPoint();
        // Calculate pressed position relative from center
        QPoint relativePressedPosition = QPoint(event->x(), event->y()) - centerPoint;

        pickCel(relativePressedPosition);

        // If no cel is selected, it will add new cel to the key frame
        if (!mpSelectedCelModel->getCelDataRef())
        {
            addNewCel(relativePressedPosition);
        }
    }
}

void AnimationViewerPanel::mouseMoveEvent(QMouseEvent *event)
{
    // Move cel if it is selected
    if (mpSelectedCelModel->getCelDataRef())
    {
        QPoint centerPoint = getCenterPoint();
        if (mpSelectedCelModel->getCelDataRef()->mCelNo >= 0)
        {
            mpSelectedCelModel->setPositionX(event->x() - centerPoint.x() + mSelectedOffset.x());
            mpSelectedCelModel->setPositionY(event->y() - centerPoint.y() + mSelectedOffset.y());
            refresh();
        }
    }
}

void AnimationViewerPanel::mouseReleaseEvent(QMouseEvent *event)
{
    repaint();
}

void AnimationViewerPanel::timerEvent()
{
 
}
