#pragma once
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace std;

//本文件声明游戏全局类、变量和函数

#define OBJECT shared_ptr<OBJECT_implement>
#define SPRITE shared_ptr<SPRITE_implement>
#define COLLIDER shared_ptr<COLLIDER_implement>
#define RIGIDBODY shared_ptr<RIGIDBODY_implement>

#define wOBJECT weak_ptr<OBJECT_implement>
#define wSPRITE weak_ptr<SPRITE_implement>
#define wCOLLIDER weak_ptr<COLLIDER_implement>
#define wRIGIDBODY weak_ptr<RIGIDBODY_implement>

class OBJECT_implement;
class COLLIDER_implement;
class RIGIDBODY_implement;
class SPRITE_implement;

void Game_ObjectInit();

void Game_ObjectStart();
void Game_FixedUpdate();
void Game_Physics();
void Game_Collision();
void Game_Update();

OBJECT CreateObject(VECTOR2 _Pos, string _Name);

extern unordered_map<int, OBJECT> aliveObject;

extern unordered_map<int, OBJECT> onStartObject;

extern unordered_set<int> onPhysicsObject;
extern unordered_map<int, int> onCollisionObject;
extern unordered_set<int> onPaintingObject;

//map<string, HBITMAP> resourceImageMap;	//图片名称和图片对象的map