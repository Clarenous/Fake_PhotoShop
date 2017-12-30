#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QTextStream>
#include <QProcess>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextEdit>
#include <QImage>
#include <QPixmap>
#include <QPainter>
#include <iostream>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_picture->setScaledContents(true);
    pixmap = new QPixmap();
    preview = new QPixmap();
    ui->widget_Brightness->setHidden(true);
    ui->widget_Contrast->setHidden(true);
    ui->widget_Monochrome->setHidden(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //平移滚动条 显示状态
    bool h_visable = false;
    bool v_visable = false;
    if(ui->label_picture->size().width()>ui->widget->size().width())
        h_visable = true;
    if(ui->label_picture->size().height()>ui->widget->size().height())
        v_visable = true;
    ui->HMovePicture->setVisible(h_visable);
    ui->VMovePicture->setVisible(v_visable);

    if(!picture_scale)
    {
        ui->label_picture->resize(ui->label_picture->sizeHint());//自动设置合适的图片大小，不随窗口大小变化
        //ui->label_picture->resize(ui->widget->size());图片尺寸随窗口变化，会宽高比失真
    }

    int w0,w2,h0,h2;//分别表示最佳的图片的宽高、放置图片的widget的宽高
    w0=ui->label_picture->sizeHint().width();
    h0=ui->label_picture->sizeHint().height();
    w2=ui->widget->size().width();
    h2=ui->widget->size().height();

    if((!picture_move) || picture_open)
    {
        int w1,h1;//目前窗口中图片的宽高
        w1=ui->label_picture->size().width();
        h1=ui->label_picture->size().height();
        ui->label_picture->setGeometry((w2-w1)/2,(h2-h1)/2,w1,h1);//将图片放置在中央位置
        picture_open = false;
    }

    INIT_x = (w2-w0)/2;    INIT_y = (h2-h0)/2;
    INIT_width = w0;    INIT_height = h0;
}

void MainWindow::on_action_File_Open_triggered()//Function_打开图片文件
{
    //QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("All File(*.*);;BMP Files(*.BMP);;JPG Files(*.JPG);;JPEG Files(*.JPEG);;PNG Files(*.PNG);;GIF Files(*.GIF)"));
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Images (*.BMP *.JPG *.JPEG *.PNG *.GIF)"));
    if(!path.isEmpty())
    {
        QImage *image = new QImage(path);//载入图片
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))//不编辑只读文件
        {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }

        //QPixmap对象载入内容
        pixmap -> convertFromImage(*image);//将图片写入QPixmap 对象

        if(pixmap->save("original.bmp"))
            std::cout<<"good!"<<std::endl;
        //外部exe处理图像
        QProcess process(this);
        process.start("cmd_copy.exe");
        //process.write ("ReWrite.exe original.bmp tmp.dat\n\r");
        process.write ("outsideEXE_FPS.exe 1 original.bmp tmp.dat\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        //显示图片
        pixmap->load("tmp.dat");
        ui -> label_picture -> setPixmap(*pixmap);
        file.close();

        //补充类中私有成员的信息
        int MemoryLimit = 0xffffff; //限制图片占用的内存
        pic_width = image->width();     pic_height = image->height();
        //计算放大倍数
        amplification = 4;
        if(pic_width*pic_height>MemoryLimit)
            amplification = 1;
        else
            if(MemoryLimit/(pic_width*pic_height)<16)
                amplification = sqrt((int)(MemoryLimit/(pic_width*pic_height)));
        //计算缩小倍数(百分数),必须在载入图片之后，否则sizeHint()得不到正确结果
        int w_temp, h_temp;
        minification = 80;
        if(pic_width>winMaxWidth||pic_height>winMaxHeight)
        {
            if(pic_width/winMaxWidth>pic_height/winMaxHeight)
            {w_temp = winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
                else
                {h_temp = winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
            //得到缩小系数
            minification = (w_temp*80)/ui->label_picture->sizeHint().width();
        }

        int w0,w2,h0,h2;//分别表示最佳的图片的宽高、放置图片的widget的宽高
        w0=ui->label_picture->sizeHint().width();
        h0=ui->label_picture->sizeHint().height();
        w2=ui->widget->size().width();
        h2=ui->widget->size().height();
        INIT_x = (w2-w0)/2;    INIT_y = (h2-h0)/2;
        INIT_width = w0;    INIT_height = h0;

        picture_move = false;//防止主窗口自动调整图片位置
        picture_scale = false;//防止主窗口自动调整图片位置
        picture_open = true;//防止主窗口自动调整图片位置
        checked_Brightness = false;
        checked_Contrast = false;
    }
    else
    {
        //QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}

void MainWindow::on_scalePicture_valueChanged(int value)//拖动水平滑块实现缩放画面
{
    int INIT_Position = 40;//滑块初始位置，需要与ui界面保持一致
    int w0,w1,w2,h0,h1,h2;    //分别表示最佳的图片宽高、目前窗口中图片的宽高、以及放置图片的widget的宽高
    w0=ui->label_picture->sizeHint().width();
    h0=ui->label_picture->sizeHint().height();
    w1=ui->label_picture->size().width();
    h1=ui->label_picture->size().height();
    w2=ui->widget->size().width();
    h2=ui->widget->size().height();
    int w,h;    //缩放变化后的宽和高
    float k = 1;    //缩放比例系数
    //图片在缩小和放大时的策略不同
    if(value < INIT_Position)//缩小图片,最小缩小至minification%大小
        k = (float)(minification + ((float)(100-minification)/INIT_Position)*value)/100;
    if(value > INIT_Position)//放大图片，最大放大至amplification倍大小
        k = (1 + (float)(amplification-1)*(value - INIT_Position)/(100-INIT_Position));
    w = k*w0;   h = k*h0;
    //图片绘制原点(x,y)
    int x,y;
    x = ui->label_picture->pos().rx();    y = ui->label_picture->pos().ry();
    //误差量error
    int error_x = x - (w2-w1)/2;    int error_y = y - (h2 - h1)/2;
    //经过反馈修正后的新图片绘制原点(x0,y0)
    int x0 = (w2-w1)/2; int y0 = (h2 - h1)/2;
    //放大时需要修正绘制原点(x0,y0)
    if(value > INIT_Position)
    {
        x0 = (k/amplification-1)*(float)error_x + (w2-w1)/2;     y0 = (k/amplification-1)*(float)error_y + (h2 - h1)/2;
    }
    //缩放图片如下
    if(value == INIT_Position)
    {
        ui->HMovePicture->setValue(0);
        ui->VMovePicture->setValue(0);
        ui->label_picture->setGeometry(INIT_x, INIT_y, INIT_width, INIT_height);
        picture_move = false;
        return;
    }
    ui->label_picture->setGeometry(x0, y0, w, h);
    picture_scale = true;
    /*以下功能存在待解决问题*/
    /*ui->label_picture->resize(w,h);
    //滚动条同时改变位置
    ui->HMovePicture->setValue(500*(x0-x)/(11*x));
    ui->VMovePicture->setValue(500*(y0-y)/(11*x));*/
}

void MainWindow::on_HMovePicture_valueChanged(int value)//拖动水平滚动条，水平移动图片
{
    int w1,w2;//分别表示目前窗口中图片的宽、放置图片的widget的宽
    w1=ui->label_picture->size().width();
    w2=ui->widget->size().width();
    if(w1>w2)//图片大于窗口时才可平移
    {
        //图片绘制原点(x,y)
        int x,y;
        x = (w2 - w1)/2;    y = ui->label_picture->pos().ry();
        //向左拖动滑块，图片绘制原点向右平移,反之亦然
        //ui->label_picture->setGeometry(x + value*1.1*x/50, y, w1, h1);//将图片位置水平移动
        ui->label_picture->move(x + value*1.1*x/50, y);//将图片位置水平移动
        picture_move = true;
    }
}

void MainWindow::on_VMovePicture_valueChanged(int value)//拖动竖直滚动条，竖直移动图片
{
    int h1,h2;//分别表示目前窗口中图片的高、放置图片的widget的高
    h1=ui->label_picture->size().height();
    h2=ui->widget->size().height();
    if(h1>h2)//图片大于窗口时才可平移
    {
        //图片绘制原点(x,y)
        int x,y;
        x = ui->label_picture->pos().rx();    y = (h2 - h1)/2;
        //向下拖动滑块，图片绘制原点向上平移,反之亦然
        //ui->label_picture->setGeometry(x, y + value*1.1*y/50, w1, h1);//将图片在位置垂直方向移动
        ui->label_picture->move(x, y + value*1.1*y/50);//将图片在位置垂直方向移动
        picture_move = true;
    }
}

void MainWindow::on_action_Win_content_fix_triggered()//使图片内容显示最佳/适应内容
{
    ui->scalePicture->setValue(40);//与ui界面须一致
    ui->HMovePicture->setValue(0);
    ui->VMovePicture->setValue(0);
    ui->label_picture->setGeometry(INIT_x, INIT_y, INIT_width, INIT_height);
    picture_move = false;
}

void MainWindow::on_action_Win_window_fix_triggered()//使图片适应窗口/适应窗口
{
    ui->HMovePicture->setValue(0);
    ui->VMovePicture->setValue(0);

    int w_temp, h_temp;
    //分别表示放置图片的widget的宽高
    int w2=ui->widget->size().width();
    int h2=ui->widget->size().height();
    //图片过大时


    //std::cout<<pic_height<<std::endl;
    //std::cout<<winMaxWidth<<std::endl;
    //std::cout<<winMaxHeight<<std::endl;
    /*
    //以下if-else语句已经由结果更准确的pixmap->scaled()函数代替
    if(pic_width>winMaxWidth||pic_height>winMaxHeight)
    {
        float redu = 0.8;//冗余量
        if(pic_width/winMaxWidth>pic_height/winMaxHeight)
        {w_temp = redu*winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
        else
        {h_temp = redu*winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
    }
    else    //图片过小时,对于某些特殊尺寸图片仍可能出现问题，现已排除正方形存在的问题
    {
        float redu = 0.8;//冗余量
        if(winMaxWidth/pic_width>winMaxHeight/pic_height || winMaxWidth/pic_width==winMaxHeight/pic_height)
            {h_temp = redu*winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
        else
            {w_temp = redu*winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
    }
    //std::cout<<pic_width<<std::endl<<pic_height<<std::endl<<w_temp<<std::endl<<h_temp<<std::endl;
    */
    //新的方式获取适应窗口的宽和高
    QSize fixedSize = pixmap->scaled(QSize(winMaxWidth,winMaxHeight),Qt::KeepAspectRatio,Qt::FastTransformation).size();
    float redu = 0.8;//冗余量
    w_temp = redu*fixedSize.width(); h_temp = redu*fixedSize.height();
    ui->label_picture->setGeometry((w2-w_temp)/2,(h2-h_temp)/2,w_temp,h_temp);//将图片放置在中央位置

    picture_move = false;//否则图片错位
    picture_scale = true;//否则打开图片立即使用此功能时无效
}

void MainWindow::on_action_File_Save_triggered()//保存图片，将tmp.dat另存即可
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("PNG Files(*.PNG);;BMP Files(*.BMP);;JPEG Files(*.JPEG);;JPG Files(*.JPG);;GIF Files(*.GIF)"));
    if(!path.isEmpty())
    {
        if(pixmap->save(path))
            QMessageBox::information(this, QStringLiteral("保存图片"), QStringLiteral("保存成功!"));
        else
            QMessageBox::warning(this, QStringLiteral("保存图片"), QStringLiteral("保存失败."));
    }
    else
    {
        QMessageBox::information(this, QStringLiteral("保存图片"), QStringLiteral("未选择保存路径."));
    }
}

/*以下是图像处理部分*/

void MainWindow::on_action_Edit_GrayProcess24_triggered()//灰度处理
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("GrayProcess24.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 2 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_Smooth4_triggered()//4-邻域平滑
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("Smooth4.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 4 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_Smooth8_triggered()//8-邻域平滑
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("Smooth8.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 5 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_FlipHorizontal_triggered()//水平翻转
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("FlipHorizontal.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 6 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();

    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_FlipVertical_triggered()//垂直翻转
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("FlipVertical.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 7 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_Revert_triggered()//恢复原图
{
    pixmap->load("original.bmp");
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("ReWrite.exe original.bmp tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 1 original.bmp tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_ConvertToTxt_triggered()//转换为txt文件
{
    //第一步，先将偏大图片缩小
    if(pic_width>512||pic_height>464)
    {
        /*if(pic_width/512>pic_height/464)
            (pixmap->scaledToWidth(512,Qt::FastTransformation)).save("tmp_bmp.bmp");
        else
            (pixmap->scaledToHeight(464,Qt::FastTransformation)).save("tmp_bmp.bmp");*/
        pixmap->scaled(QSize(512,464),Qt::KeepAspectRatio,Qt::FastTransformation).save("tmp_bmp.bmp");
    }
    else
        pixmap->save("tmp_bmp.bmp");
    //第二步，将图像转为8位灰度图
    QProcess process01(this);
    process01.start("cmd_copy.exe");
    //process01.write ("Convert24to8Bit_Grey.exe tmp_bmp.bmp tmp_bmp.bmp\n\r");
    process01.write ("outsideEXE_FPS.exe 3 tmp_bmp.bmp tmp_bmp.bmp\n\r");
    process01.write ("exit\n\r");
    process01.waitForFinished();
    process01.close();
    //第三步，转换为txt文件
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write("ConvertToTxt.exe tmp_bmp.bmp 转换为ASCII字符.txt\n\r");
    process.write("outsideEXE_FPS.exe 9 tmp_bmp.bmp 转换为ASCII字符.txt\n\r");
    process.write("转换为ASCII字符.txt\n\r");
    process.write("exit\n\r");
    process.waitForFinished();
    process.close();
    //完成提示
    QMessageBox::information(this,QStringLiteral("转换为TXT文件"),QStringLiteral("转换成功! 手动另存保存，或在程序目录中寻找\"转换为ASCII字符.txt\""));
}

void MainWindow::on_action_Edit_Brightness_triggered(bool checked)//亮度调节_toolBar
{
    std::cout<<checked<<std::endl;
    ui->widget_Brightness->setHidden(!checked);
    if(checked)
    {
        preview->load("tmp.dat");//载入原始数据
        /*if(pic_width>350 || pic_height>220)
            preview->scaled(QSize(350,220),Qt::KeepAspectRatio,Qt::SmoothTransformation).save("preview.bmp");
        else
            preview->save("preview.bmp");*/
        preview->scaled(QSize(350,220),Qt::KeepAspectRatio,Qt::SmoothTransformation).save("preview.bmp");
        preview->load("preview.bmp");
        ui->picture_preview_Brightness->setPixmap(*preview);
    }
    ui->toolBar->setEnabled(!checked);
    ui->menu_Edit->setEnabled(!checked);
    ui->action_File_Open->setEnabled(!checked);
    ui->action_File_Save->setEnabled(!checked);
}

void MainWindow::on_Slider_Brightness_valueChanged(int value)//调节亮度_滑动条
{
    if (!convertBySpinBox)
        ui->doubleSpinBox_Brightness->setValue((double)value/100);//修改数字输入框的值
}

void MainWindow::on_doubleSpinBox_Brightness_valueChanged(double arg1)//调节亮度_数字框
{
    convertBySpinBox = true;
    ui->Slider_Brightness->setValue(100*arg1);//同时改变滑动条的值
    convertBySpinBox = false;
    //exe的参数
    //QString qstr(QString("Brightness.exe preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 11 preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    preview->load("preview_tmp.dat");
    ui->picture_preview_Brightness->setPixmap(*preview);//载入变化后的预览图
}

void MainWindow::on_apply_Brightness_pressed()//亮度调节_应用按钮
{
    //QString qstr(QString("Brightness.exe tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 11 tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui->label_picture->setPixmap(*pixmap);
    //处理弹出的窗口
    ui->widget_Brightness->setHidden(true);
    ui->action_Edit_Brightness->setChecked(false);
    ui->doubleSpinBox_Brightness->setValue(0.5);
    convertBySpinBox = false;
    //取消防误触
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_close_Brightness_pressed()//亮度调节_取消按钮
{
    //处理弹出的窗口
    ui->widget_Brightness->setHidden(true);
    ui->action_Edit_Brightness->setChecked(false);
    ui->doubleSpinBox_Brightness->setValue(0.5);
    convertBySpinBox = false;
    //取消防误触
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_action_Edit_Contrast_triggered(bool checked)//对比度调节_toolBar
{
    ui->widget_Contrast->setHidden(!checked);
    if(checked)
    {
        preview->load("tmp.dat");//载入原始数据
        preview->scaled(QSize(350,220),Qt::KeepAspectRatio,Qt::SmoothTransformation).save("preview.bmp");
        preview->load("preview.bmp");
        ui->picture_preview_Contrast->setPixmap(*preview);
    }
    ui->toolBar->setEnabled(!checked);
    ui->menu_Edit->setEnabled(!checked);
    ui->action_File_Open->setEnabled(!checked);
    ui->action_File_Save->setEnabled(!checked);
}

void MainWindow::on_Slider_Contrast_valueChanged(int value)//对比度调节_滑动条
{
    if (!convertBySpinBox)
        ui->doubleSpinBox_Contrast->setValue((double)value/100);//修改数字输入框的值
}

void MainWindow::on_doubleSpinBox_Contrast_valueChanged(double arg1)//对比度调节_数字框
{
    convertBySpinBox = true;
    ui->Slider_Contrast->setValue(100*arg1);//同时改变滑动条的值
    convertBySpinBox = false;
    //exe的参数
    //QString qstr(QString("Contrast.exe preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 12 preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    preview->load("preview_tmp.dat");
    ui->picture_preview_Contrast->setPixmap(*preview);//载入变化后的预览图
}

void MainWindow::on_apply_Contrast_pressed()//对比度调节_应用按钮
{
    //QString qstr(QString("Contrast.exe tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 12 tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui->label_picture->setPixmap(*pixmap);
    //处理弹出的窗口
    ui->widget_Contrast->setHidden(true);
    ui->action_Edit_Contrast->setChecked(false);
    ui->doubleSpinBox_Contrast->setValue(0.5);
    convertBySpinBox = false;
    //取消防误触
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_close_Contrast_pressed()//对比度调节_取消按钮
{
    //处理弹出的窗口
    ui->widget_Contrast->setHidden(true);
    ui->action_Edit_Contrast->setChecked(false);
    ui->doubleSpinBox_Contrast->setValue(0.5);
    convertBySpinBox = false;
    //取消防误触
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_action_Edit_Convert8toMonochrome_triggered(bool checked)//转为黑白图像
{
    ui->widget_Monochrome->setHidden(!checked);
    if(checked)
    {
        preview->load("tmp.dat");//载入原始数据
        preview->scaled(QSize(350,220),Qt::KeepAspectRatio,Qt::SmoothTransformation).save("preview.bmp");

        //外部exe处理图像
        QProcess process(this);
        process.start("cmd_copy.exe");
        //process.write ("Convert24to8Bit_Grey.exe preview.bmp preview.bmp\n\r");
        process.write ("outsideEXE_FPS.exe 3 preview.bmp preview.bmp\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        //QString qstr(QString("Convert8toMonochrome.exe preview.bmp preview_tmp.bmp ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
        QString qstr(QString("outsideEXE_FPS.exe 13 preview.bmp preview_tmp.bmp ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
        QProcess process01(this);
        process01.start("cmd_copy.exe");
        process01.write (qstr.toStdString().c_str());
        process01.write ("exit\n\r");
        process01.waitForFinished();
        process01.close();

        preview->load("preview_tmp.bmp");
        ui->picture_preview_Monochrome->setPixmap(*preview);
    }
    ui->toolBar->setEnabled(!checked);
    ui->menu_Edit->setEnabled(!checked);
    ui->action_File_Open->setEnabled(!checked);
    ui->action_File_Save->setEnabled(!checked);
}

void MainWindow::on_Slider_Monochrome_valueChanged(int value)//黑白处理_滑动条
{
    if (!convertBySpinBox)
        ui->spinBox_Monochrome->setValue(value);//修改数字输入框的值
}

void MainWindow::on_spinBox_Monochrome_valueChanged(int arg1)//黑白处理_数字框
{
    convertBySpinBox = true;
    ui->Slider_Monochrome->setValue(arg1);//同时改变滑动条的值
    convertBySpinBox = false;
    //exe的参数
    //QString qstr(QString("Convert8toMonochrome.exe preview.bmp preview_tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 13 preview.bmp preview_tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    preview->load("preview_tmp.dat");
    ui->picture_preview_Monochrome->setPixmap(*preview);//载入变化后的预览图
}

void MainWindow::on_apply_Monochrome_pressed()//黑白处理_应用按钮
{
    //QString qstr(QString("Convert24to8Bit_Grey.exe tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 3 tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    //QString qstr01(QString("Convert8toMonochrome.exe tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    QString qstr01(QString("outsideEXE_FPS.exe 13 tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write (qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();

    QProcess process01(this);
    process01.start("cmd_copy.exe");
    process01.write (qstr01.toStdString().c_str());
    process01.write ("exit\n\r");
    process01.waitForFinished();
    process01.close();

    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);

    //处理弹出的窗口
    ui->widget_Monochrome->setHidden(true);
    ui->action_Edit_Convert8toMonochrome->setChecked(false);
    ui->spinBox_Monochrome->setValue(128);
    convertBySpinBox = false;
    //取消防误触
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_close_Monochrome_pressed()//黑白处理_取消按钮
{
    //处理弹出的窗口
    ui->widget_Monochrome->setHidden(true);
    ui->action_Edit_Convert8toMonochrome->setChecked(false);
    ui->spinBox_Monochrome->setValue(128);
    convertBySpinBox = false;
    //取消防误触
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_action_Edit_InverseColor_triggered()//反色处理
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("InverseColor.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 10 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_Convert24to256_triggered()//24位色图片转8位256色图片
{
    //外部exe处理图像
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("Convert24to8Bit.exe tmp.dat tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 8 tmp.dat tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_File_Quit_triggered()
{
    QApplication::closeAllWindows();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QProcess process(this);
    switch( QMessageBox::information( this, QStringLiteral("退出程序"),
      QStringLiteral("你的操作将不会保存，是否确认退出程序？"),
      QStringLiteral("是"), QStringLiteral("否"),
      0, 1) )
     {
        case 0:
            //删除临时文件
            process.start("cmd_copy.exe");
            process.write ("del tmp.dat\n\r");
            process.write ("del original.bmp\n\r");
            process.write ("del preview.bmp\n\r");
            process.write ("del preview_tmp.bmp\n\r");
            process.write ("del preview_tmp.dat\n\r");
            process.write ("del tmp_bmp.bmp\n\r");
            process.write ("del toBeUnZip.fpsLZW\n\r");
            process.write ("del tmp_zip.fpsLZW\n\r");
            process.write ("exit\n\r");
            process.waitForFinished();
            event->accept();
            break;
        case 1:
     default:
            event->ignore();
            break;
     }
     process.close();
}

void MainWindow::on_action_Help_About_triggered()
{
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write ("README.txt\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
}

/*
void MainWindow::on_action_File_UnZip_triggered()//解压fpsLZW图片并打开,与File_Open类似
{
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("选择待解压的文件"), ".", tr("fpsLZW Files (*.fpsLZW)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))//不编辑只读文件
        {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        //先清除目录中现存的"toBeUnZip.fpsLZW"
        QProcess process02(this);
        process02.start("cmd_copy.exe");
        process02.write ("del toBeUnZip.fpsLZW\n\r");
        process02.write ("exit\n\r");
        process02.waitForFinished();
        process02.close();

        file.copy("toBeUnZip.fpsLZW");
        //调用外部exe解压fpsLZW文件
        QProcess process01(this);
        process01.start("cmd_copy.exe");
        process01.write ("LZWUN_BMP_FPS.exe toBeUnZip.fpsLZW original.bmp\n\r");
        process01.write ("exit\n\r");
        process01.waitForFinished();
        process01.close();

        QImage *image = new QImage("original.bmp");//载入图片

        //QPixmap对象载入内容
        pixmap -> load("original.bmp");

        //外部exe处理图像
        QProcess process(this);
        process.start("cmd_copy.exe");
        //process.write ("ReWrite.exe original.bmp tmp.dat\n\r");
        process.write ("outsideEXE_FPS.exe 1 original.bmp tmp.dat\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        //显示图片
        pixmap->load("tmp.dat");
        ui -> label_picture -> setPixmap(*pixmap);
        file.close();

        //补充类中私有成员的信息
        int MemoryLimit = 0xffffff; //限制图片占用的内存
        pic_width = image->width();     pic_height = image->height();
        //计算放大倍数
        amplification = 4;
        if(pic_width*pic_height>MemoryLimit)
            amplification = 1;
        else
            if(MemoryLimit/(pic_width*pic_height)<16)
                amplification = sqrt((int)(MemoryLimit/(pic_width*pic_height)));
        //计算缩小倍数(百分数),必须在载入图片之后，否则sizeHint()得不到正确结果
        int w_temp, h_temp;
        minification = 80;
        if(pic_width>winMaxWidth||pic_height>winMaxHeight)
        {
            if(pic_width/winMaxWidth>pic_height/winMaxHeight)
            {w_temp = winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
                else
                {h_temp = winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
            //得到缩小系数
            minification = (w_temp*80)/ui->label_picture->sizeHint().width();
        }

        int w0,w2,h0,h2;//分别表示最佳的图片的宽高、放置图片的widget的宽高
        w0=ui->label_picture->sizeHint().width();
        h0=ui->label_picture->sizeHint().height();
        w2=ui->widget->size().width();
        h2=ui->widget->size().height();
        INIT_x = (w2-w0)/2;    INIT_y = (h2-h0)/2;
        INIT_width = w0;    INIT_height = h0;

        picture_move = false;//防止主窗口自动调整图片位置
        picture_scale = false;//防止主窗口自动调整图片位置
        picture_open = true;//防止主窗口自动调整图片位置
        checked_Brightness = false;
        checked_Contrast = false;

        QMessageBox::information(this, QStringLiteral("解压图片"), QStringLiteral("成功解压并打开!"));
    }
    else
    {
        //QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}
*/

/*
void MainWindow::on_action_File_Zip_triggered()//压缩BMP图片并另存为fpsLZW文件,与File_Save类似
{
    QString path = QFileDialog::getSaveFileName(this, QStringLiteral("选择压缩图片的保存路径"), ".", tr("fpsLZW Files (*.fpsLZW)"));
    if(!path.isEmpty())
    {
        //调用外部exe压缩图片
        QProcess process(this);
        process.start("cmd_copy.exe");
        process.write ("LZW_BMP_FPS.exe tmp.dat tmp_zip.fpsLZW\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        QFile file("tmp_zip.fpsLZW");

        if(file.rename(path))
            QMessageBox::information(this, QStringLiteral("压缩图片"), QStringLiteral("压缩保存成功!"));
        else
            QMessageBox::warning(this, QStringLiteral("压缩图片"), QStringLiteral("压缩保存失败."));
    }
    else
    {
        QMessageBox::information(this, QStringLiteral("压缩图片"), QStringLiteral("未选择保存路径."));
    }
}
*/
