#pragma once
#include <string>
#include <io.h>
#include <fstream>
#include <vector>

using namespace std;

//class POINT
//{
//public:
//	POINT(double xi = 0, double yi = 0)
//	{
//		x = xi;
//		y = yi;
//	}
//	double x, y;
//};

class VECTOR2
{
public:
	VECTOR2(float ix=0, float iy=0)
	{
		x = ix;
		y = iy;
	}
	float x;
	float y;
};

class FILEIO
{
public:
	void GetJustCurrentFile(string path, vector<string>& files,string extension)	//获取指定路径下所有扩展名为extension文件名
	{
		long hFile = 0;
		struct _finddata_t fileinfo;
		string p;
		if ((hFile = _findfirst(p.assign(path).append("\\*." + extension).c_str(), &fileinfo)) != -1) 
		{
			do {
				if ((fileinfo.attrib & _A_SUBDIR)) 
				{
					;
				}
				else 
				{
					files.push_back(fileinfo.name);
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
};

class CONTROLLER
{
public:

	int mouseX, mouseY;
	bool mouseLeftDown, mouseRightDown, mouseLeftUp, mouseRightUp;
	bool mouseLeftClick, mouseRightClick;

	bool keyDown[26];
	bool keyUp[26];

};

class IMAGERESOURCE
{
public:
	IMAGERESOURCE(string iName,int iW,int iH):name(iName),height(iH),width(iW){}
	IMAGERESOURCE() {};
	string name;
	int width, height;
};

