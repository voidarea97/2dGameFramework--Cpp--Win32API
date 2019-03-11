//Head File
#include "ObjectClass.h"

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


#pragma	comment(lib,"winmm.lib")			//����PlaySound����������ļ�
#pragma comment(lib,"Msimg32.lib")		//���ʹ��TransparentBlt��������Ŀ��ļ�

using namespace std;

//-----------------#define--------------------

#define WINDOW_WIDTH	800							//Ϊ���ڿ�ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڿ��
#define WINDOW_HEIGHT	600							//Ϊ���ڸ߶ȶ���ĺ꣬�Է����ڴ˴��޸Ĵ��ڸ߶�
#define WINDOW_TITLE	L"GAME DEMO"		//Ϊ���ڱ��ⶨ��ĺ�
#define PI				3.14159265f

class CONVERT
{
public:
	wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
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
	wstring s2ws(const std::string& s)
	{
		return Ansi2WChar(s.c_str(), s.size());
	}
};

//-----------------ȫ�ֱ���--------------------

HDC			g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;     //ȫ���豸���������ȫ���ڴ�DC���
DWORD		g_tFixPre = 0, g_tNow = 0, g_tPre = 0;			
RECT		g_rect;											//����һ��RECT�ṹ�壬���ڴ����ڲ��������������
HBITMAP		g_hBackGround;

CONTROLLER controller;

//��Ķ���

unordered_map<int, OBJECT> aliveObject;

unordered_map<int, OBJECT> onStartObject;

unordered_set<int> onPhysicsObject;
unordered_map<int, int> onCollisionObject;
unordered_set<int> onPaintingObject;

map<string, HBITMAP> resourceImageMap;	//ͼƬ���ƺ�ͼƬ�����map

int fixedDeltaTime = 20;
int deltaTime = 0;

int objectID = 0;

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



//-----------------------------------��WinMain( )������--------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//��1�����ڴ����Ĳ���֮һ����ʼ���һ�������Ĵ�����
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

												//��2�����ڴ����Ĳ���֮����ע�ᴰ����
	if (!RegisterClassEx(&wndClass))				//����괰�ں���Ҫ�Դ��������ע�ᣬ�������ܴ��������͵Ĵ���
		return -1;

	//��3�����ڴ����Ĳ���֮������ʽ��������
	HWND hwnd = CreateWindow(L"GDIcore", WINDOW_TITLE,		//ϲ���ּ��Ĵ������ں���CreateWindow
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH,
		WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	//��4�����ڴ����Ĳ���֮�ģ����ڵ��ƶ�����ʾ�����
	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);		//����������ʾʱ��λ�ã�ʹ�������Ͻ�λ�ڣ�250,80����
	ShowWindow(hwnd, nShowCmd);    //����ShowWindow��������ʾ����
	UpdateWindow(hwnd);						//�Դ��ڽ��и��£��������������·���Ҫװ��һ��

	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����
		return FALSE;
	}
	//PlaySound(L"GameMedia\\�λ�����ԭ��-ս��1-ɭ��.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //ѭ�����ű������� 

	//��5����Ϣѭ������
	MSG msg = { 0 };				//���岢��ʼ��msg
	//bool bFrist = 1;

	g_tNow = GetTickCount();   //��ȡ��ǰϵͳʱ��
	g_tFixPre = g_tNow;

	Game_ObjectInit();

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

			if (g_tNow - g_tFixPre >= fixedDeltaTime)        //���˴�ѭ���������ϴλ�ͼʱ�����0.06��ʱ�ٽ����ػ����
			{
				//Game_Main(hwnd);
				Game_FixedUpdate();
				Game_Physics();		//�����˶�����ײ���
				Game_Collision();	//��ײ����

				g_tFixPre += fixedDeltaTime;
			}
			Game_Update();
			Game_Paint(hwnd);
			//Game_Main(hwnd);
			Sleep(1);

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
		CONVERT convert;
		for (auto i : vImage)
		{
			//const wchar_t* wt = convert.s2ws(i.name).c_str();
			hbt = (HBITMAP)LoadImage(NULL, convert.s2ws("Image//"+i.name).c_str(), 
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

		ClipCursor(&g_rect);
		ShowCursor(0);

		Game_Paint(hwnd);  //����һ����ϷMain����
		return TRUE;
	}

	VOID Game_Paint(HWND hwnd)
	{
		SelectObject(g_bufdc, g_hBackGround);
		//BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

		for (auto i = onPaintingObject.begin();i!=onPaintingObject.end();i++)
		{
			if (aliveObject.find(*i)==aliveObject.end())
				continue;
			OBJECT obj = aliveObject[*i];
			auto sprite = obj->sprite;
			if (sprite != nullptr)		//������ͼ
			{

				SelectObject(g_bufdc, resourceImageMap[obj->sprite->sourceName]);
				BitBlt(g_mdc, obj->position.x + sprite->position.x - sprite->width/2.0f,
					obj->position.y + sprite->position.y - sprite->height / 2.0f,
					sprite->width, sprite->width, g_bufdc, 0, 0, SRCCOPY);

				//TransparentBlt(g_mdc, obj->position.x + sprite->position.x - sprite->width / 2.0f,
				//	obj->position.y + sprite->position.y - sprite->height / 2.0f,
				//	sprite->width, sprite->width,
				//	g_bufdc, 0, 0, sprite->width, sprite->width, RGB(255, 255, 255));
			}
			
		}

		BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
		//g_tFixPre += fixedDeltaTime;
		return VOID();
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

	void Game_ObjectInit()
	{
		//OBJECT o1(new OBJECT_implement(0));
		OBJECT o1 = CreateObject(VECTOR2(0, 0), "obj1");
		o1->AddSprite(VECTOR2(100, 100), 100, 100, "2.bmp");

		OBJECT o2 = CreateObject(VECTOR2(0, 0),"111");
		o2->AddSprite(VECTOR2(250, 250), 100, 100, "2.bmp");
		//onStartObject.emplace(0, o1);

		
	}

	void Game_ObjectStart()		//ÿһ֡��ʼ���ã�������һ֡����������ĳ�ʼ��
	{
		if (onStartObject.empty())
			return;
		for (auto i = onStartObject.begin();i!=onStartObject.end();i++)
		{
			i->second->Start();
			aliveObject.emplace(i->first, i->second);
			//if (i->second->sprite != nullptr)
				//onPaintingObject.insert(i->first);
			i->second = nullptr;
		}
		onStartObject.clear();
	}

	void Game_FixedUpdate()		//ÿ��fixedDeltaTime����
	{
		for (auto i = aliveObject.begin();i!=aliveObject.end();i++)
		{
			i->second->FixedUpdate();
		}
	}

	void Game_Physics()
	{
		if (onPhysicsObject.empty())
			return;
		for (auto i = onPhysicsObject.begin();i!=onPaintingObject.end();i++)
		{
			if (aliveObject.find(*i) == aliveObject.end())
				continue;
			if (aliveObject[*i]->rigidbody != nullptr)
				aliveObject[*i]->rigidbody->PhysicsCall();
		}


		//��ײ���		
		onCollisionObject.clear();	//�����Ҫ��ײ���������set

	}

	void Game_Collision()	//��ײ����Call
	{
		for (auto i = onCollisionObject.begin();i!=onCollisionObject.end();i++)
		{
			auto p1 = aliveObject.find(i->first);
			auto p2 = aliveObject.find(i->second);
			if (p1 == aliveObject.end() || p2 == aliveObject.end())
				continue;
			if (p1->second->collider == nullptr || p2->second->collider == nullptr)
				continue;
			p1->second->collider->CollisionCall(p2->second);
			p2->second->collider->CollisionCall(p1->second);
		}
	}

	void Game_Update()	//ÿ֡����
	{
		for (auto i = aliveObject.begin();i!=aliveObject.end();i++)
		{
			i->second->Update();
		}
	}

	OBJECT CreateObject(VECTOR2 _Pos, string _Name)
	{
		OBJECT obj = make_shared<OBJECT_implement>(objectID, _Pos, _Name);
		onStartObject.emplace(objectID, obj);
		objectID++;
		return obj;
	}

