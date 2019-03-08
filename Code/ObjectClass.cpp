#include "ObjectClass.h"

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

void COLLIDER_implement::CollisionCall(void(*OnCollision)(void), OBJECT)
{
	OnCollision();
}

void RIGIDBODY_implement::PhysicsCall(void(*OnPhysics)(void))
{
	OnPhysics();
}
