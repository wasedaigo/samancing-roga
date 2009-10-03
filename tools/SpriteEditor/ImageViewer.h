#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>

class QPaintEvent;
class QPixmap;
class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    ImageViewer();
    ~ImageViewer();

public slots:
    void setImage(QString filePath);
    void setCenterX(int value);
    void setCenterY(int value);
    void setGridX(int value);
    void setGridY(int value);

signals:
    void centerXChange(int value);
    void centerYChange(int value);
    void gridXChange(int value);
    void gridYChange(int value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QPixmap* mpPixmap;
    int mCenterX;
    int mCenterY;
    int mGridX;
    int mGridY;
};

#endif // IMAGEVIEWER_H
