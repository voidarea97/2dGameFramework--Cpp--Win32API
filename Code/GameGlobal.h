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
//#include <tchar.h>//使用swprintf_s函数所需的头文件
//#include <time.h> //使用获取系统时间time()函数需要包含的头文件
//#include <cmath>
//#include <sstream>
//#include <iostream>
//#include <stdlib.h>
//
//
//#pragma	comment(lib,"winmm.lib")			//调用PlaySound函数所需库文件
//#pragma comment(lib,"Msimg32.lib")		//添加使用TransparentBlt函数所需的库文件

using namespace std;

//本文件声明游戏全局类、变量和函数

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


//////////////////此区域声明自定义的组件类////////////////////
extern class PLAYERCONTROLLER_implement;
#define PLAYERCONTROLLER shared_ptr<PLAYERCONTROLLER_implement>



//map<string, HBITMAP> resourceImageMap;	//图片名称和图片对象的map