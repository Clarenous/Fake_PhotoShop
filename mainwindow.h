#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDesktopWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private slots:


    void on_action_File_Open_triggered();

    void on_scalePicture_valueChanged(int value);

    void on_HMovePicture_valueChanged(int value);

    void on_VMovePicture_valueChanged(int value);

    void on_action_Win_content_fix_triggered();

    void on_action_Win_window_fix_triggered();

    void on_action_File_Save_triggered();

    void on_action_Edit_GrayProcess24_triggered();

    void on_action_Edit_Smooth4_triggered();

    void on_action_Edit_Smooth8_triggered();

    void on_action_Edit_FlipHorizontal_triggered();

    void on_action_Edit_FlipVertical_triggered();

    void on_action_Edit_Revert_triggered();

    void on_action_Edit_ConvertToTxt_triggered();

    void on_action_Edit_Brightness_triggered(bool checked);

    void on_Slider_Brightness_valueChanged(int value);

    void on_doubleSpinBox_Brightness_valueChanged(double arg1);

    void on_apply_Brightness_pressed();

    void on_close_Brightness_pressed();

    void on_action_Edit_Contrast_triggered(bool checked);

    void on_Slider_Contrast_valueChanged(int value);

    void on_doubleSpinBox_Contrast_valueChanged(double arg1);

    void on_apply_Contrast_pressed();

    void on_close_Contrast_pressed();

    void on_Slider_Monochrome_valueChanged(int value);

    void on_spinBox_Monochrome_valueChanged(int arg1);

    void on_apply_Monochrome_pressed();

    void on_close_Monochrome_pressed();

    void on_action_Edit_Convert8toMonochrome_triggered(bool checked);

    void on_action_Edit_InverseColor_triggered();

    void on_action_Edit_Convert24to256_triggered();

    void on_action_File_Quit_triggered();

    void on_action_Help_About_triggered();

    //void on_action_File_UnZip_triggered();

    //void on_action_File_Zip_triggered();

private:
    Ui::MainWindow *ui;
    bool picture_move = false;//��Ч�϶���ƽ��ͼƬ�Ļ���
    bool picture_open = false;//�����µ�ͼƬ
    bool picture_scale = false;//������ͼƬ
    QPixmap *pixmap;//��ŵ�ǰλͼ����
    QPixmap *preview;//���Ԥ��ͼ����
    int pic_width;//ԭʼͼƬ��
    int pic_height;//ԭʼͼƬ��
    float amplification = 4;//ͼƬ���Ŵ���
    int minification = 80;//ͼƬ��С��С����(�ٷ���)
    int winMaxWidth = QApplication::desktop()->width();//��󴰿ڿ��
    int winMaxHeight = QApplication::desktop()->height();//��󴰿ڸ߶�
    //���ų�ʼλ�õ�label_picture��Ϣ
    int INIT_width, INIT_height, INIT_x, INIT_y;
    //��¼�Ƿ���й����ȡ��Աȶȴ���
    bool checked_Brightness = false;
    bool checked_Contrast = false;
    bool convertBySpinBox = false;//ͬ������������뻬����
};

#endif // MAINWINDOW_H
