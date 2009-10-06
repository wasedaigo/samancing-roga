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
    void clear();
    void setImage(QString filePath);
    void setCenterX(int value);
    void setCenterY(int value);
    void setSelectedGrid(int x, int y);
    QPoint getSelectedGrid() const;
    QString getLoadedFilePath() const;

    void setGridX(int value);
    void setGridY(int value);

signals:
    void centerXChange(int value);
    void centerYChange(int value);
    void gridXChange(int value);
    void gridYChange(int value);

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QPixmap* mpPixmap;
    int mCenterX;
    int mCenterY;
    int mGridX;
    int mGridY;

    int mX;
    int mY;
    int mNo;
    QString mLoadedFilePath;
};

#endif // IMAGEVIEWER_H
