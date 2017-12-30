#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H
#include <QWidget>
struct FramelessWindowPrivate {
    FramelessWindowPrivate(QWidget *contentWidget) : contentWidget(contentWidget) {}
    QWidget *contentWidget;
    QPoint mousePressedPosition; // 鼠标按下时的坐标
    QPoint windowPositionAsDrag; // 鼠标按小时窗口左上角的坐标
};

class FramelessWindow : public QWidget {
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *contentWidget, QWidget *parent = 0);
    ~FramelessWindow();
protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
private:
    FramelessWindowPrivate *d;
};
#endif // FRAMELESSWINDOW_H
