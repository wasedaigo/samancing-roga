#ifndef ANIMATIONIMAGEPALETPANEL_H
#define ANIMATIONIMAGEPALETPANEL_H

#include <QWidget>
#include <QPixmap>
#include <QList>

class AnimationModel;
class GLSprite;
class QPaintEvent;
class QPixmap;
class QTimer;
class AnimationImagePaletPanel : public QWidget
{
    Q_OBJECT

public:

    enum CanvasType
    {
        CanvasType_None,
        CanvasType_Image,
        CanvasType_Animation
    };

    AnimationImagePaletPanel(AnimationModel* pAnimationModel);
    ~AnimationImagePaletPanel();

    void setSnapGrid(int gridX, int gridY, bool snapGridCheck);
    
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent  *event);

private slots:
    void onAnimationImagePaletChanged(QString id);
    void onTick();

private:
    QPoint getSnappedPosition(int x, int y);

    QRect mSelectedRect;
    AnimationModel* mpAnimationModel;
    QColor mClearColor;
    bool mPressed;
    int mSnapGridX;
    int mSnapGridY;
    bool mSnapGridCheck;
    CanvasType mCanvasType;

    QList<const GLSprite*> mGlSpriteList;
    AnimationModel* mpPlayingAnimationModel;
    QTimer* mpAnimationPlayTimer;
    int mAnimationFrameNo;
};

#endif // ANIMATIONIMAGEPALETPANEL_H
