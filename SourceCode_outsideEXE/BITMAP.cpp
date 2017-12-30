#include "BITMAP.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <climits>
#include <string>
#include <chrono>
using namespace std;

CDib::CDib()  //���캯��
{
	pDib = NULL;
}

CDib::~CDib()  //��������
{
	if (pDib)
		delete[]pDib;
}

bool CDib::Load(string filename)  //��BMP�ļ�
{
	ifstream ifs(filename, ios::binary);  //���ļ�
	ifs.seekg(0, ios::end);  //�ļ�ָ��ָ���ļ�ĩβ
	int size = ifs.tellg();  //�õ��ļ���С
	ifs.seekg(0, ios::beg);  //�ļ�ָ��ָ���ļ�ͷ
	ifs.read((char *)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));  //��ȡλͼ�ļ�ͷ�ṹ
	if (m_BitmapFileHeader.bfType != 0x4d42)  //�ж��ļ������Ƿ���ȷ
	{
		throw "�ļ����Ͳ���ȷ!";
		return false;
	}
	if (size != m_BitmapFileHeader.bfSize)  //�ж��ļ���ʽ�Ƿ���ȷ
	{
		throw "�ļ���ʽ����ȷ!";
		return false;
	}
	pDib = new BYTE[size - sizeof(BITMAPFILEHEADER)];  //���ڴ洢�ļ�������������
	if (!pDib)  //��������̬�洢�ռ䲻�ɹ�
	{
		throw "�ڴ治��!";
		return false;
	}
	ifs.read((char *)pDib, size - sizeof(BITMAPFILEHEADER));  //��ȡ�ļ�������������
	m_pBitmapInfoHeader = (BITMAPINFOHEADER *)pDib;
	m_pRgbQuad = (RGBQUAD *)(pDib + sizeof(BITMAPINFOHEADER));
	int colorTableSize = m_BitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - m_pBitmapInfoHeader->biSize;
	int numberOfColors = GetNumberOfColors();  //��ȡ��ɫ��Ŀ
	if (numberOfColors * sizeof(RGBQUAD) != colorTableSize)  //У���ļ��ṹ
	{
		delete[]pDib;
		pDib = NULL;
		throw "��ɫ���С�������!";
		return false;
	}
	m_pData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;  //λͼͼ��������ʼλ��
	return true;
}

int CDib::GetNumberOfColors()  //�����ɫ��Ŀ
{
	int numberOfColors = 0;
	if (m_pBitmapInfoHeader->biClrUsed)  //����Ϣͷ�ж�������ɫ�����Ŀ��
		numberOfColors = m_pBitmapInfoHeader->biClrUsed;
	else
	{
		switch (m_pBitmapInfoHeader->biBitCount)
		{
		case 1:numberOfColors = 2; break;  //��ɫͼ��
		case 4:numberOfColors = 16; break;  //16ɫͼ��
		case 8:numberOfColors = 256;  //256ɫͼ��
		}
	}
	return numberOfColors;
}

bool CDib::Save(string filename)  //����BMP�ļ�
{
	if (!pDib)  //���ڴ���û�ж���ͼ������ʱ
		return false;
	ofstream ofs(filename, ios::binary);
	if (ofs.fail())  //��д���̳�����ʽ����ʱ
		return false;
	ofs.write((char *)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));  //д��λͼ�ļ�ͷ�ṹ
	ofs.write((char *)pDib, m_BitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER));  //д��ʣ������������
	ofs.close();
	return true;
}

bool CDib::Create(int nWidth, int nHeight, int nColor)  //����Ĭ��BMP�ļ��ṹ
{
	if (pDib) delete[]pDib;
	//������ɫ��Ĵ�С��������16ɫ��256ɫͼ������ɫ���о�����8����ɫ
	int colorTableSize = 0;
	if (nColor == 1)
		colorTableSize = 2 * sizeof(RGBQUAD);
	else if (nColor == 4)
		colorTableSize = 8 * sizeof(RGBQUAD);  //����4λͼ���ʹ��8����ɫ
	else if (nColor == 8)
		colorTableSize = 256 * sizeof(RGBQUAD);
	else nColor = 24;
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��ռ�õ��ֽ���
	int dataSize = nHeight * bytePerline;  //����������ռ���ֽ�
	//�����ļ�ͷ�ṹ
	m_BitmapFileHeader.bfType = 0x4d42;
	m_BitmapFileHeader.bfReserved1 = 0;
	m_BitmapFileHeader.bfReserved2 = 0;
	m_BitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
	m_BitmapFileHeader.bfSize = m_BitmapFileHeader.bfOffBits + dataSize;
	//�����洢BMP�ļ��������ֽṹ�Ŀռ�
	pDib = new BYTE[m_BitmapFileHeader.bfSize - sizeof(BITMAPFILEHEADER)];
	if (!pDib) return false;
	//�����ļ���Ϣͷ�ṹ
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
	//������������ָ��
	m_pData = pDib + m_BitmapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
	//������ɫ��
	m_pRgbQuad = (RGBQUAD *)(pDib + sizeof(BITMAPINFOHEADER));
	switch (nColor)
	{
	case 1:  //��ɫͼ��ֻ��2����ɫ
		SetColor(m_pRgbQuad, 0xff, 0xff, 0xff);  //����ֵΪ0����ɫΪ��ɫ
		SetColor(m_pRgbQuad + 1, 0, 0, 0);  //����ֵΪ1����ɫΪ��ɫ
		memset(m_pData, 0x00, dataSize);  //����ͼ����ÿ��������ɫ����ֵΪ0
		break;
	case 4:  //����ֻ����8����ɫ0xffffff,0xffff00,0xff00ff,0xff0000,0x00ffff,0x00ff00,0x0000ff,0x000000
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				for (int k = 0; k < 2; k++)
					SetColor(m_pRgbQuad + 7 - i * 4 - j * 2 - k, i * 255, j * 255, k * 255);
		memset(m_pData, 0x00, dataSize);  //����ͼ����ÿ��������ɫ����ֵΪ0
		break;
	case 8:
		for (int i = 0; i < 256; i++)
			SetColor(m_pRgbQuad + i, i, i, i);  //8λͼ������Ϊ�Ҷ�ͼ
		memset(m_pData, 0xff, dataSize);  //����8λͼ��ÿ�����ؾ�Ϊ��ɫ
		break;
	default:
		memset(m_pData, 0xff, dataSize);  //�������ɫͼ��ÿ�����ؾ�Ϊ��ɫ
	}
	return true;
}

void CDib::SetColor(RGBQUAD * rgb, BYTE r, BYTE g, BYTE b)  //������ɫ����
{
	if (rgb)
	{
		rgb->rgbRed = r;
		rgb->rgbGreen = g;
		rgb->rgbBlue = b;
		rgb->rgbReserved = 0;
	}
	else throw"��ɫ�������!";
}

inline void SetGrey(BYTE *rgb)  //��24λ��һ������תΪ�Ҷ�
{
	BYTE grey = (*rgb * 11 + *(rgb + 1) * 59 + *(rgb + 2) * 30) / 100;  //�Ҷ�ֵ,���������ݴ�����߼���Ч��
	*rgb = *(rgb + 1) = *(rgb + 2) = grey;  //����RGBֵΪ�Ҷ�ֵ
}

bool CDib::GreyProcess()
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nColor==24)  //��ͼ����24λɫʱ
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //����ɨ��
			for (int j = 0; j < nWidth; j++)  //���ɨ�赥�е�����
				SetGrey(m_pData + (i*bytePerline + 3 * j));  //�������ش���Ϊ�Ҷ�
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	return true;
}

inline void SetBrightness(BYTE *rgb, float a)  //�������ص�����
{
	*rgb = pow(*rgb, a)*pow(255, 1 - a);  //����B ֵ
	*(rgb + 1) = pow(*(rgb + 1), a)*pow(255, 1 - a);  //����G ֵ
	*(rgb + 2) = pow(*(rgb + 2), a)*pow(255, 1 - a);  //����R ֵ
}

bool CDib::Brightness(float a)  //����ͼƬ����
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nColor == 24)  //��ͼ����24λɫʱ
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //����ɨ��
			for (int j = 0; j < nWidth; j++)  //���ɨ�赥�е�����
				SetBrightness(m_pData + (i*bytePerline + 3 * j), a);  //���ڸ���������
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	return true;
}

inline BYTE BoundaryTreat(float p)  //�Աȶȵ��ڱ߽紦��
{
	if (p > 255)
		return 255;
	else if (p >= 0)
		return (BYTE)p;
	else
		return 0;
}

inline void SetContrast(BYTE *rgb, float a)  //�������ضԱȶ�
{
	*rgb = BoundaryTreat(128 + (*rgb - 128)*(1 + a));  //����B ֵ
	*(rgb + 1) = BoundaryTreat(128 + (*(rgb + 1) - 128)*(1 + a));  //����G ֵ
	*(rgb + 2) = BoundaryTreat(128 + (*(rgb + 2) - 128)*(1 + a));  //����G ֵ
}

bool CDib::Contrast(float a)  //����ͼƬ�Աȶ�
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nColor == 24)  //��ͼ����24λɫʱ
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //����ɨ��
			for (int j = 0; j < nWidth; j++)  //���ɨ�赥�е�����
				SetContrast(m_pData + (i*bytePerline + 3 * j), a);  //���ڸ����ضԱȶ�
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	return true;
}

inline void SwapBYTE(BYTE *p, BYTE *q)  //��������BYTE ���͵�ֵ
{
	BYTE t;
	t = *p;  *p = *q;  *q = t;
}

inline void SwapPixel(BYTE *p, BYTE *q)  //����������
{
	SwapBYTE(p, q);  //����B ֵ
	SwapBYTE(p + 1, q + 1);  //����G ֵ
	SwapBYTE(p + 2, q + 2);  //����R ֵ
}

bool CDib::FlipHorizontal()  //ˮƽ��תͼ��
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nColor == 24)  //��ͼ����24λɫʱ
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //����ɨ��
			for (int j = 0; j < nWidth / 2 + 1; j++)  //���ɨ�赥�е�ǰ��������
				SwapPixel(m_pData + (i*bytePerline + 3 * j), m_pData + (i*bytePerline + (nWidth - 1 - j) * 3));  //�������Գ�λ�õ�����
	else if (nColor == 8)	//��ͼ����8λɫʱ
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //����ɨ��
			for (int j = 0; j < nWidth / 2 + 1; j++)  //���ɨ�赥�е�ǰ��������
				SwapBYTE(m_pData + (i*bytePerline + j), m_pData + (i*bytePerline + (nWidth - 1 - j)));  //�������Գ�λ�õ�����
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	return true;
}

bool CDib::FlipVertical()  //��ֱ��תͼ��
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nHeight = m_pBitmapInfoHeader->biHeight;  //ͼ��߶�(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nColor == 24)  //��ͼ����24λɫʱ
		for (int i = 0; i < nWidth; i++)  //����ɨ��
			for (int j = 0; j < nHeight / 2 + 1; j++)  //���ɨ�赥�е�ǰ��������
				SwapPixel(m_pData + (j*bytePerline + 3 * i), m_pData + ((nHeight - j - 1)*bytePerline + 3 * i));  //�������Գ�λ�õ�����
	else if (nColor == 8)
		for (int i = 0; i < nWidth; i++)  //����ɨ��
			for (int j = 0; j < nHeight / 2 + 1; j++)  //���ɨ�赥�е�ǰ��������
				SwapBYTE(m_pData + (j*bytePerline + i), m_pData + ((nHeight - j - 1)*bytePerline + i));  //�������Գ�λ�õ�����
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	return true;
}

void Smooth4_8bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //8λɫͼ���4-����ƽ��
{
	/*���ĸ��ǽ���ƽ��*/
	int pixel = 0;  //�������Ͻǣ�ʵ��Ϊͼ�����½�
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel + bytePerline]) / 3;
	pixel = nWidth - 1;  //�������Ͻǣ�ʵ��Ϊͼ�����½�����
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel + bytePerline]) / 3;
	pixel = bytePerline * (nHeight - 1);  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline]) / 3;
	pixel = bytePerline * (nHeight - 1) + nWidth - 1;  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel - bytePerline]) / 3;
	/*�����������ĸ��߽����ƽ��*/
	int i, last;
	last = nWidth - 1;
	for (i = 1; i < last; i++)  //�����ϱ߽磬ʵ��Ϊͼ���±߽�
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i + bytePerline]) / 4;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = bytePerline * (nHeight - 1) + 1; i < last; i++)  //�����±߽磬ʵ��Ϊͼ����ϱ߽�
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline]) / 4;
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //������߽磬ʵ��Ϊͼ��������ϵ���߽�
		pData[i] = (pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline]) / 4;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = nWidth - 1 + bytePerline; i < last; i += bytePerline)  //�����ұ߽磬ʵ��Ϊͼ��������ϵ��ұ߽�
		pData[i] = (pDataCopy[i] + pDataCopy[i - 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline]) / 4;
	/*����������ƽ��*/
	for (i = 1; i < nHeight - 1; i++)  //���д���
		for (int j = 1; j < nWidth - 1; j++)  //���δ����е�ÿ������
		{
			pixel = i * bytePerline + j;
			pData[pixel] = (pDataCopy[pixel - 1] + pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel + bytePerline]) / 5;
		}
}

inline void GetAverage_3pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* p)
{
	*p = (*a + *b + *c) / 3;  //��B ֵƽ��ֵ
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1)) / 3;  //��G ֵƽ��ֵ
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2)) / 3;  //��R ֵƽ��ֵ
}

inline void GetAverage_4pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* p)
{
	*p = (*a + *b + *c + *d) / 4;  //��B ֵƽ��ֵ
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1)) / 4;  //��G ֵƽ��ֵ
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2)) / 4;  //��R ֵƽ��ֵ
}

inline void GetAverage_5pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* e, BYTE* p)
{
	*p = (*a + *b + *c + *d + *e) / 5;  //��B ֵƽ��ֵ
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1) + *(e + 1)) / 5;  //��G ֵƽ��ֵ
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2) + *(e + 2)) / 5;  //��R ֵƽ��ֵ
}

void Smooth4_24bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //24λɫͼ���4-����ƽ��
{
	/*���ĸ��ǽ���ƽ��*/
	int pixel = 0;  //�������Ͻǣ�ʵ��Ϊͼ�����½�
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel + bytePerline, pData + pixel);
	pixel = nWidth * 3 - 3;  //�������Ͻǣ�ʵ��Ϊͼ�����½�����
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel + bytePerline, pData + pixel);
	pixel = bytePerline * (nHeight - 1);  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pData + pixel);
	pixel = bytePerline * (nHeight - 1) + nWidth * 3 - 3;  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	GetAverage_3pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel - bytePerline, pData + pixel);
	/*�����������ĸ��߽����ƽ��*/
	int i, last;
	last = nWidth * 3 - 3;
	for (i = 3; i < last; i += 3)  //�����ϱ߽磬ʵ��Ϊͼ���±߽�
		GetAverage_4pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i + bytePerline, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = bytePerline * (nHeight - 1) + 3; i < last; i += 3)  //�����±߽磬ʵ��Ϊͼ����ϱ߽�
		GetAverage_4pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pData + i);
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //������߽磬ʵ��Ϊͼ��������ϵ���߽�
		GetAverage_4pixel(pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = nWidth * 3 - 3 + bytePerline; i < last; i += bytePerline)  //�����ұ߽磬ʵ��Ϊͼ��������ϵ��ұ߽�
		GetAverage_4pixel(pDataCopy + i, pDataCopy + i - 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pData + i);
	/*����������ƽ��*/
	for (i = 1; i < nHeight - 1; i++)  //���д���
		for (int j = 1; j < nWidth - 1; j++)  //���δ����е�ÿ������
		{
			pixel = i * bytePerline + j * 3;
			GetAverage_5pixel(pDataCopy + pixel - 3, pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel + bytePerline, pData + pixel);
		}
}

bool CDib::Smooth4()  //4-����ƽ��
{
	int nWidth = m_pBitmapInfoHeader->biWidth;
	int nHeight = m_pBitmapInfoHeader->biHeight;
	int nColor = m_pBitmapInfoHeader->biBitCount;
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nWidth < 2 || nHeight < 2) return false;  //ͼ��ߴ��С
	if (!m_pData) return false;  //��ȡ����ʧ��
	int DataSize = bytePerline*nHeight;  //���ؾ����С
	BYTE *pDataCopy = new BYTE[DataSize];  //�½������ؾ���
	if (pDataCopy == NULL)  //�ж϶ѿռ��Ƿ�ɹ�����
	{
		cout << "�ѿռ��ڴ�������" << endl;
		return false;
	}
	for (int i = 0; i < DataSize; i++)  //��������������
		pDataCopy[i] = m_pData[i];
	switch (nColor)
	{
	case 8:
		Smooth4_8bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //����8λɫͼ��
		break;
	case 24:
		Smooth4_24bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //����24λɫͼ��
		break;
	default:
		cout << "�޷�������ɫ���Ϊ" << nColor << "λ��ͼ��!" << endl;
		return false;
		break;
	}
	delete[]pDataCopy;  //�ͷſռ�
	pDataCopy = NULL;
	return true;
}

void Smooth8_8bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //8λɫͼ���8-����ƽ��
{
	/*���ĸ��ǽ���ƽ��*/
	int pixel = 0;  //�������Ͻǣ�ʵ��Ϊͼ�����½�
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel + bytePerline] + pDataCopy[pixel + bytePerline + 1]) / 4;
	pixel = nWidth - 1;  //�������Ͻǣ�ʵ��Ϊͼ�����½�����
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel + bytePerline] + pDataCopy[pixel + bytePerline - 1]) / 4;
	pixel = bytePerline * (nHeight - 1);  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel - bytePerline + 1]) / 4;
	pixel = bytePerline * (nHeight - 1) + nWidth - 1;  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	pData[pixel] = (pDataCopy[pixel] + pDataCopy[pixel - 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel - bytePerline - 1]) / 4;
	/*�����������ĸ��߽����ƽ��*/
	int i, last;
	last = nWidth - 1;
	for (i = 1; i < last; i++)  //�����ϱ߽磬ʵ��Ϊͼ���±߽�
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i + bytePerline] + pDataCopy[i + bytePerline - 1] + pDataCopy[i + bytePerline + 1]) / 6;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = bytePerline * (nHeight - 1) + 1; i < last; i++)  //�����±߽磬ʵ��Ϊͼ����ϱ߽�
		pData[i] = (pDataCopy[i - 1] + pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline] + pDataCopy[i - bytePerline - 1] + pDataCopy[i - bytePerline + 1]) / 6;
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //������߽磬ʵ��Ϊͼ��������ϵ���߽�
		pData[i] = (pDataCopy[i] + pDataCopy[i + 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline] + pDataCopy[i - bytePerline + 1] + pDataCopy[i + bytePerline + 1]) / 6;
	last = bytePerline * (nHeight - 1) + nWidth - 1;
	for (i = nWidth - 1 + bytePerline; i < last; i += bytePerline)  //�����ұ߽磬ʵ��Ϊͼ��������ϵ��ұ߽�
		pData[i] = (pDataCopy[i] + pDataCopy[i - 1] + pDataCopy[i - bytePerline] + pDataCopy[i + bytePerline] + pDataCopy[i - bytePerline - 1] + pDataCopy[i + bytePerline - 1]) / 6;
	/*����������ƽ��*/
	for (i = 1; i < nHeight - 1; i++)  //���д���
		for (int j = 1; j < nWidth - 1; j++)  //���δ����е�ÿ������
		{
			pixel = i * bytePerline + j;
			pData[pixel] = (pDataCopy[pixel - 1] + pDataCopy[pixel] + pDataCopy[pixel + 1] + pDataCopy[pixel - bytePerline] + pDataCopy[pixel + bytePerline]+pDataCopy[pixel - bytePerline-1] + pDataCopy[pixel - bytePerline + 1] + pDataCopy[pixel + bytePerline - 1] + pDataCopy[pixel + bytePerline + 1]) / 9;
		}
}

inline void GetAverage_6pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* e, BYTE* f,BYTE* p)
{
	*p = (*a + *b + *c + *d + *e + *f) / 6;  //��B ֵƽ��ֵ
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1) + *(e + 1) + *(f + 1)) / 6;  //��G ֵƽ��ֵ
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2) + *(e + 2) + *(f + 2)) / 6;  //��R ֵƽ��ֵ
}

inline void GetAverage_9pixel(BYTE* a, BYTE* b, BYTE* c, BYTE* d, BYTE* e, BYTE* f, BYTE* g, BYTE* h, BYTE* i, BYTE* p)
{
	*p = (*a + *b + *c + *d + *e + *f + *g + *h + *i) / 9;  //��B ֵƽ��ֵ
	*(p + 1) = (*(a + 1) + *(b + 1) + *(c + 1) + *(d + 1) + *(e + 1) + *(f + 1) + *(g + 1) + *(h + 1) + *(i + 1)) / 9;  //��G ֵƽ��ֵ
	*(p + 2) = (*(a + 2) + *(b + 2) + *(c + 2) + *(d + 2) + *(e + 2) + *(f + 2) + *(g + 2) + *(h + 2) + *(i + 2)) / 9;  //��R ֵƽ��ֵ
}

void Smooth8_24bit(BYTE * pData, BYTE * pDataCopy, int nWidth, int nHeight, int bytePerline)  //24λɫͼ���8-����ƽ��
{
	/*���ĸ��ǽ���ƽ��*/
	int pixel = 0;  //�������Ͻǣ�ʵ��Ϊͼ�����½�
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel + bytePerline, pDataCopy + pixel + bytePerline + 3, pData + pixel);
	pixel = nWidth * 3 - 3;  //�������Ͻǣ�ʵ��Ϊͼ�����½�����
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel + bytePerline, pDataCopy + pixel + bytePerline - 3, pData + pixel);
	pixel = bytePerline * (nHeight - 1);  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel - bytePerline + 3, pData + pixel);
	pixel = bytePerline * (nHeight - 1) + nWidth * 3 - 3;  //�������½ǣ�ʵ��Ϊͼ�����Ͻ�Ԫ��
	GetAverage_4pixel(pDataCopy + pixel, pDataCopy + pixel - 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel - bytePerline - 3, pData + pixel);
	/*�����������ĸ��߽����ƽ��*/
	int i, last;
	last = nWidth * 3 - 3;
	for (i = 3; i < last; i += 3)  //�����ϱ߽磬ʵ��Ϊͼ���±߽�
		GetAverage_6pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i + bytePerline, pDataCopy + i + bytePerline - 3, pDataCopy + i + bytePerline + 3, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = bytePerline * (nHeight - 1) + 3; i < last; i += 3)  //�����±߽磬ʵ��Ϊͼ����ϱ߽�
		GetAverage_6pixel(pDataCopy + i - 3, pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pDataCopy + i - bytePerline - 3, pDataCopy + i - bytePerline + 3, pData + i);
	last = bytePerline * (nHeight - 1);
	for (i = bytePerline; i < last; i += bytePerline)  //������߽磬ʵ��Ϊͼ��������ϵ���߽�
		GetAverage_6pixel(pDataCopy + i, pDataCopy + i + 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pDataCopy + i - bytePerline + 3, pDataCopy + i + bytePerline + 3, pData + i);
	last = bytePerline * (nHeight - 1) + nWidth * 3 - 3;
	for (i = nWidth * 3 - 3 + bytePerline; i < last; i += bytePerline)  //�����ұ߽磬ʵ��Ϊͼ��������ϵ��ұ߽�
		GetAverage_6pixel(pDataCopy + i, pDataCopy + i - 3, pDataCopy + i - bytePerline, pDataCopy + i + bytePerline, pDataCopy + i - bytePerline - 3, pDataCopy + i + bytePerline - 3, pData + i);
	/*����������ƽ��*/
	for (i = 1; i < nHeight - 1; i++)  //���д���
		for (int j = 1; j < nWidth - 1; j++)  //���δ����е�ÿ������
		{
			pixel = i * bytePerline + j * 3;
			GetAverage_9pixel(pDataCopy + pixel - 3, pDataCopy + pixel, pDataCopy + pixel + 3, pDataCopy + pixel - bytePerline, pDataCopy + pixel + bytePerline, pDataCopy + pixel - bytePerline - 3, pDataCopy + pixel - bytePerline + 3, pDataCopy + pixel + bytePerline - 3, pDataCopy + pixel + bytePerline + 3, pData + pixel);
		}
}

bool CDib::Smooth8()  //8-����ƽ��
{
	int nWidth = m_pBitmapInfoHeader->biWidth;
	int nHeight = m_pBitmapInfoHeader->biHeight;
	int nColor = m_pBitmapInfoHeader->biBitCount;
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nWidth < 2 || nHeight < 2) return false;  //ͼ��ߴ��С
	if (!m_pData) return false;  //��ȡ����ʧ��
	int DataSize = bytePerline*nHeight;  //���ؾ����С
	BYTE *pDataCopy = new BYTE[DataSize];  //�½������ؾ���
	if (pDataCopy == NULL)  //�ж϶ѿռ��Ƿ�ɹ�����
	{
		cout << "�ѿռ��ڴ�������" << endl;
		return false;
	}
	for (int i = 0; i < DataSize; i++)  //��������������
		pDataCopy[i] = m_pData[i];
	switch (nColor)
	{
	case 8:
		Smooth8_8bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //����8λɫͼ��
		break;
	case 24:
		Smooth8_24bit(m_pData, pDataCopy, nWidth, nHeight, bytePerline);  //����24λɫͼ��
		break;
	default:
		cout << "�޷�������ɫ���Ϊ" << nColor << "λ��ͼ��!" << endl;
		return false;
		break;
	}
	delete[]pDataCopy;  //�ͷſռ�
	pDataCopy = NULL;
	return true;
}

bool CDib::Convert24to8Bit_Grey(BYTE *p, int w, int h, int n)  //��24λͼ����8λ�Ҷȴ���
{
	if (n != 24) return false;
	int nColor = 8;  //8λͼ����ɫ���Ϊ8
	int bytePerline = ((w*n + 31) / 32) * 4;  //ԭͼ��ÿ���ֽ���
	int New_bytePerline = ((w*nColor + 31) / 32) * 4;  //��ͼ��ÿ���ֽ���
	/*����һ�����ؾ����Կ����Ҷ�����*/
	for (int i = 0; i < h; i++)  //���б���
		for (int j = 0; j < w; j++)  //�������е�ÿ������
			m_pData[New_bytePerline*i + j] = p[bytePerline*i + 3 * j];
	return true;
}

BYTE* CDib::GetBasicInfo(int &w, int &h, int &n)  //��ȡͼ�������Ϣ
{
	w = m_pBitmapInfoHeader->biWidth;
	h = m_pBitmapInfoHeader->biHeight;
	n = m_pBitmapInfoHeader->biBitCount;
	return m_pData;
}

bool CDib::Convert8toMonochrome(int k)  //��8λ�Ҷ�BMP�ļ�ת���ɺڰ�ͼ��
{
	//BMP��������Ŀ�϶࣬����ֱ�Ӹı���ɫ����
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

bool CDib::Convert24to8Bit(BYTE *p, int w, int h, int n)  //��24λ���ɫͼ��ת��Ϊ256ɫͼ��
{
	if (n != 24) return false;
	int nColor = 8;  //8λͼ����ɫ���Ϊ8
	int bytePerline = ((w*n + 31) / 32) * 4;  //ԭͼ��ÿ���ֽ���
	int New_bytePerline = ((w*nColor + 31) / 32) * 4;  //��ͼ��ÿ���ֽ���
	//����ԭ���ؾ���ͳ��256������ɫ
	int *colorCount = new int[16777218];  //���������λ�ù�ջ����ջ��ʹ��
	for (int i = 0; i < 16777216; i++)  //��ʼ��
		colorCount[i] = 0;
	if (!colorCount)
	{
		cout << "ͼ��ͳ���ڴ����ʧ��!" << endl;
		return false;
	}
	//��ʼ�����ڼ�¼����ɫ��ջ
	auto start01 = std::chrono::high_resolution_clock::now();  //�㷨��ʼʱ��
	DNode *top = new DNode;  //ջ��ջ�����
	DNode *bottom = new DNode;  //ջ��ջ�׽��
	top->prior = bottom->next = NULL;
	top->next = bottom;
	bottom->prior = top;
	top->data = 16777216;
	colorCount[16777216] = 0;  //ջ��Ԫ�ض�Ӧ�Ĵ�����С
	bottom->data = 16777217;
	colorCount[16777217] = INT_MAX;  //ջ��Ԫ�ض�Ӧ�Ĵ������
	int Ssize = 0;  //ջ�ĳ���
	int loc;  //����λ��
	int min_count = 0;  //ջ�г��ִ������ٵ�����ɫ�Ĵ���
	for (int i = 0; i < h; i++)  //���б���
		for (int j = 0; j < w; j++)  //�������е�ÿ������
		{
			loc = p[bytePerline*i + 3 * j] * 65536 + p[bytePerline*i + 3 * j + 1] * 256 + p[bytePerline*i + 3 * j + 2];
			colorCount[loc]++;
			if (colorCount[loc] > min_count || !(Ssize == 256))  //ĳ��ɫ������ջ����ʱ
			{
				DNode *p = new DNode;
				DNode *q = top;
				p->data = loc;
				while (q)  //���ʵ�λ�ò������ɫ����
				{
					if (colorCount[loc] <= colorCount[q->data])
					{
						if (loc == q->data)  //����ɫ�Ѵ�����������ʱ
						{
							//��ժ��
							q->prior->next = q->next;
							q->next->prior = q->prior;
							q = q->next;
							while (q)  //�ں���λ�ò�����
							{
								if (colorCount[loc] <= colorCount[q->data])
								{
									//��q��ǰһλ�ò��룬��ʱջ�ĳ��Ȳ���
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
							//��q��ǰһλ�ò���p
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
				if (Ssize > 256)  //��ջ����256����ɫ����ջ��Ԫ�س�ջ
				{
					q = top->next;
					top->next = q->next;
					q->next->prior = top;
					delete q;
					Ssize--;
				}
				min_count = colorCount[top->next->data];  //����min_count
			}
		}
	//��ѡ��������ɫд����ɫ����Ƶ�ȴӸߵ��ʹ�ţ���߽�����д��������ʱ���ٶ�
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
	//�ͷ�ջ�ռ�
	popular = top;
	while (popular->next)  //��ѭ����δ�ͷ�ջ��
	{
		popular = popular->next;
		delete popular->prior;
	}
	delete bottom;
	top = bottom = popular = NULL;
	auto elapsed01 = std::chrono::high_resolution_clock::now() - start01;  //�㷨����ʱ��
	
	/*//����������ѡ������
	auto start01 = std::chrono::high_resolution_clock::now();  //�㷨��ʼʱ��
	for (int i = 0; i < h; i++)  //���б���
		for (int j = 0; j < w; j++)  //�������е�ÿ������
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
	auto elapsed01 = std::chrono::high_resolution_clock::now() - start01;  //�㷨����ʱ��
	*/
	//�ͷ�colorCount
	delete[]colorCount;
	colorCount = NULL;
	//������ɫ��ĻҶ�������
	int rgbQuad_grey[257] = { 25500 };  //Ԥ��һ��λ�÷�������ʱ����,Ĭ��Ϊ��ɫ
	for (int i = 0; i < 256; i++)
		rgbQuad_grey[i] = m_pRgbQuad[i].rgbBlue * 11 + m_pRgbQuad[i].rgbGreen * 59 + m_pRgbQuad[i].rgbRed * 30;
	//����ԭͼ�����ؾ��󣬰��Ҷ�ֵ����̶ȴ洢256ɫͼ������ؾ���
	for (int i = 0; i < h; i++)  //���б���
		for (int j = 0; j < w; j++)  //�������е�ÿ������
		{
			int pixel_grey = p[bytePerline*i + 3 * j] * 11 + p[bytePerline*i + 3 * j + 1] * 59 + p[bytePerline*i + 3 * j + 2] * 30;  //���ص�ĻҶ�ֵ
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
	/*//������������ԭͼ�����ؾ��󣬰�RGBֵ�ܲ�ֵ����̶ȴ洢256ɫͼ������ؾ���(�ٶ�����Ч���޲��)
	for (int i = 0; i < h; i++)  //���б���
		for (int j = 0; j < w; j++)  //�������е�ÿ������
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
	cout << "(" << time01 << "��s)" << endl;
	return true;
}

bool CDib::ConvertToASCII(string txtName)	//��ͼƬתΪASCII���txt�ļ�
{
	char rank[16] = {'M','N','H','Q','$','O','C','?','7','>','!',':','�C',';','.',' '};	//16���ȼ���ascii���
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nHeight = m_pBitmapInfoHeader->biHeight;  //ͼ��߶�(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	ofstream ofile(txtName);
	if (nColor == 8)  //��ͼ����8λ�Ҷ�ͼʱ
		for (int i = m_pBitmapInfoHeader->biHeight - 1; i >= 0; i--)  //����ɨ��,�������ϣ�����ͼƬ�洢˳��
		{
			for (int j = 0; j < nWidth; j++)  //���ɨ�赥�е�����
				ofile << rank[m_pRgbQuad[m_pData[i*bytePerline + j]].rgbBlue / 16] << rank[m_pRgbQuad[m_pData[i*bytePerline + j]].rgbBlue / 16];
			ofile << endl;
		}
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	ofile.close();
	return true;
}

inline void SetInverseColor(BYTE *rgb)  //�������ص�����
{
	*rgb = 255 - *rgb;  //����B ֵ
	*(rgb + 1) = 255 - *(rgb + 1);  //����G ֵ
	*(rgb + 2) = 255 - *(rgb + 2);  //����R ֵ
}

bool CDib::InverseColor()	//��ɫ����
{
	int nWidth = m_pBitmapInfoHeader->biWidth;  //ͼ����(ÿ��������)
	int nColor = m_pBitmapInfoHeader->biBitCount;  //��ɫ���
	int bytePerline = ((nWidth*nColor + 31) / 32) * 4;  //ÿ��������ռ�ֽ���
	if (nColor == 24)  //��ͼ����24λɫʱ
		for (int i = 0; i < m_pBitmapInfoHeader->biHeight; i++)  //����ɨ��
			for (int j = 0; j < nWidth; j++)  //���ɨ�赥�е�����
				SetInverseColor(m_pData + (i*bytePerline + 3 * j));  //���ڸ���������
	else if (nColor == 8)
		for (int i = 0; i < nColor; i++)
			SetColor(m_pRgbQuad + i, 255 - m_pRgbQuad[i].rgbBlue, 255 - m_pRgbQuad[i].rgbGreen, 255 - m_pRgbQuad[i].rgbRed);
	else  //������ɫ��ȵ�ͼ���账��
		return false;
	return true;
}