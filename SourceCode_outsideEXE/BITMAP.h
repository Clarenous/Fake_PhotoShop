#pragma once
#pragma pack(1)
#include <string>
using namespace std;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER  //位图文件头结构
{
	WORD bfType;  //位图文件的类型，两字节，必须为BM 两字符的ASCII，即0X4D42
	DWORD bfSize;  //位图文件的大小，以字节为单位
	WORD bfReserved1;  //保留字，必须为0
	WORD bfReserved2;  //保留字，必须为0
	DWORD bfOffBits;  //位图数据的起始位置，即相对于位图文件头的偏移量
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER  //位图信息头结构
{
	DWORD biSize;  //本结构所占用的字节数
	LONG biWidth;  //位图的宽度
	LONG biHeight;  //位图的高度
	WORD biPlanes;  //目标设备的级别，必须为1
	WORD biBitCount;  //每个像素所需的位数，一般为1、4、8或24等
	DWORD biCompression;  //压缩类型，一般为0(不压缩)、1(BI_RLE8压缩)、2(BI_RLE4压缩)，本例中假定不压缩
	DWORD biSizeImage;  //位图的大小，以字节为单位
	LONG biXPelsPerMeter;  //位图水平分辨率(每米像素数)
	LONG biYPelsPerMeter;  //位图垂直分辨率(每米像素数)
	DWORD biClrUsed;  //位图实际使用的颜色表中的颜色数
	DWORD biClrImportant;  //位图显示过程中重要的颜色数
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD  //位图颜色表
{
	BYTE rgbBlue;  //蓝色的亮度(范围为0~255)
	BYTE rgbGreen;  //绿色的亮度(范围为0~255)
	BYTE rgbRed;  //红色的亮度(范围为0~255)
	BYTE rgbReserved;  //保留字，必须为0
}RGBQUAD;

class CDib  //即Control Device Independent Bitmap，是操作BMP文件的类
{
public:
	CDib();  //默认构造函数
	~CDib();  //析构函数
	bool Load(string filename);  //打开BMP文件
	bool Save(string filename);  //保存BMP文件
	bool Create(int nWidth, int nHeight, int nColor);  //建立新的BMP结构
	bool GreyProcess();  //灰度处理
	bool Brightness(float a);  //调整图片亮度
	bool Contrast(float a);  //调整图片对比度
	bool FlipHorizontal();  //水平翻转图像
	bool FlipVertical();  //垂直翻转图像
	bool Smooth4();  //4-邻域平滑
	bool Smooth8();  //8-邻域平滑
	bool Convert24to8Bit_Grey(BYTE *p, int w, int h, int n);  //24位图进行8位灰度处理
	bool Convert8toMonochrome(int k);  //将8位灰度BMP文件转化成黑白图像
	bool InverseColor();	//反色处理
	bool Convert24to8Bit(BYTE *p, int w, int h, int n);  //将24位真彩色图像转化为256色图像
	bool ConvertToASCII(string txtName);	//将图像转为ASCII字符阵列
	BYTE *GetBasicInfo(int &w, int &h, int &n);  //获取图像的基本信息
private:
	int GetNumberOfColors();  //获取颜色表的表项数目
	void SetColor(RGBQUAD * rgb, BYTE r, BYTE g, BYTE b);  //设置颜色表项
	BITMAPFILEHEADER m_BitmapFileHeader;  //BMP文件头结构
	BITMAPINFOHEADER * m_pBitmapInfoHeader;  //指向BMP文件信息结构
	RGBQUAD * m_pRgbQuad;  //指向颜色表
	BYTE * m_pData;  //像素阵列
	BYTE * pDib;  //指向BMP文件后三部分数据
};
#pragma pack()