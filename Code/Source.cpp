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
#include <tchar.h>//ʹ��swprintf_s���������ͷ�ļ�
#include <time.h> //ʹ�û�ȡϵͳʱ��time()������Ҫ������ͷ�ļ�
#include <cmath>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include "QuadTree.h"

#pragma	comment(lib,"winmm.lib")			//����PlaySound����������ļ�
#pragma comment(lib,"Msimg32.lib")		//���ʹ��TransparentBlt��������Ŀ��ļ�

using namespace std;

//-----------------#define--------------------

#define WINDOW_WIDTH	800							//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define WINDOW_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define WINDOW_TITLE	L"GAME DEMO"		//Ϊ���ڱ��ⶨ��ĺ�
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

//-----------------ȫ�ֱ���--------------------

HDC			g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;     //ȫ���豸���������ȫ���ڴ�DC���
DWORD		 g_tNow = 0, g_tPre = 0;
RECT		g_rect;											//����һ��RECT�ṹ�壬���ڴ����ڲ��������������
HBITMAP		g_hBackGround;

CONTROLLER controller;

//��Ķ���

unordered_map<int, GAMEOBJECT> activeObject;

unordered_map<int, GAMEOBJECT> onStartObject;

unordered_set<int> onCollisionDetectionObject;
unordered_set<int> onMovementObject;
unordered_map<int, int> onCollisionObject;
unordered_set<int> onPaintingObject;
unordered_set<int> onDestroyObject;

map<string, HBITMAP> resourceImageMap;	//ͼƬ���ƺ�ͼƬ�����map

unsigned long fixedDeltaTime = 10;
float deltaTime = 0;

int objectID = 0;
Quadtree quadtree(0,RECTANGLE(0,0,WINDOW_WIDTH, WINDOW_HEIGHT));

//-----------------��������--------------------

LRESULT CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);  //���ڹ��̺���
BOOL						Game_Init(HWND hwnd);			//�ڴ˺����н�����Դ�ĳ�ʼ��
VOID						Game_Paint(HWND hwnd);		//�ڴ˺����н��л�ͼ�������д
BOOL						Game_ShutDown(HWND hwnd);	//�ڴ˺����н�����Դ������

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



//-----------------------------------��WinMain( )������--------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//��1����ʼ���һ�������Ĵ�����
	WNDCLASSEX wndClass = { 0 };							//��WINDCLASSEX������һ��������
	wndClass.cbSize = sizeof(WNDCLASSEX);			//���ýṹ����ֽ�����С
	wndClass.style = CS_HREDRAW | CS_VREDRAW;	//���ô��ڵ���ʽ
	wndClass.lpfnWndProc = WndProc;					//����ָ�򴰿ڹ��̺�����ָ��
	wndClass.cbClsExtra = 0;								//������ĸ����ڴ棬ȡ0�Ϳ�����
	wndClass.cbWndExtra = 0;							//���ڵĸ����ڴ棬��Ȼȡ0������
	wndClass.hInstance = hInstance;						//ָ���������ڹ��̵ĳ����ʵ�������
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);  //���ؼ����Զ���icoͼ��
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);    //ָ��������Ĺ������
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);  //ΪhbrBackground��Աָ��һ����ɫ��ˢ���	
	wndClass.lpszMenuName = NULL;						//��һ���Կ���ֹ���ַ�����ָ���˵���Դ�����֡�
	wndClass.lpszClassName = L"GDIcore";		//��һ���Կ���ֹ���ַ�����ָ������������֡�

	//��2��ע�ᴰ����
	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	//��3����ʽ��������
	HWND hwnd = CreateWindow(L"GDIcore", WINDOW_TITLE,		//�������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//��4�����ڵ��ƶ�����ʾ�����
	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);		//����ShowWindow��������ʾ����
	UpdateWindow(hwnd);				//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����
		return FALSE;
	}
	//PlaySound(L"GameMedia\\music.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); 

	//��5����Ϣѭ������
	MSG msg = { 0 };				//���岢��ʼ��msg
		//bool bFrist = 1;

		//DWORD tFixedCostTime;		//ÿ��fixed֡����ʱ��
	DWORD originFixedDeltaTime = fixedDeltaTime;	//����趨��fixedDeltaTime

	DWORD tFixedStart =0;	//fixed֡��ʼʱ��
	DWORD tFixedEnd =0;	//fixed֡����ʱ��
	//int tt = 0;		//��������ʱ����

	g_tNow = GetTickCount();   //��ȡ��ǰϵͳʱ��
	

	Game_ObjectInSence();

	while (msg.message != WM_QUIT)		//ʹ��whileѭ���������Ϣ����WM_QUIT��Ϣ���ͼ���ѭ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			g_tPre = g_tNow;
			g_tNow = GetTickCount();   //��ȡ��ǰϵͳʱ��
			deltaTime = g_tNow - g_tPre;


			Game_ObjectStart();		//�����ʼ��


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
				Game_Physics();		//�����˶�����ײ���
				Game_Collision();	//��ײ����
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
				//g_tFixPre��¼�ϴ�Ӧִ��fixupdate��ʱ�����ʵ��ִ��ʱ�䣬��ʵ��ĳִ֡�н���ʱ���ٲ�֡

				//g_tFixedNow = GetTickCount();
				if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
				{
					TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
					DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
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
	//��6���������ע��
	UnregisterClass(L"GDIcore", wndClass.hInstance);  //����׼��������ע��������
	return 0;
}


	//-----------------------------------��WndProc( )������--------------------------------------
//	���������ڹ��̺���WndProc,�Դ�����Ϣ���д���
//------------------------------------------------------------------------------------------------
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)						//switch��俪ʼ
		{
		case WM_PAINT:						// ���ǿͻ����ػ���Ϣ
			ValidateRect(hwnd, NULL);		// ���¿ͻ�������ʾ
			break;									//������switch���

		case WM_KEYDOWN:                // ���Ǽ��̰�����Ϣ
			switch (wParam)
			{
			case VK_ESCAPE:           //���¡�Esc����

				DestroyAll();
				DestroyWindow(hwnd);    // ���ٴ���, ������һ��WM_DESTROY��Ϣ
				PostQuitMessage(0);  //��������
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

		case WM_LBUTTONDOWN:			//��������Ϣ
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
		case WM_DESTROY:				//���Ǵ���������Ϣ
			PostQuitMessage(0);		//��ϵͳ�����и��߳�����ֹ����������ӦWM_DESTROY��Ϣ
			break;								//������switch���

		default:									//������case�����������ϣ���ִ�и�default���
			return DefWindowProc(hwnd, message, wParam, lParam);		//����ȱʡ�Ĵ��ڹ���
		}
		return 0;			//�����˳�
	}




	BOOL Game_Init(HWND hwnd)
	{

		HBITMAP bmp;

		//��������ϵ�Ĵ���
		g_hdc = GetDC(hwnd);
		g_mdc = CreateCompatibleDC(g_hdc);  //����һ����hdc���ݵ��ڴ�DC
		g_bufdc = CreateCompatibleDC(g_hdc);//�ٴ���һ����hdc���ݵĻ���DC
		bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT); //��һ���ʹ��ڼ��ݵĿյ�λͼ����

		SelectObject(g_mdc, bmp);//����λͼ����ŵ�mdc��

		g_hBackGround = (HBITMAP)LoadImage(NULL, L"BackGround.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);

		//��ָ��Ŀ¼����������ͼ
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
		

		//���ƹ���ƶ�����
		
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

		Game_Paint(hwnd);  //����һ����ϷMain����
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

	void Game_ObjectInSence()	//��ʼ����
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

	void Game_ObjectStart()		//ÿһ֡��ʼ���ã�������һ֡����������ĳ�ʼ��
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

	void Game_FixedUpdate()		//ÿ��fixedDeltaTime����
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

		//��ײ���		
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
						onCollisionObject.emplace(t, j);	//��ӵ���������ײ����
				}
			}
		}
		

	}

	void Game_Collision()	//��ײ����
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

		onCollisionObject.clear();	//�����Ҫ��ײ���������
	}

	void Game_Update()	//ÿ֡����
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
			if (sprite != nullptr)		//������ͼ
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

