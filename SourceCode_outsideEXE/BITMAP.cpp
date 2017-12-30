#include "BITMAP.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <climits>
#include <string>
#include <chrono>
using namespace std;

CDib::CDib()  //构造函数
{
	pDib = NULL;
}

CDib::~CDib()  //析构函数
{
	if (pDib)
		delete[]pDib;
}

bool CDib::Load(string filename)  //打开BMP文件
{
	ifstream ifs(filename, ios::binary);  //打开文件
	ifs.seekg(0, ios::end);  //文件指针指向文件末尾
	int size = ifs.tellg();  //得到文件大小
	ifs.seekg(0, ios::beg);  //文件指针指向文件头
	ifs.read((char *)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));  //读取位图文件头结构
	if (m_BitmapFileHeader.bfType != 0x4d42)  //判断文件类型是否正确
	{
		throw "文件类型不正确!";
		return false;
	}
	if (size != m_BitmapFileHeader.bfSize)  //判断文件格式是否正确
	{
		throw "文件格式不正确!";
		return false;
	}
	pDib = new BYTE[size - sizeof(BITMAPFILEHEADER)];  //用于存储文件后三部分数据
	if (!pDib)  //若创建动态存储空间不成功
	{
		throw "内存不足!";
		return false;
	}
	ifs.read((char *)pDib, size - sizeof(BITMAPFILEHEADER));  //读取文件后三部分数据
	m_pBitmapInfoHeader = (BITMAPINFOHEADER *)pDib;
	m_pRgbQuad = (RGBQUAD *)(pDib + sizeof(BITMAPINFOHEADER));
	int colorTableSize = m_BitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - m_pBitmapInfoHeader->biSize;
	int numberOfColors = GetNumberOfColors();  //获取颜色数目
	if (numberOfColors * sizeof(RGBQUAD) != colorTableSize)  //校验文件结构
	{
		delete[]pDib;
		pDib = NULL;
		throw "颜色表大小计算错误!";
		return false;
	}
	m_pData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;  //位图图像数据起始位置
	return true;
}

int CDib::GetNumberOfColors()  //获得颜色数目
{
	int numberOfColors = 0;
	if (m_pBitmapInfoHeader->biClrUsed)  //若信息头中定义了颜色表的项目数
		numberOfColors = m_pBitmapInfoHeader->biClrUsed;
	else
	{
		switch (m_pBitmapInfoHeader->biBitCount)
		{
		case 1:numberOfColors = 2; break;  //单色图像
		case 4:numberOfColors = 16; break;  //16色图像
		case 8:numberOfColors = 256;  //256色图像
		}
	}
	return numberOfColors;
}

bool CDib::Save(string filename)  //保存BMP文件
{
	if (!pDib)  //当内存中没有读入图像数据时
		return false;
	ofstream ofs(filename, ios::binary);
	if (ofs.fail())  //读写过程出错或格式错误时
		return false;
	ofs.write((char *)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));  //写入位图文件头结构
	ofs.write((char *)pDib, m_BitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER));  //写入剩余三部分数据
	ofs.close();
	return true;
}

bool CDib::Create(int nWidth, int nHeight, int nColor)  //建立默认BMP文件结构
{
	if (pDib) delete[]pDib;
	//设置颜色表的大小，在这里16色和256色图像在颜色表中均设置8种颜色
	int colorTableSize = 0;
	if (nColor == 1)
		colorTableSize = 2 * sizeof(RGBQUAD);
	else if (nColor == 4)
		colorTableSize = 8 * sizeof(RGBQUAD);  //这里4位图像仅使用8种颜色
	else if (nColor == 8)
		colorTableSize = 256 * sizeof(RGBQUAD);
	else nColor = 24;
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行占用的字节数
	int dataSize = nHeight * bytePerline;  //所有像素所占的字节
	//设置文件头结构
	m_BitmapFileHeader.bfType = 0x4d42;
	m_BitmapFileHeader.bfReserved1 = 0;
	m_BitmapFileHeader.bfReserved2 = 0;
	m_BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
	m_BitmapFileHeader.bfSize = m_BitmapFileHeader.bfOffBits + dataSize;
	//建立存储BMP文件后三部分结构的空间
	pDib = new BYTE[m_BitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER)];
	if (!pDib) return false;
	//设置文件信息头结构
	m_pBitmapInfoHeader = (BITMAPINFOHEADER *)pDib;
	m_pBitmapInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfoHeader->biBitCount = nColor;
	m_pBitmapInfoHeader->biClrImportant = 1;
	m_pBitmapInfoHeader->biClrUsed = colorTableSize / sizeof(RGBQUAD);
	m_pBitmapInfoHeader->biCompression = 0;
	m_pBitmapInfoHeader->biPlanes = 1;
	m_pBitmapInfoHeader->biSizeImage = dataSize;
	m_pBitmapInfoHeader->biXPelsPerMeter = 1024;
	m_pBitmapInfoHeader->biYPelsPerMeter = 1024;
	m_pBitmapInfoHeader->biHeight = nHeight;
	m_pBitmapInfoHeader->biWidth = nWidth;
	//设置像素数据指针
	m_pData = pDib + m_BitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
	//设置颜色表
	m_pRgbQuad = (RGBQUAD *)(pDib + sizeof(BITMAPINFOHEADER));
	switch (nColor)
	{
	case 1:  //单色图像，只有2种颜色
		SetColor(m_pRgbQuad, 0xff, 0xff, 0xff);  //索引值为0的颜色为白色
		SetColor(m_pRgbQuad + 1, 0, 0, 0);  //索引值为1的颜色为黑色
		memset(m_pData, 0x00, dataSize);  //设置图像中每个像素颜色索引值为0
		break;
	case 4:  //这里只定义8种颜色0xffffff,0xffff00,0xff00ff,0xff0000,0x00ffff,0x00ff00,0x0000ff,0x000000
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					SetColor(m_pRgbQuad + 7 - i * 4 - j * 2 - k, i * 255, j * 255, k * 255);
		memset(m_pData, 0x00, dataSize);  //设置图像中每个像素颜色索引值为0
		break;
	case 8:
		for (int i = 0; i < 256; i++)
			SetColor(m_pRgbQuad + i, i, i, i);  //8位图像设置为灰度图
		memset(m_pData, 0xff, dataSize);  //设置8位图像每个像素均为白色
		break;
	default:
		memset(m_pData, 0xff, dataSize);  //设置真彩色图像每个像素均为白色
	}
	return true;
}

void CDib::SetColor(RGBQUAD * rgb, BYTE r, BYTE g, BYTE b)  //设置颜色表项
{
	if (rgb)
	{
		rgb->rgbRed = r;
		rgb->rgbGreen = g;
		rgb->rgbBlue = b;
		rgb->rgbReserved = 0;
	}
	else throw"颜色表项不存在!";
}

inline void SetGrey(BYTE *rgb)  //将24位的一个像素转为灰度
{
	BYTE grey = (*rgb * 11 + *(rgb + 1) * 59 + *(rgb + 2) * 30) / 100;  //灰度值,用整形数据处理提高计算效率
	*rgb = *(rgb + 1) = *(rgb + 2) = grey;  //调整RGB值为灰度值
}

bool CDib::GreyProcess()
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nColor==24)  //当图像是24位色时
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //逐行扫描
			for (int j = 0; j < nWidth; j++)  //逐个扫描单行的像素
				SetGrey(m_pData + (i*bytePerline + 3 * j));  //将该像素处理为灰度
	else  //其余颜色深度的图像不予处理
		return false;
	return true;
}

inline void SetBrightness(BYTE *rgb, float a)  //调整像素点亮度
{
	*rgb = pow(*rgb, a)*pow(255, 1 - a);  //调整B 值
	*(rgb + 1) = pow(*(rgb + 1), a)*pow(255, 1 - a);  //调整G 值
	*(rgb + 2) = pow(*(rgb + 2), a)*pow(255, 1 - a);  //调整R 值
}

bool CDib::Brightness(float a)  //调整图片亮度
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nColor == 24)  //当图像是24位色时
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //逐行扫描
			for (int j = 0; j < nWidth; j++)  //逐个扫描单行的像素
				SetBrightness(m_pData + (i*bytePerline + 3 * j), a);  //调节该像素亮度
	else  //其余颜色深度的图像不予处理
		return false;
	return true;
}

inline BYTE BoundaryTreat(float p)  //对比度调节边界处理
{
	if (p > 255)
		return 255;
	else if (p >= 0)
		return (BYTE)p;
	else
		return 0;
}

inline void SetContrast(BYTE *rgb, float a)  //调节像素对比度
{
	*rgb = BoundaryTreat(128 + (*rgb - 128)*(1 + a));  //调整B 值
	*(rgb + 1) = BoundaryTreat(128 + (*(rgb + 1) - 128)*(1 + a));  //调整G 值
	*(rgb + 2) = BoundaryTreat(128 + (*(rgb + 2) - 128)*(1 + a));  //调整G 值
}

bool CDib::Contrast(float a)  //调整图片对比度
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nColor == 24)  //当图像是24位色时
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //逐行扫描
			for (int j = 0; j < nWidth; j++)  //逐个扫描单行的像素
				SetContrast(m_pData + (i*bytePerline + 3 * j), a);  //调节该像素对比度
	else  //其余颜色深度的图像不予处理
		return false;
	return true;
}

inline void SwapBYTE(BYTE *p, BYTE *q)  //交换两个BYTE 类型的值
{
	BYTE t;
	t = *p;  *p = *q;  *q = t;
}

inline void SwapPixel(BYTE *p, BYTE *q)  //交换两像素
{
	SwapBYTE(p, q);  //交换B 值
	SwapBYTE(p + 1, q + 1);  //交换G 值
	SwapBYTE(p + 2, q + 2);  //交换R 值
}

bool CDib::FlipHorizontal()  //水平翻转图像
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nColor == 24)  //当图像是24位色时
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //逐行扫描
			for (int j = 0; j < nWidth / 2 + 1; j++)  //逐个扫描单行的前半行像素
				SwapPixel(m_pData + (i*bytePerline + 3 * j), m_pData + (i*bytePerline + (nWidth - 1 - j) * 3));  //交换两对称位置的像素
	else if (nColor == 8)	//当图像是8位色时
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //逐行扫描
			for (int j = 0; j < nWidth / 2 + 1; j++)  //逐个扫描单行的前半行像素
				SwapBYTE(m_pData + (i*bytePerline + j), m_pData + (i*bytePerline + (nWidth - 1 - j)));  //交换两对称位置的像素
	else  //其余颜色深度的图像不予处理
		return false;
	return true;
}

bool CDib::FlipVertical()  //垂直翻转图像
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nHeight = m_pBitmapInfoHeader->biHeight;  //图像高度(每列像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nColor == 24)  //当图像是24位色时
		for (int i = 0; i < nWidth; i++)  //逐列扫描
			for (int j = 0; j < nHeight / 2 + 1; j++)  //逐个扫描单列的前半列像素
				SwapPixel(m_pData + (j*bytePerline + 3 * i), m_pData + ((nHeight - j - 1)*bytePerline + 3 * i));  //交换两对称位置的像素
	else if (nColor == 8)
		for (int i = 0; i < nWidth; i++)  //逐列扫描
			for (int j = 0; j < nHeight / 2 + 1; j++)  //逐个扫描单列的前半列像素
				SwapBYTE(m_pData + (j*bytePerline + i), m_pData + ((nHeight - j - 1)*bytePerline + i));  //交换两对称位置的像素
	else  //其余颜色深度的图像不予处理
		return false;
	return true;
}

void Smooth4_8bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //8位色图像的4-邻域平滑
{
	/*对四个角进行平滑*/
	int pixel = 0;  //矩阵左上角，实际为图像左下角
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel + bytePerline]) / 3;
	pixel = nWidth - 1;  //矩阵右上角，实际为图像右下角像素
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel + bytePerline]) / 3;
	pixel = bytePerline * (nHeight - 1);  //矩阵左下角，实际为图像左上角元素
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline]) / 3;
	pixel = bytePerline * (nHeight - 1) + nWidth - 1;  //矩阵右下角，实际为图像右上角元素
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel - bytePerline]) / 3;
	/*对上下左右四个边界进行平滑*/
	int i, last;
	last = nWidth - 1;
	for (i = 1; i < last; i++)  //矩阵上边界，实际为图像下边界
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i + bytePerline]) / 4;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = bytePerline * (nHeight - 1) + 1; i < last; i++)  //矩阵下边界，实际为图像的上边界
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline]) / 4;
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //矩阵左边界，实际为图像从下至上的左边界
		pData[i] = (pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline]) / 4;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = nWidth - 1 + bytePerline; i < last; i += bytePerline)  //矩阵右边界，实际为图像从下至上的右边界
		pData[i] = (pDataCopy[i] + pDataCopy[i - 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline]) / 4;
	/*对其他像素平滑*/
	for (i = 1; i < nHeight - 1; i++)  //逐行处理
		for (int j = 1; j < nWidth - 1; j++)  //依次处理单行的每个像素
		{
			pixel = i * bytePerline + j;
			pData[pixel] = (pDataCopy[pixel - 1] + pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel + bytePerline]) / 5;
		}
}

inline void GetAverage_3pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* p)
{
	*p = (*a + *b + *c) / 3;  //求B 值平均值
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1)) / 3;  //求G 值平均值
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2)) / 3;  //求R 值平均值
}

inline void GetAverage_4pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* p)
{
	*p = (*a + *b + *c + *d) / 4;  //求B 值平均值
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1)) / 4;  //求G 值平均值
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2)) / 4;  //求R 值平均值
}

inline void GetAverage_5pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* e, BYTE* p)
{
	*p = (*a + *b + *c + *d + *e) / 5;  //求B 值平均值
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1) + *(e + 1)) / 5;  //求G 值平均值
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2) + *(e + 2)) / 5;  //求R 值平均值
}

void Smooth4_24bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //24位色图像的4-邻域平滑
{
	/*对四个角进行平滑*/
	int pixel = 0;  //矩阵左上角，实际为图像左下角
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel + bytePerline, pData + pixel);
	pixel = nWidth * 3 - 3;  //矩阵右上角，实际为图像右下角像素
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel + bytePerline, pData + pixel);
	pixel = bytePerline * (nHeight - 1);  //矩阵左下角，实际为图像左上角元素
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pData + pixel);
	pixel = bytePerline * (nHeight - 1) + nWidth * 3 - 3;  //矩阵右下角，实际为图像右上角元素
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel - bytePerline, pData + pixel);
	/*对上下左右四个边界进行平滑*/
	int i, last;
	last = nWidth * 3 - 3;
	for (i = 3; i < last; i += 3)  //矩阵上边界，实际为图像下边界
		GetAverage_4pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i + bytePerline, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = bytePerline * (nHeight - 1) + 3; i < last; i += 3)  //矩阵下边界，实际为图像的上边界
		GetAverage_4pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pData + i);
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //矩阵左边界，实际为图像从下至上的左边界
		GetAverage_4pixel(pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = nWidth * 3 - 3 + bytePerline; i < last; i += bytePerline)  //矩阵右边界，实际为图像从下至上的右边界
		GetAverage_4pixel(pDataCopy + i, pDataCopy + i - 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pData + i);
	/*对其他像素平滑*/
	for (i = 1; i < nHeight - 1; i++)  //逐行处理
		for (int j = 1; j < nWidth - 1; j++)  //依次处理单行的每个像素
		{
			pixel = i * bytePerline + j * 3;
			GetAverage_5pixel(pDataCopy + pixel - 3, pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel + bytePerline, pData + pixel);
		}
}

bool CDib::Smooth4()  //4-邻域平滑
{
	int nWidth = m_pBitmapInfoHeader->biWidth;
	int nHeight = m_pBitmapInfoHeader->biHeight;
	int nColor = m_pBitmapInfoHeader->biBitCount;
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nWidth < 2 || nHeight < 2) return false;  //图像尺寸过小
	if (!m_pData) return false;  //读取数据失败
	int DataSize = bytePerline*nHeight;  //像素矩阵大小
	BYTE *pDataCopy = new BYTE[DataSize];  //新建空像素矩阵
	if (pDataCopy == NULL)  //判断堆空间是否成功分配
	{
		cout << "堆空间内存分配出错！" << endl;
		return false;
	}
	for (int i = 0; i < DataSize; i++)  //拷贝数据至副本
		pDataCopy[i] = m_pData[i];
	switch (nColor)
	{
	case 8:
		Smooth4_8bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //处理8位色图像
		break;
	case 24:
		Smooth4_24bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //处理24位色图像
		break;
	default:
		cout << "无法处理颜色深度为" << nColor << "位的图像!" << endl;
		return false;
		break;
	}
	delete[]pDataCopy;  //释放空间
	pDataCopy = NULL;
	return true;
}

void Smooth8_8bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //8位色图像的8-邻域平滑
{
	/*对四个角进行平滑*/
	int pixel = 0;  //矩阵左上角，实际为图像左下角
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel + bytePerline] + pDataCopy[pixel + bytePerline + 1]) / 4;
	pixel = nWidth - 1;  //矩阵右上角，实际为图像右下角像素
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel + bytePerline] + pDataCopy[pixel + bytePerline - 1]) / 4;
	pixel = bytePerline * (nHeight - 1);  //矩阵左下角，实际为图像左上角元素
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel - bytePerline + 1]) / 4;
	pixel = bytePerline * (nHeight - 1) + nWidth - 1;  //矩阵右下角，实际为图像右上角元素
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel - bytePerline - 1]) / 4;
	/*对上下左右四个边界进行平滑*/
	int i, last;
	last = nWidth - 1;
	for (i = 1; i < last; i++)  //矩阵上边界，实际为图像下边界
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i + bytePerline] + pDataCopy[i + bytePerline - 1] + pDataCopy[i + bytePerline + 1]) / 6;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = bytePerline * (nHeight - 1) + 1; i < last; i++)  //矩阵下边界，实际为图像的上边界
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline] + pDataCopy[i - bytePerline - 1] + pDataCopy[i - bytePerline + 1]) / 6;
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //矩阵左边界，实际为图像从下至上的左边界
		pData[i] = (pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline] + pDataCopy[i - bytePerline + 1] + pDataCopy[i + bytePerline + 1]) / 6;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = nWidth - 1 + bytePerline; i < last; i += bytePerline)  //矩阵右边界，实际为图像从下至上的右边界
		pData[i] = (pDataCopy[i] + pDataCopy[i - 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline] + pDataCopy[i - bytePerline - 1] + pDataCopy[i + bytePerline - 1]) / 6;
	/*对其他像素平滑*/
	for (i = 1; i < nHeight - 1; i++)  //逐行处理
		for (int j = 1; j < nWidth - 1; j++)  //依次处理单行的每个像素
		{
			pixel = i * bytePerline + j;
			pData[pixel] = (pDataCopy[pixel - 1] + pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel + bytePerline]+pDataCopy[pixel - bytePerline-1] + pDataCopy[pixel - bytePerline + 1] + pDataCopy[pixel + bytePerline - 1] + pDataCopy[pixel + bytePerline + 1]) / 9;
		}
}

inline void GetAverage_6pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* e, BYTE* f,BYTE* p)
{
	*p = (*a + *b + *c + *d + *e + *f) / 6;  //求B 值平均值
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1) + *(e + 1) + *(f + 1)) / 6;  //求G 值平均值
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2) + *(e + 2) + *(f + 2)) / 6;  //求R 值平均值
}

inline void GetAverage_9pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* e, BYTE* f, BYTE* g, BYTE* h, BYTE* i, BYTE* p)
{
	*p = (*a + *b + *c + *d + *e + *f + *g + *h + *i) / 9;  //求B 值平均值
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1) + *(e + 1) + *(f + 1) + *(g + 1) + *(h + 1) + *(i + 1)) / 9;  //求G 值平均值
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2) + *(e + 2) + *(f + 2) + *(g + 2) + *(h + 2) + *(i + 2)) / 9;  //求R 值平均值
}

void Smooth8_24bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //24位色图像的8-邻域平滑
{
	/*对四个角进行平滑*/
	int pixel = 0;  //矩阵左上角，实际为图像左下角
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel + bytePerline, pDataCopy + pixel + bytePerline + 3, pData + pixel);
	pixel = nWidth * 3 - 3;  //矩阵右上角，实际为图像右下角像素
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel + bytePerline, pDataCopy + pixel + bytePerline - 3, pData + pixel);
	pixel = bytePerline * (nHeight - 1);  //矩阵左下角，实际为图像左上角元素
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel - bytePerline + 3, pData + pixel);
	pixel = bytePerline * (nHeight - 1) + nWidth * 3 - 3;  //矩阵右下角，实际为图像右上角元素
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel - bytePerline - 3, pData + pixel);
	/*对上下左右四个边界进行平滑*/
	int i, last;
	last = nWidth * 3 - 3;
	for (i = 3; i < last; i += 3)  //矩阵上边界，实际为图像下边界
		GetAverage_6pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i + bytePerline, pDataCopy + i + bytePerline - 3, pDataCopy + i + bytePerline + 3, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = bytePerline * (nHeight - 1) + 3; i < last; i += 3)  //矩阵下边界，实际为图像的上边界
		GetAverage_6pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pDataCopy + i - bytePerline - 3, pDataCopy + i - bytePerline + 3, pData + i);
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //矩阵左边界，实际为图像从下至上的左边界
		GetAverage_6pixel(pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pDataCopy + i - bytePerline + 3, pDataCopy + i + bytePerline + 3, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = nWidth * 3 - 3 + bytePerline; i < last; i += bytePerline)  //矩阵右边界，实际为图像从下至上的右边界
		GetAverage_6pixel(pDataCopy + i, pDataCopy + i - 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pDataCopy + i - bytePerline - 3, pDataCopy + i + bytePerline - 3, pData + i);
	/*对其他像素平滑*/
	for (i = 1; i < nHeight - 1; i++)  //逐行处理
		for (int j = 1; j < nWidth - 1; j++)  //依次处理单行的每个像素
		{
			pixel = i * bytePerline + j * 3;
			GetAverage_9pixel(pDataCopy + pixel - 3, pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel + bytePerline, pDataCopy + pixel - bytePerline - 3, pDataCopy + pixel - bytePerline + 3, pDataCopy + pixel + bytePerline - 3, pDataCopy + pixel + bytePerline + 3, pData + pixel);
		}
}

bool CDib::Smooth8()  //8-邻域平滑
{
	int nWidth = m_pBitmapInfoHeader->biWidth;
	int nHeight = m_pBitmapInfoHeader->biHeight;
	int nColor = m_pBitmapInfoHeader->biBitCount;
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nWidth < 2 || nHeight < 2) return false;  //图像尺寸过小
	if (!m_pData) return false;  //读取数据失败
	int DataSize = bytePerline*nHeight;  //像素矩阵大小
	BYTE *pDataCopy = new BYTE[DataSize];  //新建空像素矩阵
	if (pDataCopy == NULL)  //判断堆空间是否成功分配
	{
		cout << "堆空间内存分配出错！" << endl;
		return false;
	}
	for (int i = 0; i < DataSize; i++)  //拷贝数据至副本
		pDataCopy[i] = m_pData[i];
	switch (nColor)
	{
	case 8:
		Smooth8_8bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //处理8位色图像
		break;
	case 24:
		Smooth8_24bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //处理24位色图像
		break;
	default:
		cout << "无法处理颜色深度为" << nColor << "位的图像!" << endl;
		return false;
		break;
	}
	delete[]pDataCopy;  //释放空间
	pDataCopy = NULL;
	return true;
}

bool CDib::Convert24to8Bit_Grey(BYTE *p, int w, int h, int n)  //将24位图进行8位灰度处理
{
	if (n != 24) return false;
	int nColor = 8;  //8位图像颜色深度为8
	int bytePerline = ((w*n + 31) / 32) * 4;  //原图像每行字节数
	int New_bytePerline = ((w*nColor + 31) / 32) * 4;  //新图像每行字节数
	/*遍历一遍像素矩阵以拷贝灰度数据*/
	for (int i = 0; i < h; i++)  //按行遍历
		for (int j = 0; j < w; j++)  //遍历单行的每个像素
			m_pData[New_bytePerline*i + j] = p[bytePerline*i + 3 * j];
	return true;
}

BYTE* CDib::GetBasicInfo(int &w, int &h, int &n)  //获取图像基本信息
{
	w = m_pBitmapInfoHeader->biWidth;
	h = m_pBitmapInfoHeader->biHeight;
	n = m_pBitmapInfoHeader->biBitCount;
	return m_pData;
}

bool CDib::Convert8toMonochrome(int k)  //将8位灰度BMP文件转化成黑白图像
{
	//BMP的像素数目较多，所以直接改变颜色表即可
	int colorTableSize = m_BitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - m_pBitmapInfoHeader->biSize;
	int nColor = colorTableSize / sizeof(RGBQUAD);
	for (int i = 0; i < nColor; i++)
	{
		if (m_pRgbQuad[i].rgbRed < k)
			SetColor(m_pRgbQuad + i, 0, 0, 0);
		else
			SetColor(m_pRgbQuad + i, 255, 255, 255);
	}
	return true;
}

struct DNode
{
	int data;
	struct DNode *prior;
	struct DNode *next;
};

bool CDib::Convert24to8Bit(BYTE *p, int w, int h, int n)  //将24位真彩色图像转化为256色图像
{
	if (n != 24) return false;
	int nColor = 8;  //8位图像颜色深度为8
	int bytePerline = ((w*n + 31) / 32) * 4;  //原图像每行字节数
	int New_bytePerline = ((w*nColor + 31) / 32) * 4;  //新图像每行字节数
	//遍历原像素矩阵统计256个流行色
	int *colorCount = new int[16777218];  //多分配两个位置供栈顶、栈底使用
	for (int i = 0; i < 16777216; i++)  //初始化
		colorCount[i] = 0;
	if (!colorCount)
	{
		cout << "图像统计内存分配失败!" << endl;
		return false;
	}
	//初始化用于记录流行色的栈
	auto start01 = std::chrono::high_resolution_clock::now();  //算法开始时间
	DNode *top = new DNode;  //栈的栈顶结点
	DNode *bottom = new DNode;  //栈的栈底结点
	top->prior = bottom->next = NULL;
	top->next = bottom;
	bottom->prior = top;
	top->data = 16777216;
	colorCount[16777216] = 0;  //栈顶元素对应的次数最小
	bottom->data = 16777217;
	colorCount[16777217] = INT_MAX;  //栈底元素对应的次数最大
	int Ssize = 0;  //栈的长度
	int loc;  //索引位置
	int min_count = 0;  //栈中出现次数最少的流行色的次数
	for (int i = 0; i < h; i++)  //按行遍历
		for (int j = 0; j < w; j++)  //遍历单行的每个像素
		{
			loc = p[bytePerline*i + 3 * j] * 65536 + p[bytePerline*i + 3 * j + 1] * 256 + p[bytePerline*i + 3 * j + 2];
			colorCount[loc]++;
			if (colorCount[loc] > min_count || !(Ssize == 256))  //某颜色符合入栈条件时
			{
				DNode *p = new DNode;
				DNode *q = top;
				p->data = loc;
				while (q)  //在适当位置插入该颜色索引
				{
					if (colorCount[loc] <= colorCount[q->data])
					{
						if (loc == q->data)  //该颜色已存在于链表中时
						{
							//先摘链
							q->prior->next = q->next;
							q->next->prior = q->prior;
							q = q->next;
							while (q)  //在合适位置插入结点
							{
								if (colorCount[loc] <= colorCount[q->data])
								{
									//在q的前一位置插入，此时栈的长度不变
									p->next = q;
									p->prior = q->prior;
									p->prior->next = p;
									q->prior = p;
									break;
								}
								q = q->next;
							}
							break;
						}
						else
						{
							//在q的前一位置插入p
							p->next = q;
							p->prior = q->prior;
							p->prior->next = p;
							q->prior = p;
							Ssize++;
							break;
						}
					}
					q = q->next;
				}
				if (Ssize > 256)  //若栈已有256个颜色，则栈顶元素出栈
				{
					q = top->next;
					top->next = q->next;
					q->next->prior = top;
					delete q;
					Ssize--;
				}
				min_count = colorCount[top->next->data];  //更新min_count
			}
		}
	//将选出的流行色写入颜色表，按频度从高到低存放，提高接下来写入新像素时的速度
	int t = 0;
	DNode *popular = bottom->prior;
	while (t < 256 && popular->prior)
	{
		(m_pRgbQuad + t)->rgbBlue = (popular->data & 0xff0000) / 65536;
		(m_pRgbQuad + t)->rgbGreen = (popular->data & 0x00ff00) / 256;
		(m_pRgbQuad + t)->rgbRed = popular->data & 0x0000ff;
		t++;
		popular = popular->prior;
	}
	//释放栈空间
	popular = top;
	while (popular->next)  //此循环中未释放栈底
	{
		popular = popular->next;
		delete popular->prior;
	}
	delete bottom;
	top = bottom = popular = NULL;
	auto elapsed01 = std::chrono::high_resolution_clock::now() - start01;  //算法运行时间
	
	/*//方法二：简单选择排序
	auto start01 = std::chrono::high_resolution_clock::now();  //算法开始时间
	for (int i = 0; i < h; i++)  //按行遍历
		for (int j = 0; j < w; j++)  //遍历单行的每个像素
			colorCount[p[bytePerline*i + 3 * j] * 65536 + p[bytePerline*i + 3 * j + 1] * 256 + p[bytePerline*i + 3 * j + 2]]++;
	int popularColor[256] = { 0 };
	int index_popular = 0;
	for (int i = 0; i < 256; i++)
	{
		for (int j = i; j < 16777216; j++)
		{
			if (colorCount[index_popular] < colorCount[j])
				index_popular = j;
		}
		popularColor[i] = index_popular;
		colorCount[index_popular] = 0;
		index_popular = i + 1;
	}
	for (int i = 0; i < 256; i++)
	{
		(m_pRgbQuad + i)->rgbBlue = (popularColor[i] & 0xff0000) / 65536;
		(m_pRgbQuad + i)->rgbGreen = (popularColor[i] & 0x00ff00) / 256;
		(m_pRgbQuad + i)->rgbRed = popularColor[i] & 0x0000ff;
	}
	auto elapsed01 = std::chrono::high_resolution_clock::now() - start01;  //算法运行时间
	*/
	//释放colorCount
	delete[]colorCount;
	colorCount = NULL;
	//建立颜色表的灰度索引表
	int rgbQuad_grey[257] = { 25500 };  //预留一个位置方便存放临时数据,默认为白色
	for (int i = 0; i < 256; i++)
		rgbQuad_grey[i] = m_pRgbQuad[i].rgbBlue * 11 + m_pRgbQuad[i].rgbGreen * 59 + m_pRgbQuad[i].rgbRed * 30;
	//遍历原图像像素矩阵，按灰度值相近程度存储256色图像的像素矩阵
	for (int i = 0; i < h; i++)  //按行遍历
		for (int j = 0; j < w; j++)  //遍历单行的每个像素
		{
			int pixel_grey = p[bytePerline*i + 3 * j] * 11 + p[bytePerline*i + 3 * j + 1] * 59 + p[bytePerline*i + 3 * j + 2] * 30;  //像素点的灰度值
			rgbQuad_grey[256] = abs(pixel_grey - rgbQuad_grey[0]);
			int index = 0;
			for (int k = 1; k < 256; k++)
				if (rgbQuad_grey[256] > abs(rgbQuad_grey[k] - pixel_grey))
				{
					rgbQuad_grey[256] = abs(rgbQuad_grey[k] - pixel_grey);
					index = k;
				}
			m_pData[New_bytePerline*i + j] = index;
		}
	/*//方法二：遍历原图像像素矩阵，按RGB值总差值相近程度存储256色图像的像素矩阵(速度慢，效果无差别)
	for (int i = 0; i < h; i++)  //按行遍历
		for (int j = 0; j < w; j++)  //遍历单行的每个像素
		{
			int pixel_delta = abs(p[bytePerline*i + 3 * j] - m_pRgbQuad[0].rgbBlue) + abs(p[bytePerline*i + 3 * j + 1] - m_pRgbQuad[0].rgbGreen) + abs(p[bytePerline*i + 3 * j + 2] - m_pRgbQuad[0].rgbRed);
			int index = 0;
			for (int k = 1; k < 256; k++)
			{
				int temp = abs(p[bytePerline*i + 3 * j] - m_pRgbQuad[k].rgbBlue) + abs(p[bytePerline*i + 3 * j + 1] - m_pRgbQuad[k].rgbGreen) + abs(p[bytePerline*i + 3 * j + 2] - m_pRgbQuad[k].rgbRed);
				if (pixel_delta > temp)
				{
					pixel_delta = temp;
					index = k;
				}
			}
			m_pData[New_bytePerline*i + j] = index;
		}*/
	long time01 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed01).count();
	cout << "(" << time01 << "μs)" << endl;
	return true;
}

bool CDib::ConvertToASCII(string txtName)	//将图片转为ASCII码的txt文件
{
	char rank[16] = {'M','N','H','Q','$','O','C','?','7','>','!',':','–',';','.',' '};	//16个等级的ascii码表
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nHeight = m_pBitmapInfoHeader->biHeight;  //图像高度(每列像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	ofstream ofile(txtName);
	if (nColor == 8)  //当图像是8位灰度图时
		for (int i = m_pBitmapInfoHeader->biHeight - 1; i >= 0; i--)  //逐行扫描,从下向上，符合图片存储顺序
		{
			for (int j = 0; j < nWidth; j++)  //逐个扫描单行的像素
				ofile << rank[m_pRgbQuad[m_pData[i*bytePerline + j]].rgbBlue / 16] << rank[m_pRgbQuad[m_pData[i*bytePerline + j]].rgbBlue / 16];
			ofile << endl;
		}
	else  //其余颜色深度的图像不予处理
		return false;
	ofile.close();
	return true;
}

inline void SetInverseColor(BYTE *rgb)  //调整像素点亮度
{
	*rgb = 255 - *rgb;  //调整B 值
	*(rgb + 1) = 255 - *(rgb + 1);  //调整G 值
	*(rgb + 2) = 255 - *(rgb + 2);  //调整R 值
}

bool CDib::InverseColor()	//反色处理
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //图像宽度(每行像素数)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //颜色深度
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //每行像素所占字节数
	if (nColor == 24)  //当图像是24位色时
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //逐行扫描
			for (int j = 0; j < nWidth; j++)  //逐个扫描单行的像素
				SetInverseColor(m_pData + (i*bytePerline + 3 * j));  //调节该像素亮度
	else if (nColor == 8)
		for (int i = 0; i < nColor; i++)
			SetColor(m_pRgbQuad + i, 255 - m_pRgbQuad[i].rgbBlue, 255 - m_pRgbQuad[i].rgbGreen, 255 - m_pRgbQuad[i].rgbRed);
	else  //其余颜色深度的图像不予处理
		return false;
	return true;
}