#include "ImageViewer.h"

#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>


ImageViewer::ImageViewer()
        :mpPixmap(NULL),
         mCenterX(0),
         mCenterY(0),
         mGridX(0),
         mGridY(0),
         mX(0),
         mY(0),
         mNo(0),
         mLoadedFilePath("")
{
}

ImageViewer::~ImageViewer()
{
    delete mpPixmap;
}

// Slots
void ImageViewer::setImage(QString filePath)
{
    clear();
    mLoadedFilePath = filePath;

    mpPixmap = new QPixmap(filePath);
    if (mpPixmap)
    {
        this->setFixedSize(mpPixmap->width(), mpPixmap->height());
        mGridX = mpPixmap->width();
        mGridY = mpPixmap->height();
    }
    repaint();
}

void ImageViewer::clear()
{

    mLoadedFilePath = "";
    delete mpPixmap;
    mpPixmap = NULL;

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

void ImageViewer::setSelectedGrid(int x, int y)
{
    mX = x;
    mY = y;
    repaint();
}

QPoint ImageViewer::getSelectedGrid() const
{
    return QPoint(mX, mY);
}

QString ImageViewer::getLoadedFilePath() const
{
    return mLoadedFilePath;
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

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    this->setFocus();


    mX = event->x() / mGridX;
    mY = event->y() / mGridY;
    repaint();
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    if (mpPixmap )
    {
        if(mGridX > 0 && mGridY > 0)
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

            // Draw selection box
            if (mGridX != width() && mGridY != height())
            {
                painter.setPen(Qt::gray);
                painter.setOpacity(0.5);
                QRectF rect = QRectF(mX * mGridX, mY* mGridY, mGridX, mGridY);
                painter.fillRect(rect, Qt::white);
            }

            painter.end();
        }
    }
    else
    {
        QPainter painter(this);
        painter.fillRect(this->rect(), Qt::gray);
        painter.end();
    }
}
