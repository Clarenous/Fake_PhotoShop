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
    //ƽ�ƹ����� ��ʾ״̬
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
        ui->label_picture->resize(ui->label_picture->sizeHint());//�Զ����ú��ʵ�ͼƬ��С�����洰�ڴ�С�仯
        //ui->label_picture->resize(ui->widget->size());ͼƬ�ߴ��洰�ڱ仯�����߱�ʧ��
    }

    int w0,w2,h0,h2;//�ֱ��ʾ��ѵ�ͼƬ�Ŀ�ߡ�����ͼƬ��widget�Ŀ��
    w0=ui->label_picture->sizeHint().width();
    h0=ui->label_picture->sizeHint().height();
    w2=ui->widget->size().width();
    h2=ui->widget->size().height();

    if((!picture_move) || picture_open)
    {
        int w1,h1;//Ŀǰ������ͼƬ�Ŀ��
        w1=ui->label_picture->size().width();
        h1=ui->label_picture->size().height();
        ui->label_picture->setGeometry((w2-w1)/2,(h2-h1)/2,w1,h1);//��ͼƬ����������λ��
        picture_open = false;
    }

    INIT_x = (w2-w0)/2;    INIT_y = (h2-h0)/2;
    INIT_width = w0;    INIT_height = h0;
}

void MainWindow::on_action_File_Open_triggered()//Function_��ͼƬ�ļ�
{
    //QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("All File(*.*);;BMP Files(*.BMP);;JPG Files(*.JPG);;JPEG Files(*.JPEG);;PNG Files(*.PNG);;GIF Files(*.GIF)"));
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), ".", tr("Images (*.BMP *.JPG *.JPEG *.PNG *.GIF)"));
    if(!path.isEmpty())
    {
        QImage *image = new QImage(path);//����ͼƬ
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))//���༭ֻ���ļ�
        {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }

        //QPixmap������������
        pixmap -> convertFromImage(*image);//��ͼƬд��QPixmap ����

        if(pixmap->save("original.bmp"))
            std::cout<<"good!"<<std::endl;
        //�ⲿexe����ͼ��
        QProcess process(this);
        process.start("cmd_copy.exe");
        //process.write ("ReWrite.exe original.bmp tmp.dat\n\r");
        process.write ("outsideEXE_FPS.exe 1 original.bmp tmp.dat\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        //��ʾͼƬ
        pixmap->load("tmp.dat");
        ui -> label_picture -> setPixmap(*pixmap);
        file.close();

        //��������˽�г�Ա����Ϣ
        int MemoryLimit = 0xffffff; //����ͼƬռ�õ��ڴ�
        pic_width = image->width();     pic_height = image->height();
        //����Ŵ���
        amplification = 4;
        if(pic_width*pic_height>MemoryLimit)
            amplification = 1;
        else
            if(MemoryLimit/(pic_width*pic_height)<16)
                amplification = sqrt((int)(MemoryLimit/(pic_width*pic_height)));
        //������С����(�ٷ���),����������ͼƬ֮�󣬷���sizeHint()�ò�����ȷ���
        int w_temp, h_temp;
        minification = 80;
        if(pic_width>winMaxWidth||pic_height>winMaxHeight)
        {
            if(pic_width/winMaxWidth>pic_height/winMaxHeight)
            {w_temp = winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
                else
                {h_temp = winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
            //�õ���Сϵ��
            minification = (w_temp*80)/ui->label_picture->sizeHint().width();
        }

        int w0,w2,h0,h2;//�ֱ��ʾ��ѵ�ͼƬ�Ŀ�ߡ�����ͼƬ��widget�Ŀ��
        w0=ui->label_picture->sizeHint().width();
        h0=ui->label_picture->sizeHint().height();
        w2=ui->widget->size().width();
        h2=ui->widget->size().height();
        INIT_x = (w2-w0)/2;    INIT_y = (h2-h0)/2;
        INIT_width = w0;    INIT_height = h0;

        picture_move = false;//��ֹ�������Զ�����ͼƬλ��
        picture_scale = false;//��ֹ�������Զ�����ͼƬλ��
        picture_open = true;//��ֹ�������Զ�����ͼƬλ��
        checked_Brightness = false;
        checked_Contrast = false;
    }
    else
    {
        //QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}

void MainWindow::on_scalePicture_valueChanged(int value)//�϶�ˮƽ����ʵ�����Ż���
{
    int INIT_Position = 40;//�����ʼλ�ã���Ҫ��ui���汣��һ��
    int w0,w1,w2,h0,h1,h2;    //�ֱ��ʾ��ѵ�ͼƬ��ߡ�Ŀǰ������ͼƬ�Ŀ�ߡ��Լ�����ͼƬ��widget�Ŀ��
    w0=ui->label_picture->sizeHint().width();
    h0=ui->label_picture->sizeHint().height();
    w1=ui->label_picture->size().width();
    h1=ui->label_picture->size().height();
    w2=ui->widget->size().width();
    h2=ui->widget->size().height();
    int w,h;    //���ű仯��Ŀ�͸�
    float k = 1;    //���ű���ϵ��
    //ͼƬ����С�ͷŴ�ʱ�Ĳ��Բ�ͬ
    if(value < INIT_Position)//��СͼƬ,��С��С��minification%��С
        k = (float)(minification + ((float)(100-minification)/INIT_Position)*value)/100;
    if(value > INIT_Position)//�Ŵ�ͼƬ�����Ŵ���amplification����С
        k = (1 + (float)(amplification-1)*(value - INIT_Position)/(100-INIT_Position));
    w = k*w0;   h = k*h0;
    //ͼƬ����ԭ��(x,y)
    int x,y;
    x = ui->label_picture->pos().rx();    y = ui->label_picture->pos().ry();
    //�����error
    int error_x = x - (w2-w1)/2;    int error_y = y - (h2 - h1)/2;
    //�����������������ͼƬ����ԭ��(x0,y0)
    int x0 = (w2-w1)/2; int y0 = (h2 - h1)/2;
    //�Ŵ�ʱ��Ҫ��������ԭ��(x0,y0)
    if(value > INIT_Position)
    {
        x0 = (k/amplification-1)*(float)error_x + (w2-w1)/2;     y0 = (k/amplification-1)*(float)error_y + (h2 - h1)/2;
    }
    //����ͼƬ����
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
    /*���¹��ܴ��ڴ��������*/
    /*ui->label_picture->resize(w,h);
    //������ͬʱ�ı�λ��
    ui->HMovePicture->setValue(500*(x0-x)/(11*x));
    ui->VMovePicture->setValue(500*(y0-y)/(11*x));*/
}

void MainWindow::on_HMovePicture_valueChanged(int value)//�϶�ˮƽ��������ˮƽ�ƶ�ͼƬ
{
    int w1,w2;//�ֱ��ʾĿǰ������ͼƬ�Ŀ�����ͼƬ��widget�Ŀ�
    w1=ui->label_picture->size().width();
    w2=ui->widget->size().width();
    if(w1>w2)//ͼƬ���ڴ���ʱ�ſ�ƽ��
    {
        //ͼƬ����ԭ��(x,y)
        int x,y;
        x = (w2 - w1)/2;    y = ui->label_picture->pos().ry();
        //�����϶����飬ͼƬ����ԭ������ƽ��,��֮��Ȼ
        //ui->label_picture->setGeometry(x + value*1.1*x/50, y, w1, h1);//��ͼƬλ��ˮƽ�ƶ�
        ui->label_picture->move(x + value*1.1*x/50, y);//��ͼƬλ��ˮƽ�ƶ�
        picture_move = true;
    }
}

void MainWindow::on_VMovePicture_valueChanged(int value)//�϶���ֱ����������ֱ�ƶ�ͼƬ
{
    int h1,h2;//�ֱ��ʾĿǰ������ͼƬ�ĸߡ�����ͼƬ��widget�ĸ�
    h1=ui->label_picture->size().height();
    h2=ui->widget->size().height();
    if(h1>h2)//ͼƬ���ڴ���ʱ�ſ�ƽ��
    {
        //ͼƬ����ԭ��(x,y)
        int x,y;
        x = ui->label_picture->pos().rx();    y = (h2 - h1)/2;
        //�����϶����飬ͼƬ����ԭ������ƽ��,��֮��Ȼ
        //ui->label_picture->setGeometry(x, y + value*1.1*y/50, w1, h1);//��ͼƬ��λ�ô�ֱ�����ƶ�
        ui->label_picture->move(x, y + value*1.1*y/50);//��ͼƬ��λ�ô�ֱ�����ƶ�
        picture_move = true;
    }
}

void MainWindow::on_action_Win_content_fix_triggered()//ʹͼƬ������ʾ���/��Ӧ����
{
    ui->scalePicture->setValue(40);//��ui������һ��
    ui->HMovePicture->setValue(0);
    ui->VMovePicture->setValue(0);
    ui->label_picture->setGeometry(INIT_x, INIT_y, INIT_width, INIT_height);
    picture_move = false;
}

void MainWindow::on_action_Win_window_fix_triggered()//ʹͼƬ��Ӧ����/��Ӧ����
{
    ui->HMovePicture->setValue(0);
    ui->VMovePicture->setValue(0);

    int w_temp, h_temp;
    //�ֱ��ʾ����ͼƬ��widget�Ŀ��
    int w2=ui->widget->size().width();
    int h2=ui->widget->size().height();
    //ͼƬ����ʱ


    //std::cout<<pic_height<<std::endl;
    //std::cout<<winMaxWidth<<std::endl;
    //std::cout<<winMaxHeight<<std::endl;
    /*
    //����if-else����Ѿ��ɽ����׼ȷ��pixmap->scaled()��������
    if(pic_width>winMaxWidth||pic_height>winMaxHeight)
    {
        float redu = 0.8;//������
        if(pic_width/winMaxWidth>pic_height/winMaxHeight)
        {w_temp = redu*winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
        else
        {h_temp = redu*winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
    }
    else    //ͼƬ��Сʱ,����ĳЩ����ߴ�ͼƬ�Կ��ܳ������⣬�����ų������δ��ڵ�����
    {
        float redu = 0.8;//������
        if(winMaxWidth/pic_width>winMaxHeight/pic_height || winMaxWidth/pic_width==winMaxHeight/pic_height)
            {h_temp = redu*winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
        else
            {w_temp = redu*winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
    }
    //std::cout<<pic_width<<std::endl<<pic_height<<std::endl<<w_temp<<std::endl<<h_temp<<std::endl;
    */
    //�µķ�ʽ��ȡ��Ӧ���ڵĿ�͸�
    QSize fixedSize = pixmap->scaled(QSize(winMaxWidth,winMaxHeight),Qt::KeepAspectRatio,Qt::FastTransformation).size();
    float redu = 0.8;//������
    w_temp = redu*fixedSize.width(); h_temp = redu*fixedSize.height();
    ui->label_picture->setGeometry((w2-w_temp)/2,(h2-h_temp)/2,w_temp,h_temp);//��ͼƬ����������λ��

    picture_move = false;//����ͼƬ��λ
    picture_scale = true;//�����ͼƬ����ʹ�ô˹���ʱ��Ч
}

void MainWindow::on_action_File_Save_triggered()//����ͼƬ����tmp.dat��漴��
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save File"), ".", tr("PNG Files(*.PNG);;BMP Files(*.BMP);;JPEG Files(*.JPEG);;JPG Files(*.JPG);;GIF Files(*.GIF)"));
    if(!path.isEmpty())
    {
        if(pixmap->save(path))
            QMessageBox::information(this, QStringLiteral("����ͼƬ"), QStringLiteral("����ɹ�!"));
        else
            QMessageBox::warning(this, QStringLiteral("����ͼƬ"), QStringLiteral("����ʧ��."));
    }
    else
    {
        QMessageBox::information(this, QStringLiteral("����ͼƬ"), QStringLiteral("δѡ�񱣴�·��."));
    }
}

/*������ͼ������*/

void MainWindow::on_action_Edit_GrayProcess24_triggered()//�Ҷȴ���
{
    //�ⲿexe����ͼ��
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

void MainWindow::on_action_Edit_Smooth4_triggered()//4-����ƽ��
{
    //�ⲿexe����ͼ��
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

void MainWindow::on_action_Edit_Smooth8_triggered()//8-����ƽ��
{
    //�ⲿexe����ͼ��
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

void MainWindow::on_action_Edit_FlipHorizontal_triggered()//ˮƽ��ת
{
    //�ⲿexe����ͼ��
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

void MainWindow::on_action_Edit_FlipVertical_triggered()//��ֱ��ת
{
    //�ⲿexe����ͼ��
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

void MainWindow::on_action_Edit_Revert_triggered()//�ָ�ԭͼ
{
    pixmap->load("original.bmp");
    //�ⲿexe����ͼ��
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write ("ReWrite.exe original.bmp tmp.dat\n\r");
    process.write ("outsideEXE_FPS.exe 1 original.bmp tmp.dat\n\r");
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    ui -> label_picture -> setPixmap(*pixmap);
}

void MainWindow::on_action_Edit_ConvertToTxt_triggered()//ת��Ϊtxt�ļ�
{
    //��һ�����Ƚ�ƫ��ͼƬ��С
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
    //�ڶ�������ͼ��תΪ8λ�Ҷ�ͼ
    QProcess process01(this);
    process01.start("cmd_copy.exe");
    //process01.write ("Convert24to8Bit_Grey.exe tmp_bmp.bmp tmp_bmp.bmp\n\r");
    process01.write ("outsideEXE_FPS.exe 3 tmp_bmp.bmp tmp_bmp.bmp\n\r");
    process01.write ("exit\n\r");
    process01.waitForFinished();
    process01.close();
    //��������ת��Ϊtxt�ļ�
    QProcess process(this);
    process.start("cmd_copy.exe");
    //process.write("ConvertToTxt.exe tmp_bmp.bmp ת��ΪASCII�ַ�.txt\n\r");
    process.write("outsideEXE_FPS.exe 9 tmp_bmp.bmp ת��ΪASCII�ַ�.txt\n\r");
    process.write("ת��ΪASCII�ַ�.txt\n\r");
    process.write("exit\n\r");
    process.waitForFinished();
    process.close();
    //�����ʾ
    QMessageBox::information(this,QStringLiteral("ת��ΪTXT�ļ�"),QStringLiteral("ת���ɹ�! �ֶ���汣�棬���ڳ���Ŀ¼��Ѱ��\"ת��ΪASCII�ַ�.txt\""));
}

void MainWindow::on_action_Edit_Brightness_triggered(bool checked)//���ȵ���_toolBar
{
    std::cout<<checked<<std::endl;
    ui->widget_Brightness->setHidden(!checked);
    if(checked)
    {
        preview->load("tmp.dat");//����ԭʼ����
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

void MainWindow::on_Slider_Brightness_valueChanged(int value)//��������_������
{
    if (!convertBySpinBox)
        ui->doubleSpinBox_Brightness->setValue((double)value/100);//�޸�����������ֵ
}

void MainWindow::on_doubleSpinBox_Brightness_valueChanged(double arg1)//��������_���ֿ�
{
    convertBySpinBox = true;
    ui->Slider_Brightness->setValue(100*arg1);//ͬʱ�ı们������ֵ
    convertBySpinBox = false;
    //exe�Ĳ���
    //QString qstr(QString("Brightness.exe preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 11 preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    //�ⲿexe����ͼ��
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    preview->load("preview_tmp.dat");
    ui->picture_preview_Brightness->setPixmap(*preview);//����仯���Ԥ��ͼ
}

void MainWindow::on_apply_Brightness_pressed()//���ȵ���_Ӧ�ð�ť
{
    //QString qstr(QString("Brightness.exe tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 11 tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Brightness->value())+QString("\n\r"));
    //�ⲿexe����ͼ��
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui->label_picture->setPixmap(*pixmap);
    //�������Ĵ���
    ui->widget_Brightness->setHidden(true);
    ui->action_Edit_Brightness->setChecked(false);
    ui->doubleSpinBox_Brightness->setValue(0.5);
    convertBySpinBox = false;
    //ȡ������
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_close_Brightness_pressed()//���ȵ���_ȡ����ť
{
    //�������Ĵ���
    ui->widget_Brightness->setHidden(true);
    ui->action_Edit_Brightness->setChecked(false);
    ui->doubleSpinBox_Brightness->setValue(0.5);
    convertBySpinBox = false;
    //ȡ������
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_action_Edit_Contrast_triggered(bool checked)//�Աȶȵ���_toolBar
{
    ui->widget_Contrast->setHidden(!checked);
    if(checked)
    {
        preview->load("tmp.dat");//����ԭʼ����
        preview->scaled(QSize(350,220),Qt::KeepAspectRatio,Qt::SmoothTransformation).save("preview.bmp");
        preview->load("preview.bmp");
        ui->picture_preview_Contrast->setPixmap(*preview);
    }
    ui->toolBar->setEnabled(!checked);
    ui->menu_Edit->setEnabled(!checked);
    ui->action_File_Open->setEnabled(!checked);
    ui->action_File_Save->setEnabled(!checked);
}

void MainWindow::on_Slider_Contrast_valueChanged(int value)//�Աȶȵ���_������
{
    if (!convertBySpinBox)
        ui->doubleSpinBox_Contrast->setValue((double)value/100);//�޸�����������ֵ
}

void MainWindow::on_doubleSpinBox_Contrast_valueChanged(double arg1)//�Աȶȵ���_���ֿ�
{
    convertBySpinBox = true;
    ui->Slider_Contrast->setValue(100*arg1);//ͬʱ�ı们������ֵ
    convertBySpinBox = false;
    //exe�Ĳ���
    //QString qstr(QString("Contrast.exe preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 12 preview.bmp preview_tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    //�ⲿexe����ͼ��
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    preview->load("preview_tmp.dat");
    ui->picture_preview_Contrast->setPixmap(*preview);//����仯���Ԥ��ͼ
}

void MainWindow::on_apply_Contrast_pressed()//�Աȶȵ���_Ӧ�ð�ť
{
    //QString qstr(QString("Contrast.exe tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 12 tmp.dat tmp.dat ")+QString::number(ui->doubleSpinBox_Contrast->value())+QString("\n\r"));
    //�ⲿexe����ͼ��
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    pixmap->load("tmp.dat");
    ui->label_picture->setPixmap(*pixmap);
    //�������Ĵ���
    ui->widget_Contrast->setHidden(true);
    ui->action_Edit_Contrast->setChecked(false);
    ui->doubleSpinBox_Contrast->setValue(0.5);
    convertBySpinBox = false;
    //ȡ������
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_close_Contrast_pressed()//�Աȶȵ���_ȡ����ť
{
    //�������Ĵ���
    ui->widget_Contrast->setHidden(true);
    ui->action_Edit_Contrast->setChecked(false);
    ui->doubleSpinBox_Contrast->setValue(0.5);
    convertBySpinBox = false;
    //ȡ������
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_action_Edit_Convert8toMonochrome_triggered(bool checked)//תΪ�ڰ�ͼ��
{
    ui->widget_Monochrome->setHidden(!checked);
    if(checked)
    {
        preview->load("tmp.dat");//����ԭʼ����
        preview->scaled(QSize(350,220),Qt::KeepAspectRatio,Qt::SmoothTransformation).save("preview.bmp");

        //�ⲿexe����ͼ��
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

void MainWindow::on_Slider_Monochrome_valueChanged(int value)//�ڰ״���_������
{
    if (!convertBySpinBox)
        ui->spinBox_Monochrome->setValue(value);//�޸�����������ֵ
}

void MainWindow::on_spinBox_Monochrome_valueChanged(int arg1)//�ڰ״���_���ֿ�
{
    convertBySpinBox = true;
    ui->Slider_Monochrome->setValue(arg1);//ͬʱ�ı们������ֵ
    convertBySpinBox = false;
    //exe�Ĳ���
    //QString qstr(QString("Convert8toMonochrome.exe preview.bmp preview_tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 13 preview.bmp preview_tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    //�ⲿexe����ͼ��
    QProcess process(this);
    process.start("cmd_copy.exe");
    process.write(qstr.toStdString().c_str());
    process.write ("exit\n\r");
    process.waitForFinished();
    process.close();
    preview->load("preview_tmp.dat");
    ui->picture_preview_Monochrome->setPixmap(*preview);//����仯���Ԥ��ͼ
}

void MainWindow::on_apply_Monochrome_pressed()//�ڰ״���_Ӧ�ð�ť
{
    //QString qstr(QString("Convert24to8Bit_Grey.exe tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    QString qstr(QString("outsideEXE_FPS.exe 3 tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    //QString qstr01(QString("Convert8toMonochrome.exe tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    QString qstr01(QString("outsideEXE_FPS.exe 13 tmp.dat tmp.dat ")+QString::number(ui->spinBox_Monochrome->value())+QString("\n\r"));
    //�ⲿexe����ͼ��
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

    //�������Ĵ���
    ui->widget_Monochrome->setHidden(true);
    ui->action_Edit_Convert8toMonochrome->setChecked(false);
    ui->spinBox_Monochrome->setValue(128);
    convertBySpinBox = false;
    //ȡ������
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_close_Monochrome_pressed()//�ڰ״���_ȡ����ť
{
    //�������Ĵ���
    ui->widget_Monochrome->setHidden(true);
    ui->action_Edit_Convert8toMonochrome->setChecked(false);
    ui->spinBox_Monochrome->setValue(128);
    convertBySpinBox = false;
    //ȡ������
    ui->toolBar->setEnabled(true);
    ui->menu_Edit->setEnabled(true);
    ui->action_File_Open->setEnabled(true);
    ui->action_File_Save->setEnabled(true);
}

void MainWindow::on_action_Edit_InverseColor_triggered()//��ɫ����
{
    //�ⲿexe����ͼ��
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

void MainWindow::on_action_Edit_Convert24to256_triggered()//24λɫͼƬת8λ256ɫͼƬ
{
    //�ⲿexe����ͼ��
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
    switch( QMessageBox::information( this, QStringLiteral("�˳�����"),
      QStringLiteral("��Ĳ��������ᱣ�棬�Ƿ�ȷ���˳�����"),
      QStringLiteral("��"), QStringLiteral("��"),
      0, 1) )
     {
        case 0:
            //ɾ����ʱ�ļ�
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
void MainWindow::on_action_File_UnZip_triggered()//��ѹfpsLZWͼƬ����,��File_Open����
{
    QString path = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ�����ѹ���ļ�"), ".", tr("fpsLZW Files (*.fpsLZW)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))//���༭ֻ���ļ�
        {
            QMessageBox::warning(this, tr("Read File"), tr("Cannot open file:\n%1").arg(path));
            return;
        }
        //�����Ŀ¼���ִ��"toBeUnZip.fpsLZW"
        QProcess process02(this);
        process02.start("cmd_copy.exe");
        process02.write ("del toBeUnZip.fpsLZW\n\r");
        process02.write ("exit\n\r");
        process02.waitForFinished();
        process02.close();

        file.copy("toBeUnZip.fpsLZW");
        //�����ⲿexe��ѹfpsLZW�ļ�
        QProcess process01(this);
        process01.start("cmd_copy.exe");
        process01.write ("LZWUN_BMP_FPS.exe toBeUnZip.fpsLZW original.bmp\n\r");
        process01.write ("exit\n\r");
        process01.waitForFinished();
        process01.close();

        QImage *image = new QImage("original.bmp");//����ͼƬ

        //QPixmap������������
        pixmap -> load("original.bmp");

        //�ⲿexe����ͼ��
        QProcess process(this);
        process.start("cmd_copy.exe");
        //process.write ("ReWrite.exe original.bmp tmp.dat\n\r");
        process.write ("outsideEXE_FPS.exe 1 original.bmp tmp.dat\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        //��ʾͼƬ
        pixmap->load("tmp.dat");
        ui -> label_picture -> setPixmap(*pixmap);
        file.close();

        //��������˽�г�Ա����Ϣ
        int MemoryLimit = 0xffffff; //����ͼƬռ�õ��ڴ�
        pic_width = image->width();     pic_height = image->height();
        //����Ŵ���
        amplification = 4;
        if(pic_width*pic_height>MemoryLimit)
            amplification = 1;
        else
            if(MemoryLimit/(pic_width*pic_height)<16)
                amplification = sqrt((int)(MemoryLimit/(pic_width*pic_height)));
        //������С����(�ٷ���),����������ͼƬ֮�󣬷���sizeHint()�ò�����ȷ���
        int w_temp, h_temp;
        minification = 80;
        if(pic_width>winMaxWidth||pic_height>winMaxHeight)
        {
            if(pic_width/winMaxWidth>pic_height/winMaxHeight)
            {w_temp = winMaxWidth;   h_temp = pic_height*w_temp/pic_width;}
                else
                {h_temp = winMaxHeight;  w_temp = (pic_width*h_temp)/pic_height;}
            //�õ���Сϵ��
            minification = (w_temp*80)/ui->label_picture->sizeHint().width();
        }

        int w0,w2,h0,h2;//�ֱ��ʾ��ѵ�ͼƬ�Ŀ�ߡ�����ͼƬ��widget�Ŀ��
        w0=ui->label_picture->sizeHint().width();
        h0=ui->label_picture->sizeHint().height();
        w2=ui->widget->size().width();
        h2=ui->widget->size().height();
        INIT_x = (w2-w0)/2;    INIT_y = (h2-h0)/2;
        INIT_width = w0;    INIT_height = h0;

        picture_move = false;//��ֹ�������Զ�����ͼƬλ��
        picture_scale = false;//��ֹ�������Զ�����ͼƬλ��
        picture_open = true;//��ֹ�������Զ�����ͼƬλ��
        checked_Brightness = false;
        checked_Contrast = false;

        QMessageBox::information(this, QStringLiteral("��ѹͼƬ"), QStringLiteral("�ɹ���ѹ����!"));
    }
    else
    {
        //QMessageBox::warning(this, tr("Path"), tr("You did not select any file."));
    }
}
*/

/*
void MainWindow::on_action_File_Zip_triggered()//ѹ��BMPͼƬ�����ΪfpsLZW�ļ�,��File_Save����
{
    QString path = QFileDialog::getSaveFileName(this, QStringLiteral("ѡ��ѹ��ͼƬ�ı���·��"), ".", tr("fpsLZW Files (*.fpsLZW)"));
    if(!path.isEmpty())
    {
        //�����ⲿexeѹ��ͼƬ
        QProcess process(this);
        process.start("cmd_copy.exe");
        process.write ("LZW_BMP_FPS.exe tmp.dat tmp_zip.fpsLZW\n\r");
        process.write ("exit\n\r");
        process.waitForFinished();
        process.close();

        QFile file("tmp_zip.fpsLZW");

        if(file.rename(path))
            QMessageBox::information(this, QStringLiteral("ѹ��ͼƬ"), QStringLiteral("ѹ������ɹ�!"));
        else
            QMessageBox::warning(this, QStringLiteral("ѹ��ͼƬ"), QStringLiteral("ѹ������ʧ��."));
    }
    else
    {
        QMessageBox::information(this, QStringLiteral("ѹ��ͼƬ"), QStringLiteral("δѡ�񱣴�·��."));
    }
}
*/
