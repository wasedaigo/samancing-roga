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
public:
    void setPixmap(QPixmap* pixmap);
    AnimationImagePaletPanel(AnimationModel* pAnimationModel);
    
    void setSnapGrid(int gridX, int gridY, bool snapGridCheck);
    
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


private:
    QPoint getSnappedPosition(int x, int y);

    QRect mSelectedRect;
    AnimationModel* mpAnimationModel;
    QColor mClearColor;
    QPixmap* mpPixmap;
    bool mPressed;
    int mSnapGridX;
    int mSnapGridY;
    bool mSnapGridCheck;
};

#endif // ANIMATIONIMAGEPALETPANEL_H
