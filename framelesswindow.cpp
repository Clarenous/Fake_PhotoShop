#include "FramelessWindow.h"
#include <QMouseEvent>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>

FramelessWindow::FramelessWindow(QWidget *contentWidget, QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);    // ȥ���߿�
    setAttribute(Qt::WA_TranslucentBackground); // ����͸��
    d = new FramelessWindowPrivate(contentWidget);
    // �����Ӱ
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(contentWidget);
    shadowEffect->setColor(Qt::lightGray);
    shadowEffect->setBlurRadius(4); // ��Ӱ�Ĵ�С
    shadowEffect->setOffset(0, 0);
    contentWidget->setGraphicsEffect(shadowEffect);
    // ��ӵ�������
    QGridLayout *lo = new QGridLayout();
    lo->addWidget(contentWidget, 0, 0);
    lo->setContentsMargins(4, 4, 4, 4); // ע�����Ӱ��С��Э��
    setLayout(lo);
}
FramelessWindow::~FramelessWindow() {
    delete d;
}
void FramelessWindow::mousePressEvent(QMouseEvent *e) {
    // ��¼��갴��ʱȫ�ֵ�λ�úʹ������Ͻǵ�λ��
    d->mousePressedPosition = e->globalPos();
    d->windowPositionAsDrag = pos();
}
void FramelessWindow::mouseReleaseEvent(QMouseEvent *e) {
    Q_UNUSED(e)
    // ���ſ�ʼ������갴�µ�λ��Ϊ null����ʾ���û�б�����
    d->mousePressedPosition = QPoint();
}
void FramelessWindow::mouseMoveEvent(QMouseEvent *e) {
    if (!d->mousePressedPosition.isNull()) {
        // ��갴�²����ƶ�ʱ���ƶ�����, �������갴��ʱ��λ�ü��㣬��Ϊ�˷�ֹ����ۻ�
        QPoint delta = e->globalPos() - d->mousePressedPosition;
        move(d->windowPositionAsDrag + delta);
    }
}
