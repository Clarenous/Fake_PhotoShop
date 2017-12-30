#pragma once
#pragma pack(1)
#include <string>
using namespace std;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER  //λͼ�ļ�ͷ�ṹ
{
	WORD bfType;  //λͼ�ļ������ͣ����ֽڣ�����ΪBM ���ַ���ASCII����0X4D42
	DWORD bfSize;  //λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ
	WORD bfReserved1;  //�����֣�����Ϊ0
	WORD bfReserved2;  //�����֣�����Ϊ0
	DWORD bfOffBits;  //λͼ���ݵ���ʼλ�ã��������λͼ�ļ�ͷ��ƫ����
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER  //λͼ��Ϣͷ�ṹ
{
	DWORD biSize;  //���ṹ��ռ�õ��ֽ���
	LONG biWidth;  //λͼ�Ŀ��
	LONG biHeight;  //λͼ�ĸ߶�
	WORD biPlanes;  //Ŀ���豸�ļ��𣬱���Ϊ1
	WORD biBitCount;  //ÿ�����������λ����һ��Ϊ1��4��8��24��
	DWORD biCompression;  //ѹ�����ͣ�һ��Ϊ0(��ѹ��)��1(BI_RLE8ѹ��)��2(BI_RLE4ѹ��)�������мٶ���ѹ��
	DWORD biSizeImage;  //λͼ�Ĵ�С�����ֽ�Ϊ��λ
	LONG biXPelsPerMeter;  //λͼˮƽ�ֱ���(ÿ��������)
	LONG biYPelsPerMeter;  //λͼ��ֱ�ֱ���(ÿ��������)
	DWORD biClrUsed;  //λͼʵ��ʹ�õ���ɫ���е���ɫ��
	DWORD biClrImportant;  //λͼ��ʾ��������Ҫ����ɫ��
}BITMAPINFOHEADER;

typedef struct tagRGBQUAD  //λͼ��ɫ��
{
	BYTE rgbBlue;  //��ɫ������(��ΧΪ0~255)
	BYTE rgbGreen;  //��ɫ������(��ΧΪ0~255)
	BYTE rgbRed;  //��ɫ������(��ΧΪ0~255)
	BYTE rgbReserved;  //�����֣�����Ϊ0
}RGBQUAD;

class CDib  //��Control Device Independent Bitmap���ǲ���BMP�ļ�����
{
public:
	CDib();  //Ĭ�Ϲ��캯��
	~CDib();  //��������
	bool Load(string filename);  //��BMP�ļ�
	bool Save(string filename);  //����BMP�ļ�
	bool Create(int nWidth, int nHeight, int nColor);  //�����µ�BMP�ṹ
	bool GreyProcess();  //�Ҷȴ���
	bool Brightness(float a);  //����ͼƬ����
	bool Contrast(float a);  //����ͼƬ�Աȶ�
	bool FlipHorizontal();  //ˮƽ��תͼ��
	bool FlipVertical();  //��ֱ��תͼ��
	bool Smooth4();  //4-����ƽ��
	bool Smooth8();  //8-����ƽ��
	bool Convert24to8Bit_Grey(BYTE *p, int w, int h, int n);  //24λͼ����8λ�Ҷȴ���
	bool Convert8toMonochrome(int k);  //��8λ�Ҷ�BMP�ļ�ת���ɺڰ�ͼ��
	bool InverseColor();	//��ɫ����
	bool Convert24to8Bit(BYTE *p, int w, int h, int n);  //��24λ���ɫͼ��ת��Ϊ256ɫͼ��
	bool ConvertToASCII(string txtName);	//��ͼ��תΪASCII�ַ�����
	BYTE *GetBasicInfo(int &w, int &h, int &n);  //��ȡͼ��Ļ�����Ϣ
private:
	int GetNumberOfColors();  //��ȡ��ɫ��ı�����Ŀ
	void SetColor(RGBQUAD * rgb, BYTE r, BYTE g, BYTE b);  //������ɫ����
	BITMAPFILEHEADER m_BitmapFileHeader;  //BMP�ļ�ͷ�ṹ
	BITMAPINFOHEADER * m_pBitmapInfoHeader;  //ָ��BMP�ļ���Ϣ�ṹ
	RGBQUAD * m_pRgbQuad;  //ָ����ɫ��
	BYTE * m_pData;  //��������
	BYTE * pDib;  //ָ��BMP�ļ�������������
};
#pragma pack()