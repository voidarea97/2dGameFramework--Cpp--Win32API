#include "ObjectClass.h"

void OBJECT_implement::AddSprite(VECTOR2 _pos,int _width,int _height,string _Name)
{
	sprite = make_shared<SPRITE_implement>(_pos, _width, _height, _Name);
	onPaintingObject.insert(this->ID);
}

void OBJECT_implement::AddCollider()
{
}

void OBJECT_implement::AddRigidBody()
{
}

void OBJECT_implement::DeleteSprite()
{
}

void OBJECT_implement::DeleteCollider()
{
}

void OBJECT_implement::DeleteRigidBody()
{
}

void OBJECT_implement::Start()
{
}

void OBJECT_implement::Update()
{
}

void OBJECT_implement::FixedUpdate()
{
}

void OBJECT_implement::OnCollision(OBJECT obj)
{
}

void OBJECT_implement::OnPhysics()
{
}

void OBJECT_implement::Destroy()
{
}

void COLLIDER_implement::CollisionCall(OBJECT another)
{
	parentObject->OnCollision(another);
}

void RIGIDBODY_implement::PhysicsCall()
{
	parentObject->OnPhysics();
}

