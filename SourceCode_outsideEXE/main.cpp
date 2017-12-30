#include "BITMAP.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <chrono>
using namespace std;

void ReWrite(string filename, string save)
{
	string ext = ".bmp";
	string str = "_����д��";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename << "\"�����ڴ�......";
	if (A.Load(filename))
	{
		cout << "�ɹ�!" << endl << "\n���ڽ�24λ���ɫ�ļ�\"" << filename << "\"����д���ļ�Ϊ\"" << filename + str + ext << "\"......";
		if (A.Save(save))
		{
			cout << "�ɹ�!" << endl;
			return;
		}
		else
			cout << "ʧ��!" << endl;
	}
	else
		cout << "ʧ��!" << endl;
}

void GreyProcess24(string filename, string save)
{
	string ext = ".bmp";
	string str = "_24λ�Ҷȴ���";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename << "\"����24λ�Ҷȴ���Ϊ\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if(A.GreyProcess())
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void Convert24to8Bit_Grey(string filename, string save)
{
	CDib A, B;
	string ext = ".bmp";
	string str = "_8λ�Ҷȴ���";
	A.Load(filename);
	int nWidth, nHeight, nColor;
	BYTE *m_pData = A.GetBasicInfo(nWidth, nHeight, nColor);  //��ȡԭͼ��Ļ�����Ϣ
	if (!m_pData)
	{
		cout << "��ȡԭͼ�����!" << endl;
		return;
	}
	B.Create(nWidth, nHeight, 8);  //�����µ�BMPͼ��ṹ�Դ洢8λͼ��
	A.GreyProcess();
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename << "\"����8λ�Ҷȴ���Ϊ\"" << filename + str + ext << "\"......";
	if (B.Convert24to8Bit_Grey(m_pData, nWidth, nHeight, nColor))
	{
		if (B.Save(save))
		{
			cout << "�ɹ�!" << endl;
			return;
		}
	}
	cout << "ʧ��!" << endl;
}

void Convert8toMonochrome(string filename, string save, int k)//kΪ��ֵ��ϵ��
{
	string ext = ".bmp";
	string str = "_תΪ�ڰ�ͼ��";
	CDib A;
	cout << "���ڽ�8λ�Ҷ��ļ�\"" << filename << "\"���кڰ�ɫ����Ϊ\"" << save << "\"......";
	if (A.Load(filename))
	{
		if(A.Convert8toMonochrome(k))
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void Smooth4(string filename, string save)
{
	string ext = ".bmp";
	string str = "_4����ƽ��";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename<< "\"����4����ƽ������Ϊ\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.Smooth4())
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void Smooth8(string filename, string save)
{
	string ext = ".bmp";
	string str = "_8����ƽ��";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename<< "\"����8����ƽ������Ϊ\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.Smooth8())
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void FlipHorizontal(string filename, string save)
{
	string ext = ".bmp";
	string str = "_ˮƽ��ת";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename<< "\"����ˮƽ��ת����Ϊ\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.FlipHorizontal())
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void FlipVertical(string filename, string save)
{
	string ext = ".bmp";
	string str = "_��ֱ��ת";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename<< "\"���д�ֱ��ת����Ϊ\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.FlipVertical())
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void Brightness(string filename, string save, double a)
{
	string ext = ".bmp";
	char s[10];
#pragma warning(disable:4996)
	_gcvt(a, 5, s);
	string str = "_���ȵ���(aֵΪ" + (string)s + ")";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename << "\"�������ȵ��ڴ���Ϊ\"" << save << "\"......";
	a = 1 - a;
	if (a < 0.5)
		a *= 2;
	else if (a == 0.5)
		a = 1;
	else
		a = 8 * a - 3;
	if (A.Load(filename))
	{
		if (A.Brightness(a))
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void Contrast(string filename, string save, double a)
{
	string ext = ".bmp";
	char s[10];
//#pragma warning(disable:4996)
	_gcvt(a, 5, s);
	a = a * 2 - 1;
	if (a > 0)
		a *= 1.5;
	string str = "_�Աȶȵ���(aֵΪ" + (string)s + ")";
	CDib A;
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename << "\"���жԱȶȵ��ڴ���Ϊ\"" << save << "\"......";
	if (A.Load(filename))
	{
		if (A.Contrast(a))
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}

void Convert24to8Bit(string filename, string save)
{
	CDib A, B;
	string str = "_24λ���ɫת256ɫ";
	string ext = ".bmp";
	A.Load(filename);
	int nWidth, nHeight, nColor;
	BYTE *m_pData = A.GetBasicInfo(nWidth, nHeight, nColor);  //��ȡԭͼ��Ļ�����Ϣ
	if (!m_pData)
	{
		cout << "��ȡԭͼ�����!" << endl;
		return;
	}
	B.Create(nWidth, nHeight, 8);  //�����µ�BMPͼ��ṹ�Դ洢8λͼ��
	cout << "���ڽ�24λ���ɫ�ļ�\"" << filename<< "\"ת256ɫΪ\"" << save << "\"......";
	if (B.Convert24to8Bit(m_pData, nWidth, nHeight, nColor))
	{
		if (B.Save(save))
		{
			cout << "�ɹ�!" << endl;
			return;
		}
	}
	cout << "ʧ��!" << endl;
}

void ConvertToASCII(string filename, string txtName)	//ת��ΪASCII��
{
	string ext = ".bmp";
	string str = "_ˮƽ��ת";
	CDib A;
	cout << "���ڽ�8λ�Ҷ�BMP�ļ�\"" << filename << "\"����ASCII��ת������Ϊ\"" << txtName << "\"......";
	if (A.Load(filename))
	{
		if (A.ConvertToASCII(txtName))
		{
			cout << "�ɹ���" << endl;
			return;
		}
	}
	cout << "ʧ��!" << endl;
}

void InverseColor(string filename, string save)	//��ɫ����
{
	string ext = ".bmp";
	string str = "_ˮƽ��ת";
	CDib A;
	cout << "���ڽ�\"" << filename << "\"���з�ɫ����Ϊ\"" << save << "\"......";
	if (A.Load(filename))
	{
		if (A.InverseColor())
			if (A.Save(save))
			{
				cout << "�ɹ�!" << endl;
				return;
			}
	}
	cout << "ʧ��!" << endl;
}


int main(int argc, char *argv[])//atofת�ַ���Ϊdouble, atoiתΪint
{
	int menu = atoi(argv[1]);//ѡ����
	try
	{
		switch (menu)
		{
		case 1:
			ReWrite(argv[2], argv[3]);
			break;
		case 2:
			GreyProcess24(argv[2], argv[3]);
			break;
		case 3:
			Convert24to8Bit_Grey(argv[2], argv[3]);
			break;
		case 4:
			Smooth4(argv[2], argv[3]);
			break;
		case 5:
			Smooth8(argv[2], argv[3]);
			break;
		case 6:
			FlipHorizontal(argv[2], argv[3]);
			break;
		case 7:
			FlipVertical(argv[2], argv[3]);
			break;
		case 8:
			Convert24to8Bit(argv[2], argv[3]);
			break;
		case 9:
			ConvertToASCII(argv[2], argv[3]);
			break;
		case 10:
			InverseColor(argv[2], argv[3]);
			break;
		case 11:
			Brightness(argv[2], argv[3], atof(argv[4]));
			break;
		case 12:
			Contrast(argv[2], argv[3], atof(argv[4]));
			break;
		case 13:
			Convert8toMonochrome(argv[2], argv[3], atoi(argv[4]));
			break;
		default:
			break;
		}
	}
	catch (char* s)
	{
		cout << s << endl;
	}
	return 0;
}