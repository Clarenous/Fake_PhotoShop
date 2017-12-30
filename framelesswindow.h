#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H
#include <QWidget>
struct FramelessWindowPrivate {
    FramelessWindowPrivate(QWidget *contentWidget) : contentWidget(contentWidget) {}
    QWidget *contentWidget;
    QPoint mousePressedPosition; // ��갴��ʱ������
    QPoint windowPositionAsDrag; // ��갴Сʱ�������Ͻǵ�����
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
