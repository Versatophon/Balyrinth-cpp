#include "Transformf.h"

#include "Matrix4f.h"

Transformf::Transformf(Vector3f const pPosition, Quaternionf const pOrientation, float pScale) :Position(pPosition), Orientation(pOrientation), Scale(pScale)
{
}

Transformf::~Transformf()
{
}

void Transformf::SetPosition(Vector3f const pPosition, Transformf const* pRelativeTransform)
{
	Position = pRelativeTransform != nullptr 
			 ? (pRelativeTransform->Position * pRelativeTransform->Scale) + pRelativeTransform->Orientation.Transform(pPosition)
			 : pPosition;
}

Vector3f Transformf::GetPosition(Transformf const* pRelativeTransform) const
{
	return pRelativeTransform != nullptr
		 ? pRelativeTransform->Orientation.Inverted().Transform((Position - pRelativeTransform->Position) / pRelativeTransform->Scale)
		 : Position;
}

void Transformf::SetOrientation(Quaternionf const pOrientation, Transformf const* pRelativeTransform)
{
	Orientation = (pRelativeTransform != nullptr ? pRelativeTransform->Orientation * pOrientation : pOrientation);
}

Quaternionf Transformf::GetOrientation(Transformf const* pRelativeTransform) const
{
	return pRelativeTransform!= nullptr ? 
		   Orientation.GetLocalOrientationFrom(pRelativeTransform->Orientation) : Orientation;
}

void Transformf::SetScale(float const pScale, Transformf const* pRelativeTransform)
{
	Scale = pScale * (pRelativeTransform != nullptr ? pRelativeTransform->Scale : 1.f);
}

float Transformf::GetScale(Transformf const* pRelativeTransform) const
{
	return Scale / (pRelativeTransform != nullptr ? pRelativeTransform->Scale : 1.f);
}

void Transformf::SetTransform(Transformf pTransform, Transformf const* pRelativeTransform)
{
	SetPosition(pTransform.Position, pRelativeTransform);
	SetOrientation(pTransform.Orientation, pRelativeTransform);
	SetScale(pTransform.Scale, pRelativeTransform);
}

Transformf Transformf::GetTransform(Transformf const* pRelativeTransform)
{
	return Transformf(GetPosition(pRelativeTransform), GetOrientation(pRelativeTransform), GetScale(pRelativeTransform));
}

void Transformf::Translate(Vector3f pTranslation, Transformf const* pRelativeTransform)
{
	const Transformf* lRelativeTransform = pRelativeTransform != nullptr ? pRelativeTransform : this;
	Position += lRelativeTransform->Orientation.Transform(pTranslation) * lRelativeTransform->Scale;
}

void Transformf::Rotate(Quaternionf pRotation, Transformf const* pRelativeTransform, Transformf const* pCenterTransform)
{
	Rotate(pRotation, pRelativeTransform, pCenterTransform != NULL ? pCenterTransform->GetPosition() : Position);
}

void Transformf::Rotate(Quaternionf pRotation, Transformf const* pRelativeTransform, Vector3f const& pCenter)
{
	Quaternionf lConversionQuat = (pRelativeTransform ? pRelativeTransform->Orientation : Orientation).Transform(pRotation);

	Orientation = (lConversionQuat * Orientation).Normalized();
	
	Vector3f lDiff = (Position - pCenter);
	Vector3f lRotated = lConversionQuat.Transform(lDiff);
	Position += (lRotated - lDiff);
}

void Transformf::Rescale(float pScale, Transformf const* pCenterTransform)
{
	Rescale(pScale, pCenterTransform != NULL ? (pCenterTransform->GetPosition()) : Position);
}

void Transformf::Rescale(float pScale, Vector3f const& pCenter)
{
	Position += (Position - pCenter) * (pScale - 1.f);

	Scale *= pScale;
}

Transformf Transformf::GetInverse()
{
	Transformf lTransform;
	return lTransform.GetTransform(this);
}

Vector3f Transformf::TransformLocalToGlobal(Vector3f const pLocalPostion)
{
	return Position + Orientation.Transform(pLocalPostion);
}

Vector3f Transformf::TransformGlobalToLocal(Vector3f const pGlobalPostion)
{
	Transformf lTransform;
	lTransform.SetPosition(pGlobalPostion);
	return lTransform.GetTransform(this).GetPosition();
}

Matrix4f Transformf::GetMatrix() const
{
	Matrix4f lMatrix;

	lMatrix.SetOrientation(Orientation);
	lMatrix.SetPosition(Position);
	lMatrix.SetScale(Scale);

	return lMatrix;
}

bool operator==(Transformf const& pLeftTransform, Transformf const& pRightTransform)
{
	return (pLeftTransform.Position == pRightTransform.Position) && (pLeftTransform.Orientation == pRightTransform.Orientation) && (pLeftTransform.Scale == pRightTransform.Scale);
}

bool operator!=(Transformf const& pLeftTransform, Transformf const& pRightTransform)
{
	return !(pLeftTransform == pRightTransform);
}

std::ostream& operator<<(::std::ostream& pStream, const Transformf& pTransform)
{
	pStream << "R( " << pTransform.Position << ", " << pTransform.Orientation << ", " << pTransform.Scale << " )";
	return pStream;
}

const Transformf Transformf::Id = {Vector3f::Zero, Quaternionf::Id, 1.f};
