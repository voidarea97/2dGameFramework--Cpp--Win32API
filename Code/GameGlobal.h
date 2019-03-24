#pragma once
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>
//
//#include <windows.h>
////#include <string>
////#include <vector>
////#include <map>
////#include <unordered_map>
////#include <unordered_set>
//#include <algorithm>
//#include <tchar.h>//ʹ��swprintf_s���������ͷ�ļ�
//#include <time.h> //ʹ�û�ȡϵͳʱ��time()������Ҫ������ͷ�ļ�
//#include <cmath>
//#include <sstream>
//#include <iostream>
//#include <stdlib.h>
//
//
//#pragma	comment(lib,"winmm.lib")			//����PlaySound����������ļ�
//#pragma comment(lib,"Msimg32.lib")		//���ʹ��TransparentBlt��������Ŀ��ļ�

using namespace std;

//���ļ�������Ϸȫ���ࡢ�����ͺ���

#define GAMEOBJECT shared_ptr<GAMEOBJECT_implement>
#define COMPONENT shared_ptr<COMPONENT_implement>
#define TRANSFORM shared_ptr<TRANSFORM_implement>
#define SPRITE shared_ptr<SPRITE_implement>
#define COLLIDER shared_ptr<COLLIDER_implement>
#define RIGIDBODY shared_ptr<RIGIDBODY_implement>

#define wGAMEOBJECT weak_ptr<GAMEOBJECT_implement>
#define wCOMPONENT weak_ptr<COMPONENT_implement>
#define wTRANSFORM weak_ptr<TRANSFORM_implement>
#define wSPRITE weak_ptr<SPRITE_implement>
#define wCOLLIDER weak_ptr<COLLIDER_implement>
#define wRIGIDBODY weak_ptr<RIGIDBODY_implement>

class GAMEOBJECT_implement;
class TRANSFORM_implement;
class COLLIDER_implement;
class RIGIDBODY_implement;
class SPRITE_implement;

class COMPONENT_implement;

void Game_ObjectInSence();

void Game_ObjectDestroy();
void Game_ObjectStart();
void Game_FixedUpdate();
void Game_Physics();
void Game_Collision();
void Game_Update();

GAMEOBJECT CreateObject(VECTOR2 _Pos, string _Name);

extern unordered_map<int, GAMEOBJECT> activeObject;

extern unordered_map<int, GAMEOBJECT> onStartObject;

extern unordered_set<int> onCollisionDetectionObject;;
extern unordered_set<int> onMovementObject;
extern unordered_map<int, int> onCollisionObject;
extern unordered_set<int> onPaintingObject;
extern unordered_set<int> onDestroyObject;


extern unsigned long fixedDeltaTime;
extern float deltaTime;


//////////////////�����������Զ���������////////////////////
extern class PLAYERCONTROLLER_implement;
#define PLAYERCONTROLLER shared_ptr<PLAYERCONTROLLER_implement>



//map<string, HBITMAP> resourceImageMap;	//ͼƬ���ƺ�ͼƬ�����map