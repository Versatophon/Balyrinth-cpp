#pragma once

#include "SimpleMathsApi.h"

#include <iostream>

#include "Vector3f.h"
#include "Quaternionf.h"

struct Matrix4f;

struct SIMPLE_MATHS_API Transformf
{
	Transformf(Vector3f const pPosition = Vector3f(), Quaternionf const pOrientation = Quaternionf(), float pScale = 1.f);
	~Transformf();
	
	void SetPosition(Vector3f const pPosition, Transformf const * pRelativeTransform = NULL);
	Vector3f GetPosition(Transformf const * pRelativeTransform = NULL) const;

	void SetOrientation(Quaternionf const pOrientation, Transformf const* pRelativeTransform = NULL);
	Quaternionf GetOrientation(Transformf const* pRelativeTransform = NULL) const;

	void SetScale(float const pScale, Transformf const* pRelativeTransform = NULL);
	float GetScale(Transformf const* pRelativeTransform = NULL) const;

	void SetTransform(Transformf pTransform, Transformf const* pRelativeTransform = NULL);
	Transformf GetTransform(Transformf const * pRelativeTransform = NULL);

	Transformf GetInverse();

	void Translate(Vector3f pTranslation, Transformf const* pRelativeTransform = NULL);
			
	void Rotate(Quaternionf pRotation, Transformf const* pRelativeTransform, Vector3f const& pCenter);
	void Rotate(Quaternionf pRotation, Transformf const* pRelativeTransform = NULL, Transformf const* pCenterTransform = NULL);
			
	void Rescale(float pScale, Vector3f const& pCenter);
	void Rescale(float pScale, Transformf const* pCenterTransform = NULL);

	Vector3f TransformLocalToGlobal(Vector3f const pLocalPostion);
	Vector3f TransformGlobalToLocal(Vector3f const pGlobalPostion);

	Matrix4f GetMatrix() const;

	Vector3f Position;
	Quaternionf Orientation;
	float Scale;

	static const Transformf Id;
};

SIMPLE_MATHS_API std::ostream& operator<<(::std::ostream& pStream, const Transformf& pTransform);

SIMPLE_MATHS_API bool operator==(Transformf const& pLeftTransform, Transformf const& pRightTransform);
SIMPLE_MATHS_API bool operator!=(Transformf const& pLeftTransform, Transformf const& pRightTransform);
