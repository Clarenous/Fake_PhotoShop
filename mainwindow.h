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
    bool picture_move = false;//有效拖动过平移图片的滑块
    bool picture_open = false;//打开了新的图片
    bool picture_scale = false;//缩放了图片
    QPixmap *pixmap;//存放当前位图数据
    QPixmap *preview;//存放预览图数据
    int pic_width;//原始图片宽
    int pic_height;//原始图片高
    float amplification = 4;//图片最大放大倍数
    int minification = 80;//图片最小缩小倍数(百分数)
    int winMaxWidth = QApplication::desktop()->width();//最大窗口宽度
    int winMaxHeight = QApplication::desktop()->height();//最大窗口高度
    //缩放初始位置的label_picture信息
    int INIT_width, INIT_height, INIT_x, INIT_y;
    //记录是否进行过亮度、对比度处理
    bool checked_Brightness = false;
    bool checked_Contrast = false;
    bool convertBySpinBox = false;//同步数字输入框与滑动条
};

#endif // MAINWINDOW_H
