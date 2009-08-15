#include "AnimationImagePaletpanel.h"
#include "DataModels/AnimationModel.h"
#include "GLSprite.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

AnimationImagePaletPanel::AnimationImagePaletPanel(AnimationModel* pAnimationModel)
        : mpAnimationModel(pAnimationModel),
          mClearColor(Qt::black),
          mpPixmap(NULL),
          mPressed(false),
          mSnapGridX(0),
          mSnapGridY(0)
{
    mSelectedRect = QRect(0, 0, 96, 96);
    this->setFixedSize(320, 240);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

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

    if (mpPixmap)
    {
        painter.drawPixmap(0, 0, *mpPixmap);
    }
    painter.setPen(Qt::white);
    painter.drawRect(mSelectedRect);

    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}

void AnimationImagePaletPanel::setPixmap(QPixmap* pixmap)
{
    setFixedSize(pixmap->width(), pixmap->height());
    mpPixmap = pixmap;
    this->repaint();
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

void AnimationImagePaletPanel::mouseMoveEvent(QMouseEvent *event)
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

void AnimationImagePaletPanel::mouseReleaseEvent(QMouseEvent *event)
{
    mpAnimationModel->mSelectedPaletTextureSrcRect.mX = mSelectedRect.x();
    mpAnimationModel->mSelectedPaletTextureSrcRect.mY = mSelectedRect.y();
    mpAnimationModel->mSelectedPaletTextureSrcRect.mWidth = mSelectedRect.width();
    mpAnimationModel->mSelectedPaletTextureSrcRect.mHeight = mSelectedRect.height();
    mPressed = false;
}
