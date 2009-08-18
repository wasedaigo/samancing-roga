#ifndef ANIMATIONIMAGEPALETPANEL_H
#define ANIMATIONIMAGEPALETPANEL_H

#include <QWidget>
#include <QPixmap>

class AnimationModel;
class GLSprite;
class QPaintEvent;
class QPixmap;
class AnimationImagePaletPanel : public QWidget
{
    Q_OBJECT

public:
    AnimationImagePaletPanel(int paletNo, AnimationModel* pAnimationModel);
    
    void setSnapGrid(int gridX, int gridY, bool snapGridCheck);
    
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void onAnimationImagePaletChanged(int paletNo, QString id);

private:
    QPoint getSnappedPosition(int x, int y);

    QRect mSelectedRect;
    AnimationModel* mpAnimationModel;
    QColor mClearColor;
    bool mPressed;
    int mSnapGridX;
    int mSnapGridY;
    bool mSnapGridCheck;
    int mPaletNo;

};

#endif // ANIMATIONIMAGEPALETPANEL_H
