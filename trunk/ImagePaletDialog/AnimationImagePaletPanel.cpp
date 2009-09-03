#include "AnimationImagePaletpanel.h"
#include "DataModels/AnimationModel.h"
#include "ResourceManager.h"
#include "GLSprite.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>

AnimationImagePaletPanel::AnimationImagePaletPanel(AnimationModel* pAnimationModel)
        : mpAnimationModel(pAnimationModel),
          mClearColor(Qt::black),
          mPressed(false),
          mSnapGridX(0),
          mSnapGridY(0),
          mSnapGridCheck(false),
          mpPlayingAnimationModel(NULL),
          mAnimationFrameNo(0)
{
    mSelectedRect = QRect(0, 0, 96, 96);
    this->setFixedSize(320, 240);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    mpAnimationPlayTimer = new QTimer(this);
    mpAnimationPlayTimer->setInterval(30);
    connect(mpAnimationPlayTimer, SIGNAL(timeout()), this, SLOT(onTick()));
    connect(mpAnimationModel, SIGNAL(selectedPaletChanged(QString)), this, SLOT(onAnimationImagePaletChanged(QString)));
}

AnimationImagePaletPanel::~AnimationImagePaletPanel()
{
    while (!mGlSpriteList.empty()) { delete mGlSpriteList.takeFirst(); }
    delete mpPlayingAnimationModel;
    delete mpAnimationPlayTimer;
}

void AnimationImagePaletPanel::setSnapGrid(int gridX, int gridY, bool snapGridCheck)
{
    mSnapGridX = gridX;
    mSnapGridY = gridY;
    mSnapGridCheck = snapGridCheck;
}

void AnimationImagePaletPanel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    painter.fillRect(QRect(0, 0, width() - 1, height() - 1), Qt::SolidPattern);

    switch(mCanvasType)
    {
        case CanvasType_Image:
            const QPixmap* pPixmap = AnimationModel::getPixmap(mpAnimationModel->getSelectedSourcePath());
            if (pPixmap)
            {
                painter.drawPixmap(0, 0, *pPixmap);
            }
            painter.setPen(Qt::white);
            painter.drawRect(mSelectedRect);

            painter.setPen(palette().dark().color());
            painter.setBrush(Qt::NoBrush);
            painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
            break;

        case CanvasType_Animation:
            {
                QPoint centerPoint = QPoint((width()) / 2, (height()) / 2);
                QList<GLSprite*>::Iterator iter = mGlSpriteList.begin();
                while (iter != mGlSpriteList.end())
                {
                    GLSprite* glSprite = (GLSprite*)*iter;

                    painter.translate(centerPoint.x(), centerPoint.y());
                    glSprite->render(QPoint(0, 0), painter, NULL, mpAnimationModel->getTargetSprite());
                    painter.translate(-centerPoint.x(), -centerPoint.y());

                    iter++;
                }
            }
            break;
        default:
            break;
    }

    painter.end();
}

void AnimationImagePaletPanel::onAnimationImagePaletChanged(QString path)
{
    delete mpPlayingAnimationModel;
    switch(ResourceManager::getFileType(path))
    {
        case ResourceManager::FileType_Image:
            {
                const QPixmap* pPixmap = AnimationModel::getPixmap(path);
                setFixedSize(pPixmap->width(), pPixmap->height());

                mCanvasType = CanvasType_Image;
                this->repaint();
            }
            break;

        case ResourceManager::FileType_Animation:
            {
                mCanvasType = CanvasType_Animation;
                mpPlayingAnimationModel = new AnimationModel(this);
                mpPlayingAnimationModel->setRenderTarget(this);
                mpPlayingAnimationModel->loadData(ResourceManager::getResourcePath(path));

                mAnimationFrameNo = 0;
                mpAnimationPlayTimer->start();
            }
            break;

        default:
            mCanvasType = CanvasType_None;
            break;
    }
}

QPoint AnimationImagePaletPanel::getSnappedPosition(int x, int y)
{
    int tx = x;
    int ty = y;
    if (mSnapGridCheck)
    {        
        tx = (x/ mSnapGridX) * mSnapGridX;
        ty = (y/ mSnapGridY) * mSnapGridY;
    }

    return QPoint(tx, ty);
}

void AnimationImagePaletPanel::mousePressEvent(QMouseEvent *event)
{
   if (mCanvasType == CanvasType_Image)
   {
        mPressed = true;
        QPoint pos = getSnappedPosition(event->x(), event->y());

        mSelectedRect.setTopLeft(pos);
        if (mSnapGridCheck)
        {
            pos += QPoint(mSnapGridX, mSnapGridY);
        }
        mSelectedRect.setBottomRight(pos);
        repaint();
    }
}

void AnimationImagePaletPanel::mouseMoveEvent(QMouseEvent *event)
{
   if (mCanvasType == CanvasType_Image)
   {
        if (mPressed)
        {
            QPoint pos = getSnappedPosition(event->x(), event->y());
            if (mSnapGridCheck)
            {
                if (pos.x() > 0)
                {
                    pos.setX(pos.x() + mSnapGridX);
                }
                if (pos.y() > 0)
                {
                    pos.setY(pos.y() + mSnapGridY);
                }

                QPoint dPoint = pos - mSelectedRect.topLeft();
                if (dPoint.x() == 0)
                {
                    int t = event->x() - mSelectedRect.topLeft().x();
                    pos.setX(pos.x() + (t >= 0 ? mSnapGridX : -mSnapGridX));
                }
                if (dPoint.y() == 0)
                {
                    int t = event->y() - mSelectedRect.topLeft().y();
                    pos.setY(pos.y() + (t >= 0 ? mSnapGridY : -mSnapGridY));
                }
            }
            mSelectedRect.setBottomRight(pos);
            repaint();
        }
    }
}

void AnimationImagePaletPanel::mouseReleaseEvent(QMouseEvent *event)
{
    if (mCanvasType == CanvasType_Image)
    {
        mpAnimationModel->mSelectedPaletTextureSrcRect.mX = mSelectedRect.x();
        mpAnimationModel->mSelectedPaletTextureSrcRect.mY = mSelectedRect.y();
        mpAnimationModel->mSelectedPaletTextureSrcRect.mWidth = mSelectedRect.width();
        mpAnimationModel->mSelectedPaletTextureSrcRect.mHeight = mSelectedRect.height();
        mPressed = false;
    }
}

void AnimationImagePaletPanel::onTick()
{
    // Don't play an animation without any frames
    if (mpPlayingAnimationModel->getMaxFrameCount() > 0)
    {
        // goto next frame
        mAnimationFrameNo++;

        // Loop animation
        if(mAnimationFrameNo >= mpPlayingAnimationModel->getMaxFrameCount())
        {
            mAnimationFrameNo = 0;
        }

        // Delete previous generated sprites
        while (!mGlSpriteList.empty()) { delete mGlSpriteList.takeFirst(); }

        // Set current glsprite list
        mGlSpriteList = mpPlayingAnimationModel->createGLSpriteListAt(mAnimationFrameNo);

        repaint();
    }
}
