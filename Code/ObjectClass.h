#pragma once
#include "BaseClass.h"
#include "GameGlobal.h"
//#include <memory> 
//#include <string>

using namespace std;
//
//#define OBJECT shared_ptr<OBJECT_implement>
//#define SPRITE shared_ptr<SPRITE_implement>
//#define COLLIDER shared_ptr<COLLIDER_implement>
//#define RIGIDBODY shared_ptr<RIGIDBODY_implement>
//
//#define wOBJECT weak_ptr<OBJECT_implement>
//#define wSPRITE weak_ptr<SPRITE_implement>
//#define wCOLLIDER weak_ptr<COLLIDER_implement>
//#define wRIGIDBODY weak_ptr<RIGIDBODY_implement>


//class OBJECT_implement;
//class COLLIDER_implement;
//class RIGIDBODY_implement;
//class SPRITE_implement;


class OBJECT_implement									//OBJECTŒÔÃÂ¿‡
{
public:

	OBJECT_implement(int _ID,VECTOR2 _Pos=VECTOR2(0,0),string _Name="object"):ID(_ID),position(_Pos),name(_Name)
	{	} 
	VECTOR2 position = { 0,0 };
	SPRITE sprite = nullptr;
	COLLIDER collider = nullptr;
	RIGIDBODY rigidbody = nullptr;
	//bool exist;
	string name="";
	int ID;
	
	virtual ~OBJECT_implement()
	{
		
	}
	friend void Game_ObjectInit();
	friend void Game_ObjectStart();
	friend void Game_FixedUpdate();
	friend void Game_Physics();
	friend void Game_Collision();
	friend void Game_Update();
	friend RIGIDBODY_implement;
	friend SPRITE_implement;
	friend COLLIDER_implement;

	void AddSprite(VECTOR2 _pos, int _width, int _height, string _Name);
	void AddCollider();
	void AddRigidBody();
	void DeleteSprite();
	void DeleteCollider();
	void DeleteRigidBody();

protected:
	virtual void Start();
	virtual void Update();
	virtual void FixedUpdate();

	virtual void OnPhysics();
	virtual void OnCollision(OBJECT);

	virtual void Destroy();
};

class SPRITE_implement
{
public:
	SPRITE_implement(VECTOR2 iPosition, int iWidth, int iHeight, string _Name)
		:position(iPosition), width(iWidth), height(iHeight), sourceName(_Name)
	{
		
	}
	~SPRITE_implement()
	{

	}
	VECTOR2 position;
	int width, height;
	string sourceName;
};

class COLLIDER_implement
{
public:

	COLLIDER_implement(VECTOR2 iPosition, int iWidth, int iHeight) :offset(iPosition), width(iWidth), height(iHeight)
	{

	}
	~COLLIDER_implement()
	{

	}
	void CollisionCall(OBJECT);
	OBJECT parentObject;
	VECTOR2 offset;
	int height, width;
};

class RIGIDBODY_implement
{
public:
	RIGIDBODY_implement(VECTOR2 iV, VECTOR2 iA):velocity(iV),acceleration(iA)
	{
		
	}
	~RIGIDBODY_implement()
	{

	}
	void PhysicsCall();
	OBJECT parentObject;
	VECTOR2 velocity;
	VECTOR2 acceleration;
};