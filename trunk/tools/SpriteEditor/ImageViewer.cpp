#include "ImageViewer.h"

#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

static QPixmap* spCenterPointPixmap = NULL;

ImageViewer::ImageViewer()
        :mpPixmap(NULL),
         mCenterX(0),
         mCenterY(0),
         mGridX(0),
         mGridY(0)
{
    spCenterPointPixmap = new QPixmap(":/resource/center_point.png");
}

ImageViewer::~ImageViewer()
{
    delete mpPixmap;
}

// Slots
void ImageViewer::setImage(QString filePath)
{
    delete mpPixmap;
    mpPixmap = new QPixmap(filePath);
    this->setFixedSize(mpPixmap->width(), mpPixmap->height());

    repaint();
}

void ImageViewer::setCenterX(int value)
{
    mCenterX = value;
    repaint();
    emit centerXChange(mCenterX);
}

void ImageViewer::setCenterY(int value)
{
    mCenterY = value;
    repaint();
    emit centerYChange(mCenterY);
}

void ImageViewer::setGridX(int value)
{
    mGridX = value;
    repaint();
    emit gridXChange(mGridX);
}

void ImageViewer::setGridY(int value)
{
    mGridY = value;
    repaint();
    emit gridYChange(mGridY);
}

// Event
void ImageViewer::paintEvent(QPaintEvent *event)
{
    if (mpPixmap && mGridX > 0 && mGridY > 0)
    {
        QPainter painter(this);
        painter.setPen(Qt::black);
        painter.setBrush(Qt::NoBrush);
        painter.fillRect(QRect(0, 0, width() - 1, height() - 1), Qt::SolidPattern);

        painter.drawPixmap(0, 0, *mpPixmap);

        painter.setPen(Qt::white);
        int xCount = width() / mGridX + 1;
        for (int i = 0; i < xCount; i++)
        {
            painter.drawLine(QPointF(mGridX * i, 0), QPointF(mGridX * i, height()));
        }

        int yCount = height() / mGridY + 1;
        for (int i = 0; i < yCount; i++)
        {
            painter.drawLine(QPointF(0, mGridY * i), QPointF(width(), mGridY * i));
        }

        painter.end();
    }
}
