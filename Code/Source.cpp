//Head File
#include "ObjectClass.h"
#include "PlayerController.h"

#include <windows.h>
//#include <string>
//#include <vector>
//#include <map>
//#include <unordered_map>
//#include <unordered_set>
#include <algorithm>
#include <tchar.h>//使用swprintf_s函数所需的头文件
#include <time.h> //使用获取系统时间time()函数需要包含的头文件
#include <cmath>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include "QuadTree.h"

#pragma	comment(lib,"winmm.lib")			//调用PlaySound函数所需库文件
#pragma comment(lib,"Msimg32.lib")		//添加使用TransparentBlt函数所需的库文件

using namespace std;

//-----------------#define--------------------

#define WINDOW_WIDTH	800							//为窗口宽度定义的宏，以方便在此处修改窗口宽度
#define WINDOW_HEIGHT	600							//为窗口高度定义的宏，以方便在此处修改窗口高度
#define WINDOW_TITLE	L"GAME DEMO"		//为窗口标题定义的宏
#define PI				3.14159265f

static class CONVERT
{
public:
	static wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
	{
		int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
		if (nSize <= 0) return NULL;
		WCHAR *pwszDst = new WCHAR[nSize + 1];
		if (NULL == pwszDst) return NULL;
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
		pwszDst[nSize] = 0;
		if (pwszDst[0] == 0xFEFF) // skip Oxfeff   
			for (int i = 0; i < nSize; i++)
				pwszDst[i] = pwszDst[i + 1];
		std::wstring wcharString(pwszDst);
		delete pwszDst;
		return wcharString;
	}
	static wstring s2ws(const std::string& s)
	{
		return Ansi2WChar(s.c_str(), s.size());
	}
};

//-----------------全局变量--------------------

HDC			g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;     //全局设备环境句柄与全局内存DC句柄
DWORD		 g_tNow = 0, g_tPre = 0;
RECT		g_rect;											//定义一个RECT结构体，用于储存内部窗口区域的坐标
HBITMAP		g_hBackGround;

CONTROLLER controller;

//活动的对象

unordered_map<int, GAMEOBJECT> activeObject;

unordered_map<int, GAMEOBJECT> onStartObject;

unordered_set<int> onCollisionDetectionObject;
unordered_set<int> onMovementObject;
unordered_map<int, int> onCollisionObject;
unordered_set<int> onPaintingObject;
unordered_set<int> onDestroyObject;

map<string, HBITMAP> resourceImageMap;	//图片名称和图片对象的map

unsigned long fixedDeltaTime = 10;
float deltaTime = 0;

int objectID = 0;
Quadtree quadtree(0,RECTANGLE(0,0,WINDOW_WIDTH, WINDOW_HEIGHT));

//-----------------函数声明--------------------

LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);  //窗口过程函数
BOOL						Game_Init(HWND hwnd);			//在此函数中进行资源的初始化
VOID						Game_Paint(HWND hwnd);		//在此函数中进行绘图代码的书写
BOOL						Game_ShutDown(HWND hwnd);	//在此函数中进行资源的清理

//void Game_ObjectInit();
//
//void Game_ObjectStart();
//void Game_FixedUpdate();
//void Game_Physics();
//void Game_Collision();
//void Game_Update();
//void Game_Paint();

//OBJECT CreateObject(VECTOR2 _POS,string _Name);
void DestroyAll();



//-----------------------------------【WinMain( )函数】--------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//【1】开始设计一个完整的窗口类
	WNDCLASSEX wndClass = { 0 };							//用WINDCLASSEX定义了一个窗口类
	wndClass.cbSize = sizeof(WNDCLASSEX);			//设置结构体的字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//设置窗口的样式
	wndClass.lpfnWndProc = WndProc;					//设置指向窗口过程函数的指针
	wndClass.cbClsExtra = 0;								//窗口类的附加内存，取0就可以了
	wndClass.cbWndExtra = 0;							//窗口的附加内存，依然取0就行了
	wndClass.hInstance = hInstance;						//指定包含窗口过程的程序的实例句柄。
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //本地加载自定义ico图标
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //指定窗口类的光标句柄。
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //为hbrBackground成员指定一个灰色画刷句柄	
	wndClass.lpszMenuName = NULL;						//用一个以空终止的字符串，指定菜单资源的名字。
	wndClass.lpszClassName = L"GDIcore";		//用一个以空终止的字符串，指定窗口类的名字。

	//【2】注册窗口类
	if (!RegisterClassEx(&wndClass))				//设计完窗口后，需要对窗口类进行注册，这样才能创建该类型的窗口
		return -1;

	//【3】正式创建窗口
	HWND hwnd = CreateWindow(L"GDIcore", WINDOW_TITLE,		//创建窗口函数CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//【4】窗口的移动、显示与更新
	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//调整窗口显示时的位置，使窗口左上角位于（250,80）处
	ShowWindow(hwnd, nShowCmd);		//调用ShowWindow函数来显示窗口
	UpdateWindow(hwnd);				//对窗口进行更新，就像我们买了新房子要装修一样

	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
		return FALSE;
	}
	//PlaySound(L"GameMedia\\music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); 

	//【5】消息循环过程
	MSG msg = { 0 };				//定义并初始化msg
		//bool bFrist = 1;

		//DWORD tFixedCostTime;		//每个fixed帧花费时间
	DWORD originFixedDeltaTime = fixedDeltaTime;	//最初设定的fixedDeltaTime

	DWORD tFixedStart =0;	//fixed帧开始时间
	DWORD tFixedEnd =0;	//fixed帧结束时间
	//int tt = 0;		//测试用临时变量

	g_tNow = GetTickCount();   //获取当前系统时间
	

	Game_ObjectInSence();

	while (msg.message != WM_QUIT)		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);			//分发一个消息给窗口程序。
		}
		else
		{
			g_tPre = g_tNow;
			g_tNow = GetTickCount();   //获取当前系统时间
			deltaTime = g_tNow - g_tPre;


			Game_ObjectStart();		//对象初始化


			if (tFixedEnd != tFixedStart && tFixedEnd - tFixedStart > fixedDeltaTime)
			{
				while (tFixedEnd - tFixedStart > fixedDeltaTime)
					fixedDeltaTime += fixedDeltaTime;
			}
			else if (fixedDeltaTime > originFixedDeltaTime && tFixedEnd - tFixedStart < fixedDeltaTime / 4)
			{

				fixedDeltaTime = fixedDeltaTime / 2;
			}

			static DWORD g_tFixedNow;		
			g_tFixedNow = GetTickCount();
			static DWORD g_tFixedPre = g_tFixedNow;

			while (g_tFixedNow - g_tFixedPre >= fixedDeltaTime)
			{
				tFixedStart = GetTickCount();

				//Game_Main(hwnd);
				Game_FixedUpdate();
				Game_Physics();		//物体运动，碰撞检测
				Game_Collision();	//碰撞处理
				//if (tt < 50)
				//{
				//	Sleep(80);
				//	tt++;
				//}
				//Sleep(5);
				tFixedEnd = GetTickCount();

				/*if (tFixedEnd!= tFixedStart && tFixedEnd - tFixedStart > fixedDeltaTime)
				{
					while (tFixedEnd - tFixedStart > fixedDeltaTime)
					fixedDeltaTime += fixedDeltaTime;
				}
				else if (fixedDeltaTime > originFixedDeltaTime && tFixedEnd - tFixedStart<fixedDeltaTime / 4)
				{

					fixedDeltaTime = fixedDeltaTime / 2;
				}*/
				g_tFixedPre += fixedDeltaTime;
				//g_tFixPre记录上次应执行fixupdate的时间而非实际执行时间，以实现某帧执行较慢时快速补帧

				//g_tFixedNow = GetTickCount();
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
				{
					TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
					DispatchMessage(&msg);			//分发一个消息给窗口程序。
				}
			}
			

			Game_Update();
			Game_Paint(hwnd);
			Game_ObjectDestroy();
			//Game_Main(hwnd);
			Sleep(1);
			//if (tt == 50)
			//{
			//	Sleep(500);
			//}
			//tt++;

		}

	}
	//【6】窗口类的注销
	UnregisterClass(L"GDIcore", wndClass.hInstance);  //程序准备结束，注销窗口类
	return 0;
}


	//-----------------------------------【WndProc( )函数】--------------------------------------
//	描述：窗口过程函数WndProc,对窗口消息进行处理
//------------------------------------------------------------------------------------------------
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)						//switch语句开始
		{
		case WM_PAINT:						// 若是客户区重绘消息
			ValidateRect(hwnd, NULL);		// 更新客户区的显示
			break;									//跳出该switch语句

		case WM_KEYDOWN:                // 若是键盘按下消息
			switch (wParam)
			{
			case VK_ESCAPE:           //按下【Esc】键

				DestroyAll();
				DestroyWindow(hwnd);    // 销毁窗口, 并发送一条WM_DESTROY消息
				PostQuitMessage(0);  //结束程序
				break;
			default:
				if (wParam == 87)
					controller.keyDown[87 - 65] = 1;
				if (wParam == 83)
					controller.keyDown[83 - 65] = 1;
				if (wParam == 65)
					controller.keyDown[65 - 65] = 1;
				if (wParam == 68)
					controller.keyDown[68 - 65] = 1;
				break;
			}
			break;
		
		case WM_KEYUP:			
			if (wParam == 87)
			{
				controller.keyDown[87 - 65] = 0;
				controller.keyUp[87 - 65] = 1;
			}
			if (wParam == 83)
			{
				controller.keyDown[83 - 65] = 0;
				controller.keyUp[83 - 65] = 1;
			}

			if (wParam == 65)
			{
				controller.keyDown[65 - 65] = 0;
				controller.keyUp[65 - 65] = 1;
			}

			if (wParam == 68)
			{
				controller.keyDown[68 - 65] = 0;
				controller.keyUp[68 - 65] = 1;
			}
			break;

		case WM_LBUTTONDOWN:			//鼠标左键消息
			controller.mouseLeftClick = 1;
			controller.mouseLeftDown = 1;
			break;
		case WM_RBUTTONDOWN:
			controller.mouseRightClick = 1;
			controller.mouseRightDown = 1;
			break;
		case WM_LBUTTONUP:			
			controller.mouseLeftDown = 0;
			break;
		case WM_RBUTTONUP:
			controller.mouseRightDown = 0;
			break;
		case WM_MOUSEMOVE:
			controller.mouseX = LOWORD(lParam);
			controller.mouseY = HIWORD(lParam);
			break;
		case WM_DESTROY:				//若是窗口销毁消息
			PostQuitMessage(0);		//向系统表明有个线程有终止请求。用来响应WM_DESTROY消息
			break;								//跳出该switch语句

		default:									//若上述case条件都不符合，则执行该default语句
			return DefWindowProc(hwnd, message, wParam, lParam);		//调用缺省的窗口过程
		}
		return 0;			//正常退出
	}




	BOOL Game_Init(HWND hwnd)
	{

		HBITMAP bmp;

		//三缓冲体系的创建
		g_hdc = GetDC(hwnd);
		g_mdc = CreateCompatibleDC(g_hdc);  //创建一个和hdc兼容的内存DC
		g_bufdc = CreateCompatibleDC(g_hdc);//再创建一个和hdc兼容的缓冲DC
		bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //建一个和窗口兼容的空的位图对象

		SelectObject(g_mdc, bmp);//将空位图对象放到mdc中

		g_hBackGround = (HBITMAP)LoadImage(NULL, L"BackGround.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);

		//从指定目录加载所有贴图
		//vector<string> vImageName;
		//vector<pair<int, int>> vImageSize;
		//FILEIO fileIO;
		//fileIO.GetJustCurrentFile("//Image", vImageName, "");

		//for (string s : vImageName)
		//{
		//	HBITMAP hb = (HBITMAP)LoadImage(NULL, L"Media\\BackGround.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
		//	//resourceImageMap.emplace(s,)
		//}

		
		vector<IMAGERESOURCE> vImage;
		ifstream ifs("ImageResourcePara.txt");
		if (!ifs.is_open())
		{
			//cout << "Error opening file"; exit(1);
			return FALSE;
		}
		string buffer;
		stringstream ss;
		IMAGERESOURCE iBuffer;

		while (!ifs.eof())
		{
			getline(ifs, buffer);
			ss.clear();
			ss.str(buffer);
			ss >> iBuffer.name >> iBuffer.width >> iBuffer.height;
			vImage.push_back(iBuffer);			
		}
		HBITMAP hbt;
		//CONVERT convert;
		for (auto i : vImage)
		{
			//const wchar_t* wt = convert.s2ws(i.name).c_str();
			hbt = (HBITMAP)LoadImage(NULL, CONVERT::s2ws("Image//"+i.name).c_str(), 
				IMAGE_BITMAP, i.width, i.height, LR_LOADFROMFILE);
			resourceImageMap.emplace(i.name, hbt);
		}
		

		//限制光标移动区域
		
		GetClientRect(hwnd, &g_rect);

		g_rect.left += 1;
		g_rect.top += 1;
		g_rect.bottom -= 1;
		g_rect.right -= 1;
		POINT lt, rb;
		lt.x = g_rect.left;
		lt.y = g_rect.top;
		rb.x = g_rect.right;
		rb.y = g_rect.bottom;


		ClientToScreen(hwnd, &lt);
		ClientToScreen(hwnd, &rb);

		g_rect.left = lt.x;
		g_rect.top = lt.y;
		g_rect.right = rb.x;
		g_rect.bottom = rb.y;

		//ClipCursor(&g_rect);
		//ShowCursor(0);

		Game_Paint(hwnd);  //调用一次游戏Main函数
		return TRUE;
	}


	BOOL Game_ShutDown(HWND hwnd)
	{
		DeleteObject(g_hBackGround);
		for (auto i = resourceImageMap.begin();i!=resourceImageMap.end();i++)
		{
			DeleteObject(i->second);
		}
		DeleteDC(g_bufdc);
		DeleteDC(g_mdc);
		ReleaseDC(hwnd, g_hdc);
		return TRUE;
	}

	void DestroyAll()
	{
		for (auto &i : activeObject)
		{
			i.second->Destroy();
		}
		activeObject.clear();
		onCollisionDetectionObject.clear();
		onCollisionObject.clear();
		onPaintingObject.clear();
	}

	void Game_ObjectInSence()	//初始场景
	{
		//OBJECT o1(new OBJECT_implement(0));
		GAMEOBJECT o1 = CreateObject(VECTOR2(0, 0), "obj1");
		SPRITE sprite1 = make_shared<SPRITE_implement>(VECTOR2(0, 0), 100, 100, "2.bmp");
		o1->AddComponent(sprite1);
		RIGIDBODY r1 = make_shared<RIGIDBODY_implement>(VECTOR2(0, 50), VECTOR2(0, 0));
		o1->AddComponent(r1);

		//o1->AddSprite(VECTOR2(100, 100), 100, 100, "2.bmp");
		PLAYERCONTROLLER c1 = make_shared<PLAYERCONTROLLER_implement>(&controller, 100, 100);
		o1->AddComponent(c1);

		COLLIDER cl1 = make_shared<COLLIDER_implement>(VECTOR2(0, 0), 100, 100);
		o1->AddComponent(cl1);

		//for (int i = 0; i < 2000; i++)
		//{
		//	GAMEOBJECT o1 = CreateObject(VECTOR2(0, 0), "obj1");
		//	SPRITE sprite1 = make_shared<SPRITE_implement>(VECTOR2(50, 50), 100, 100, "2.bmp");
		//	o1->AddComponent(sprite1);
		//	RIGIDBODY r1 = make_shared<RIGIDBODY_implement>(VECTOR2(0, 50), VECTOR2(0, 0));
		//	o1->AddComponent(r1);
		//}

		GAMEOBJECT o2 = CreateObject(VECTOR2(0, 200),"111");
		SPRITE sprite2 = make_shared<SPRITE_implement>(VECTOR2(0, 0), 50, 50, "2.bmp");
		o2->AddComponent(sprite2);

		cl1 = make_shared<COLLIDER_implement>(VECTOR2(0, 0), 50, 50);
		o2->AddComponent(cl1);

		//o2->AddSprite(VECTOR2(250, 250), 100, 100, "2.bmp");
		//onStartObject.emplace(0, o1);

		
	}

	void Game_ObjectDestroy()
	{
		for (auto i : onDestroyObject)
		{
			auto a = activeObject.find(i);
			if (a != activeObject.end())
			{
				a->second->DestroyNow();
			}
		}
		onDestroyObject.clear();
	}

	void Game_ObjectStart()		//每一帧开始调用，用于上一帧创建的物体的初始化
	{
		if (onStartObject.empty())
			return;
		for (auto i = onStartObject.begin();i!=onStartObject.end();i++)
		{
			i->second->Start();
			activeObject.emplace(i->first, i->second);
			//if (i->second->sprite != nullptr)
				//onPaintingObject.insert(i->first);
			i->second = nullptr;
		}
		onStartObject.clear();
	}

	void Game_FixedUpdate()		//每隔fixedDeltaTime调用
	{
		for (auto i = activeObject.begin();i!=activeObject.end();i++)
		{
			i->second->FixedUpdate();
		}
	}

	void Game_Physics()
	{		
		for (auto i = onMovementObject.begin();i!=onMovementObject.end();i++)
		{
			if (activeObject.find(*i) == activeObject.end())
				continue;
			if (activeObject[*i]->rigidbody != nullptr)
				activeObject[*i]->rigidbody->Move();
		}

		//碰撞检测		
		quadtree.Clear();

		for (auto i = onCollisionDetectionObject.begin(); i != onCollisionDetectionObject.end();i++ )
		{
			quadtree.insert(*i, activeObject[*i]->collider->rect());			
		}
		vector<int> collision;
		unordered_set<int> dealingObject(onCollisionDetectionObject);
		for (auto i = dealingObject.begin(); i != dealingObject.end(); )
		{
			collision.clear();
			quadtree.Retrieve(collision, activeObject[*i]->collider->rect());
			int t = *i;
			i= dealingObject.erase(i);
			for (int j : collision)
			{
				if (dealingObject.count(j))
				{
					//if(CollisionDetection(activeObject[t]->collider->rect(),activeObject[j]->collider->rect()))
					if(activeObject[t]->collider->CollisionDetect(activeObject[j]->collider))
						onCollisionObject.emplace(t, j);	//添加到待处理碰撞物体
				}
			}
		}
		

	}

	void Game_Collision()	//碰撞处理
	{
		for (auto i = onCollisionObject.begin();i!=onCollisionObject.end();i++)
		{
			auto p1 = activeObject.find(i->first);
			auto p2 = activeObject.find(i->second);
			if (p1 == activeObject.end() || p2 == activeObject.end())
				continue;
			if (p1->second->collider == nullptr || p2->second->collider == nullptr)
				continue;

			p1->second->OnCollision(p2->second);
			p2->second->OnCollision(p1->second);

			p1->second->collider->CollisionCall(p2->second);
			p2->second->collider->CollisionCall(p1->second);
		}

		onCollisionObject.clear();	//清空需要碰撞处理的物体
	}

	void Game_Update()	//每帧调用
	{
		for (auto i = activeObject.begin();i!=activeObject.end();i++)
		{
			i->second->Update();
		}
	}

	VOID Game_Paint(HWND hwnd)
	{
		SelectObject(g_bufdc, g_hBackGround);
		BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

		for (auto i = onPaintingObject.begin(); i != onPaintingObject.end(); i++)
		{
			if (activeObject.find(*i) == activeObject.end())
				continue;
			GAMEOBJECT obj = activeObject[*i];
			auto sprite = obj->sprite;
			if (sprite != nullptr)		//绘制贴图
			{

				SelectObject(g_bufdc, resourceImageMap[obj->sprite->sourceName]);
				BitBlt(g_mdc, obj->transform->position.x + sprite->position.x - sprite->width / 2.0f,
					obj->transform->position.y + sprite->position.y - sprite->height / 2.0f,
					sprite->width, sprite->width, g_bufdc, 0, 0, SRCCOPY);

				//TransparentBlt(g_mdc, obj->position.x + sprite->position.x - sprite->width / 2.0f,
				//	obj->position.y + sprite->position.y - sprite->height / 2.0f,
				//	sprite->width, sprite->width,
				//	g_bufdc, 0, 0, sprite->width, sprite->width, RGB(255, 255, 255));
			}

		}

		BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
		//g_tFixedPre += fixedDeltaTime;
		return VOID();
	}

	GAMEOBJECT CreateObject(VECTOR2 _Pos, string _Name)
	{
		GAMEOBJECT obj = make_shared<GAMEOBJECT_implement>(objectID, _Pos, _Name);
		onStartObject.emplace(objectID, obj);
		objectID++;
		return obj;
	}

