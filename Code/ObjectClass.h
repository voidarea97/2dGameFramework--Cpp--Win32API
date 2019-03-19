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


class GAMEOBJECT_implement									//OBJECT物体类
{
public:	
	GAMEOBJECT_implement(int _id,VECTOR2 _pos=VECTOR2(0,0),string _name="object"):ID(_id),name(_name)
	{	
		//components["transform"]= make_shared<TRANSFORM_implement>(_pos);
		transform =  make_shared<TRANSFORM_implement>(this,_pos);
		//transform->gameObject = this;
	} 
	~GAMEOBJECT_implement()
	{

	}

	//特殊组件
	TRANSFORM transform = nullptr;;
	SPRITE sprite = nullptr;
	COLLIDER collider = nullptr;
	RIGIDBODY rigidbody = nullptr;

	//bool exist;


	string name="";
	int ID;
	//一般组件
	unordered_map<string, COMPONENT> components;

	friend void Game_ObjectInSence();
	friend void Game_ObjectStart();
	friend void Game_FixedUpdate();
	friend void Game_Physics();
	friend void Game_Collision();
	friend void Game_Update();

	friend COMPONENT_implement;

	bool AddComponent(COMPONENT component);
	

	//void AddSprite(VECTOR2 _pos, int _width, int _height, string _name);	//为物体添加Sprite
	//void AddCollider();
	//void AddRigidBody();
	//void DeleteSprite();
	//void DeleteCollider();
	//void DeleteRigidBody();

protected:
	void Start();
	void Update();
	void FixedUpdate();

	//virtual void OnPhysics();
	virtual void OnCollision(GAMEOBJECT);

	virtual void Destroy();
};

class COMPONENT_implement
{
public:
	//COMPONENT_implement(){}
	COMPONENT_implement(string _name):name(_name)
	{}
	COMPONENT_implement(GAMEOBJECT_implement* _gameObject,string _name)
		:gameObject(_gameObject),name(_name)
	{
		
	}
	string name;	//组件名称
	GAMEOBJECT_implement *gameObject;	//组件挂载的物体对象

	friend GAMEOBJECT_implement;
protected:
	virtual void Update() {};
	virtual void FixedUpdate() {};
	virtual void Start() {};
	virtual void OnDestroy() {};
};

class TRANSFORM_implement : public COMPONENT_implement
{
public:
	TRANSFORM_implement(GAMEOBJECT_implement *_gameObject, VECTOR2 _pos)
		:COMPONENT_implement(_gameObject,"Transform"),position(_pos){}
	VECTOR2 position;

};

class SPRITE_implement : public COMPONENT_implement
{
public:
	SPRITE_implement(VECTOR2 _pos, int _width, int _height, string _name)
		:COMPONENT_implement("Sprite"), position(_pos), width(_width), height(_height), sourceName(_name)
	{

	}
	~SPRITE_implement()
	{

	}
	VECTOR2 position;
	int width, height;
	string sourceName;
};

class COLLIDER_implement : public COMPONENT_implement
{
public:

	COLLIDER_implement(VECTOR2 _pos, int _width, int _height)
		:COMPONENT_implement("Collider"), offset(_pos), width(_width), height(_height)
	{

	}
	~COLLIDER_implement()
	{

	}
	void CollisionCall(GAMEOBJECT);
	//GAMEOBJECT parentObject;
	VECTOR2 offset;
	int height, width;
};

class RIGIDBODY_implement : public COMPONENT_implement
{
public:
	RIGIDBODY_implement( VECTOR2 _vol, VECTOR2 _acc)
		:COMPONENT_implement("Rigidbody"), velocity(_vol),acceleration(_acc)
	{
		
	}
	~RIGIDBODY_implement()
	{

	}
	void Move();
	//GAMEOBJECT parentObject;
	VECTOR2 velocity;
	VECTOR2 acceleration;
};