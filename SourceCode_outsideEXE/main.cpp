#include "BITMAP.h"
#include <iostream>
#include <string>
#include <stdlib.h>
#include <chrono>
using namespace std;

void ReWrite(string filename, string save)
{
	string ext = ".bmp";
	string str = "_重新写入";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename << "\"读入内存......";
	if (A.Load(filename))
	{
		cout << "成功!" << endl << "\n正在将24位真彩色文件\"" << filename << "\"重新写入文件为\"" << filename + str + ext << "\"......";
		if (A.Save(save))
		{
			cout << "成功!" << endl;
			return;
		}
		else
			cout << "失败!" << endl;
	}
	else
		cout << "失败!" << endl;
}

void GreyProcess24(string filename, string save)
{
	string ext = ".bmp";
	string str = "_24位灰度处理";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename << "\"进行24位灰度处理为\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if(A.GreyProcess())
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void Convert24to8Bit_Grey(string filename, string save)
{
	CDib A, B;
	string ext = ".bmp";
	string str = "_8位灰度处理";
	A.Load(filename);
	int nWidth, nHeight, nColor;
	BYTE *m_pData = A.GetBasicInfo(nWidth, nHeight, nColor);  //获取原图像的基本信息
	if (!m_pData)
	{
		cout << "读取原图像出错!" << endl;
		return;
	}
	B.Create(nWidth, nHeight, 8);  //建立新的BMP图像结构以存储8位图像
	A.GreyProcess();
	cout << "正在将24位真彩色文件\"" << filename << "\"进行8位灰度处理为\"" << filename + str + ext << "\"......";
	if (B.Convert24to8Bit_Grey(m_pData, nWidth, nHeight, nColor))
	{
		if (B.Save(save))
		{
			cout << "成功!" << endl;
			return;
		}
	}
	cout << "失败!" << endl;
}

void Convert8toMonochrome(string filename, string save, int k)//k为二值化系数
{
	string ext = ".bmp";
	string str = "_转为黑白图像";
	CDib A;
	cout << "正在将8位灰度文件\"" << filename << "\"进行黑白色处理为\"" << save << "\"......";
	if (A.Load(filename))
	{
		if(A.Convert8toMonochrome(k))
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void Smooth4(string filename, string save)
{
	string ext = ".bmp";
	string str = "_4邻域平滑";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename<< "\"进行4邻域平滑处理为\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.Smooth4())
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void Smooth8(string filename, string save)
{
	string ext = ".bmp";
	string str = "_8邻域平滑";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename<< "\"进行8邻域平滑处理为\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.Smooth8())
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void FlipHorizontal(string filename, string save)
{
	string ext = ".bmp";
	string str = "_水平翻转";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename<< "\"进行水平翻转处理为\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.FlipHorizontal())
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void FlipVertical(string filename, string save)
{
	string ext = ".bmp";
	string str = "_垂直翻转";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename<< "\"进行垂直翻转处理为\"" << filename + str + ext << "\"......";
	if (A.Load(filename))
	{
		if (A.FlipVertical())
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void Brightness(string filename, string save, double a)
{
	string ext = ".bmp";
	char s[10];
#pragma warning(disable:4996)
	_gcvt(a, 5, s);
	string str = "_亮度调节(a值为" + (string)s + ")";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename << "\"进行亮度调节处理为\"" << save << "\"......";
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
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
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
	string str = "_对比度调节(a值为" + (string)s + ")";
	CDib A;
	cout << "正在将24位真彩色文件\"" << filename << "\"进行对比度调节处理为\"" << save << "\"......";
	if (A.Load(filename))
	{
		if (A.Contrast(a))
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}

void Convert24to8Bit(string filename, string save)
{
	CDib A, B;
	string str = "_24位真彩色转256色";
	string ext = ".bmp";
	A.Load(filename);
	int nWidth, nHeight, nColor;
	BYTE *m_pData = A.GetBasicInfo(nWidth, nHeight, nColor);  //获取原图像的基本信息
	if (!m_pData)
	{
		cout << "读取原图像出错!" << endl;
		return;
	}
	B.Create(nWidth, nHeight, 8);  //建立新的BMP图像结构以存储8位图像
	cout << "正在将24位真彩色文件\"" << filename<< "\"转256色为\"" << save << "\"......";
	if (B.Convert24to8Bit(m_pData, nWidth, nHeight, nColor))
	{
		if (B.Save(save))
		{
			cout << "成功!" << endl;
			return;
		}
	}
	cout << "失败!" << endl;
}

void ConvertToASCII(string filename, string txtName)	//转换为ASCII码
{
	string ext = ".bmp";
	string str = "_水平翻转";
	CDib A;
	cout << "正在将8位灰度BMP文件\"" << filename << "\"进行ASCII码转换处理为\"" << txtName << "\"......";
	if (A.Load(filename))
	{
		if (A.ConvertToASCII(txtName))
		{
			cout << "成功！" << endl;
			return;
		}
	}
	cout << "失败!" << endl;
}

void InverseColor(string filename, string save)	//反色处理
{
	string ext = ".bmp";
	string str = "_水平翻转";
	CDib A;
	cout << "正在将\"" << filename << "\"进行反色处理为\"" << save << "\"......";
	if (A.Load(filename))
	{
		if (A.InverseColor())
			if (A.Save(save))
			{
				cout << "成功!" << endl;
				return;
			}
	}
	cout << "失败!" << endl;
}


int main(int argc, char *argv[])//atof转字符串为double, atoi转为int
{
	int menu = atoi(argv[1]);//选择功能
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