#include "AnimationViewerPanel.h"
#include "DataModels/AnimationModel.h"
#include "DataModels/CelModel.h"
#include "DataModels/KeyFrame.h"
#include "DataModels/KeyFrameData.h"
#include "ResourceManager.h"
#include "GLSprite.h"
#include <QMouseEvent>
#include <QPixmap>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <QMessageBox>

#define TARGET_SCREEN_WIDTH 320
#define TARGET_SCREEN_HEIGHT 240

AnimationViewerPanel::AnimationViewerPanel(QWidget* parent, AnimationModel* pAnimationModel, CelModel* const pSelectedCelModel)
        : QWidget(parent),
          mpAnimationModel(pAnimationModel),
          mCurrentFrameNo(),
          mClearColor(Qt::black),
          mSelectedOffset(QPoint(0, 0)),
          mpSelectedCelModel(pSelectedCelModel),
          mIsAnimationPlaying(false),
          mCelGrabbed(false),
          mTargetGrabbed(false)
{
    setAutoFillBackground(false);

    connect(mpAnimationModel, SIGNAL(refreshTimeLine()), this, SLOT(refresh()));
    connect(mpAnimationModel, SIGNAL(selectedKeyFramePositionChanged(int, int)), this, SLOT(refresh()));
    connect(mpAnimationModel, SIGNAL(targetPositionMoved(int, int)), this, SLOT(refresh()));
}

AnimationViewerPanel::~AnimationViewerPanel()
{

}

bool AnimationViewerPanel::isAnimationPlaying() const
{
    return mIsAnimationPlaying;
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
    const KeyFrame::KeyFramePosition& keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();

    int frameNo =  keyframePosition.mFrameNo;
    if (frameNo < mpAnimationModel->getMaxFrameCount())
    {
       mpAnimationModel->selectCurrentKeyFramePosition(keyframePosition.mLineNo, keyframePosition.mFrameNo + 1);
    }
}

void AnimationViewerPanel::resizeEvent(QResizeEvent *event)
{
}

QPoint AnimationViewerPanel::getCenterPoint() const
{
    return QPoint(
        (width()) / 2,
        (height()) / 2
    );
}

void AnimationViewerPanel::keyPressEvent (QKeyEvent* e)
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    switch(e->key())
    {
        // Delete selected cel
        case Qt::Key_Delete:
            mpAnimationModel->clearFrames(currentPosition.mLineNo, currentPosition.mFrameNo, currentPosition.mFrameNo);
        break;
    }
}

void AnimationViewerPanel::keyReleaseEvent (QKeyEvent* e)
{
    switch(e->key())
    {
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

    renderCross(painter);
    renderCelSprites(centerPoint, painter);
    renderTargetSprite(centerPoint, painter);

    painter.end();
}

void AnimationViewerPanel::renderCross(QPainter& painter)
{
    painter.setPen(QColor(120, 150, 200));
    painter.drawLine(QPoint(0, height() / 2), QPoint(width(), height() / 2));
    painter.drawLine(QPoint(width() / 2, 0), QPoint(width() / 2, height()));
}

void AnimationViewerPanel::renderCelSprites(const QPoint& centerPoint, QPainter& painter)
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();

    QList<const GLSprite*>::Iterator iter = mGlSpriteList.begin();
    while (iter != mGlSpriteList.end())
    {
        const GLSprite* glSprite = (GLSprite*)*iter;

        // render sprite
        painter.translate(centerPoint.x(), centerPoint.y());
        glSprite->render(QPoint(0, 0), painter,AnimationModel::getTargetSprite());
        painter.translate(-centerPoint.x(), -centerPoint.y());

        QPoint spriteRenderPoint = centerPoint - glSprite->mSpriteDescriptor.center();

        // Don't render when playing the animation
        if (!mIsAnimationPlaying)
        {
            if (!glSprite->isSelectable())
            {
                // unselectable cel
                painter.setPen(QColor(100, 100, 200));
                painter.setOpacity(0.5);
            }
            else if (glSprite->mID == currentPosition.mLineNo)
            {
                renderCenterPointSprite(glSprite, spriteRenderPoint, painter);
                // Cel selecte color
                painter.setPen(Qt::yellow);
                painter.setOpacity(1.0);
            }
            else
            {
                // unselected cel
                painter.setPen(Qt::white);
                painter.setOpacity(0.5);
            }

            // Draw image border rectanble
            // Calculate draw position and draw
            QRect rect = glSprite->getRect();
            rect.translate(spriteRenderPoint);
            painter.drawRect(rect);

            // Draw Text
            painter.drawText(QRect(
                                    spriteRenderPoint.x() + (int)glSprite->mSpriteDescriptor.mPosition.mX,
                                    spriteRenderPoint.y() + (int)glSprite->mSpriteDescriptor.mPosition.mY,
                                    16,
                                    16
                                   ),
                             Qt::AlignCenter, QString("%0").arg(glSprite->mID + 1));
            }

        iter++;
    }
}

void AnimationViewerPanel::renderCenterPointSprite(const GLSprite* pGlSprite, const QPoint& centerPoint, QPainter& painter)
{
    if (pGlSprite->mSpriteDescriptor.isImage())
    {
        // render center point
        GLSprite* centerPointSprite = AnimationModel::getCenterPointSprite();
        QPointF offset = QPointF(
                (pGlSprite->mSpriteDescriptor.mPosition.mX + pGlSprite->mSpriteDescriptor.center().x()),
                (pGlSprite->mSpriteDescriptor.mPosition.mY + pGlSprite->mSpriteDescriptor.center().y())
        );

        QPoint centerPointCenterPoint = centerPoint + offset.toPoint();

        painter.translate(centerPointCenterPoint.x(), centerPointCenterPoint.y());
        centerPointSprite->render(QPoint(0, 0), painter, AnimationModel::getTargetSprite());
        painter.translate(-centerPointCenterPoint.x(), -centerPointCenterPoint.y());
    }
}

void AnimationViewerPanel::renderTargetSprite(const QPoint& centerPoint, QPainter& painter)
{
    painter.setOpacity(mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mAlpha);

    painter.translate(centerPoint.x(), centerPoint.y());
    mpAnimationModel->getTargetSprite()->render(QPoint(0, 0), painter, NULL);
    painter.translate(-centerPoint.x(), -centerPoint.y());
}

void AnimationViewerPanel::clearSprites()
{
    while (!mGlSpriteList.empty())
    {
        delete mGlSpriteList.takeFirst();
    }
}

void AnimationViewerPanel::refresh()
{
    clearSprites();

    if (!isAnimationPlaying())
    {
        float targetX = mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mX;
        float targetY = mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mY;

        QPoint centerPoint = getCenterPoint();

        if (targetX < -centerPoint.x()){targetX = -centerPoint.x();}
        if (targetX > centerPoint.x()){targetX = centerPoint.x();}
        if (targetY < -centerPoint.y()){targetY = -centerPoint.y();}
        if (targetY > centerPoint.y()){targetY = centerPoint.y();}

        mpAnimationModel->setTargetSpritePosition(targetX, targetY);

        // set cel reference
        KeyFrame::KeyFramePosition keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
        KeyFrame* pKeyframe = mpAnimationModel->getKeyFrame(keyframePosition.mLineNo, keyframePosition.mFrameNo);

        if (pKeyframe)
        {
            mpSelectedCelModel->setKeyFrameDataReference(pKeyframe->mpKeyFrameData);
            emit celSelected(pKeyframe->mpKeyFrameData);
        }
        else
        {
            mpSelectedCelModel->setKeyFrameDataReference(NULL);
            emit celSelected(NULL);
        }
    }
    mGlSpriteList = mpAnimationModel->createGLSpriteListAt(NULL, mpAnimationModel->getCurrentKeyFramePosition().mFrameNo);

    repaint();
}

/* -------------------------------------------------------------------

 Cel list change event

---------------------------------------------------------------------*/
void AnimationViewerPanel::addNewCel(QPoint& relativePressedPosition)
{
    if (mpAnimationModel->getSelectedSourcePath() != "")
    {
        GLSprite::Point2 position;
        position.mX = relativePressedPosition.x();
        position.mY = relativePressedPosition.y();

        KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
        mpAnimationModel->setKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo, position);
    }
}

void AnimationViewerPanel::selectCel(int lineNo)
{
    KeyFrame::KeyFramePosition keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
    if (keyframePosition.mLineNo != lineNo)
    {
        mpAnimationModel->selectCurrentKeyFramePosition(lineNo, keyframePosition.mFrameNo);
    }
}

void AnimationViewerPanel::pickCel(QPoint& relativePressedPosition)
{

    QPointF pt2 = QPointF(mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mX, mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mY);
    if(mpAnimationModel->getTargetSprite()->contains(
            relativePressedPosition
       )
    )
    {
        mSelectedOffset = mpAnimationModel->getTargetSprite()->getRect().topLeft() - relativePressedPosition;
        mTargetGrabbed = true;
    }
    else
    {
        // Select a cel, if mouse has clicked on it
        for (int i = mGlSpriteList.count() - 1; i >= 0; i--)
        {
            const GLSprite* glSprite = mGlSpriteList[i];
            if (glSprite->isSelectable() &&
                glSprite->contains(
                        relativePressedPosition                   )
                )
            {
                mSelectedOffset = glSprite->getRect().topLeft() - relativePressedPosition;
                selectCel(glSprite->mID);
                mCelGrabbed = true;
                break;
            }
        }
    }
}

void AnimationViewerPanel::mousePressEvent(QMouseEvent *event)
{
    mCelGrabbed = false;
    mTargetGrabbed = false;

    // Don't do anything if it is center point mode
    if (event->modifiers() & Qt::ControlModifier)
    {
        setCenterPoint(event);
        return;
    }

    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();

    this->setFocus();
    // Get center point here
    QPoint centerPoint = getCenterPoint();
    // Calculate pressed position relative from center
    QPoint relativePressedPosition = QPoint(event->x(), event->y()) - centerPoint;

    pickCel(relativePressedPosition);

    if(!mCelGrabbed && !mTargetGrabbed)
    {
        QString path1 = mpAnimationModel->getLoadedAnimationPath();
        QString path2 = mpAnimationModel->getSelectedSourcePath();
      if (mpAnimationModel->getLoadedAnimationPath() == mpAnimationModel->getSelectedSourcePath())
      {
          QMessageBox::information(window(), tr("Animation nest error"),
                 tr("You cannot nest the same animation"));
      }
      else
      {
        switch(ResourceManager::getFileType(mpAnimationModel->getSelectedSourcePath()))
        {
            case ResourceManager::FileType_Animation:
            case ResourceManager::FileType_Image:
                if (mpAnimationModel->getKeyFrameIndex(currentPosition.mLineNo, currentPosition.mFrameNo) == -1)
                {
                    GLSprite::Point2 pt;
                    pt.mX = relativePressedPosition.x();
                    pt.mY = relativePressedPosition.y();

                    mpAnimationModel->setKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo, pt);
                }
                else
                {
                    if (event->modifiers() & Qt::ShiftModifier)
                    {
                        swapSourceTexture();
                    }
                }
            break;

            default:
            break;
        }
      }
    }
}

// When the user clicked on the canvas with Shift key pressed,
// it causes the cel to swap its texture to currenly selected palet
void AnimationViewerPanel::swapSourceTexture()
{
    KeyFrameData* pKeyFrameData = mpSelectedCelModel->getKeyFrameDataReference();
    if (pKeyFrameData)
    {
        QString path = mpAnimationModel->getSelectedSourcePath();
        if (path != "")
        {
            mpSelectedCelModel->setSourceTexture(path, mpAnimationModel->mSelectedPaletTextureSrcRect);
            refresh();
        }
    }
}

void AnimationViewerPanel::setCenterPoint(QMouseEvent *event)
{
    KeyFrameData* pKeyFrameData = mpSelectedCelModel->getKeyFrameDataReference();
    if (pKeyFrameData && pKeyFrameData->mSpriteDescriptor.isImage())
    {
        QPoint centerPoint = getCenterPoint();
        int centerX = (int)(event->x() - centerPoint.x() - pKeyFrameData->mSpriteDescriptor.mPosition.mX + pKeyFrameData->mSpriteDescriptor.mCenter.mX);
        int centerY = (int)(event->y() - centerPoint.y() - pKeyFrameData->mSpriteDescriptor.mPosition.mY + pKeyFrameData->mSpriteDescriptor.mCenter.mY);

        mpSelectedCelModel->setCenterX(centerX);// / pKeyFrameData->mSpriteDescriptor.textureCenter().x());
        mpSelectedCelModel->setCenterY(centerY);// / pKeyFrameData->mSpriteDescriptor.textureCenter().y());
    }
}

void AnimationViewerPanel::mouseMoveEvent(QMouseEvent *event)
{
    QPoint centerPoint = getCenterPoint();
    int newPosX = event->x() - centerPoint.x() + mSelectedOffset.x();
    int newPosY = event->y() - centerPoint.y() + mSelectedOffset.y();
    KeyFrameData* pKeyFrameData = mpSelectedCelModel->getKeyFrameDataReference();

    if (event->modifiers() & Qt::ControlModifier)
    {
        setCenterPoint(event);
    }
    else if (mCelGrabbed)
    {
        // Move cel if it is selected
        if (pKeyFrameData)
        {
            if (pKeyFrameData->mSpriteDescriptor.mRelativeToTarget)
            {
                newPosX -= (int)mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mX;
                newPosY -= (int)mpAnimationModel->getTargetSprite()->mSpriteDescriptor.mPosition.mY;
            }
            mpSelectedCelModel->setPositionX(newPosX);
            mpSelectedCelModel->setPositionY(newPosY);
        }
    }
    else if (mTargetGrabbed)
    {
        mpAnimationModel->setTargetSpritePosition(newPosX, newPosY);
        refresh();
    }
}

void AnimationViewerPanel::mouseReleaseEvent(QMouseEvent *event)
{
    mCelGrabbed = false;
    mTargetGrabbed = false;
}

void AnimationViewerPanel::timerEvent()
{
 
}
