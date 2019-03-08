#pragma once
#include "BaseClass.h"
#include <memory> 
#include <string>

using namespace std;

#define OBJECT shared_ptr<OBJECT_implement>
#define SPRITE shared_ptr<SPRITE_implement>
#define COLLIDER shared_ptr<COLLIDER_implement>
#define RIGIDBODY shared_ptr<RIGIDBODY_implement>

#define wOBJECT weak_ptr<OBJECT_implement>
#define wSPRITE weak_ptr<SPRITE_implement>
#define wCOLLIDER weak_ptr<COLLIDER_implement>
#define wRIGIDBODY weak_ptr<RIGIDBODY_implement>


class OBJCT_implement;
class COLLIDER_implement;
class RIGIDBODY_implement;
class SPRITE_implement;


class OBJECT_implement									//OBJECTŒÔÃÂ¿‡
{
public:

	OBJECT_implement()
	{	}
	VECTOR2 position;
	SPRITE sprite;
	COLLIDER collider;
	RIGIDBODY rigidbody;
	//bool exist;
	string name;
	int ID;

	virtual ~OBJECT_implement()
	{
		
	}
	friend void Game_ObjectStart();
	friend void Game_FixedUpdate();
	friend void Game_Physics();
	friend void Game_Collision();
	friend void Game_Update();

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
	SPRITE_implement(VECTOR2 iPosition, int iWidth, int iHeight, string iPath)
		:position(iPosition), width(iWidth), height(iHeight), sourceName(iPath)
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

	void CollisionCall(void(*OnCollision)(void), OBJECT);

	VECTOR2 offset;
	int height, width;
};

class RIGIDBODY_implement
{
public:
	RIGIDBODY_implement(VECTOR2 iV, VECTOR2 iA):velocity(iV),acceleration(iA)
	{
		
	}

	void PhysicsCall(void(*OnPhysics)(void));

	VECTOR2 velocity;
	VECTOR2 acceleration;
};