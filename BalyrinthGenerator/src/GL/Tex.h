#pragma once

#include "Bindable.h"

#include <Vector2us.h>

#include <cstdint>
#include <string>

class Tex:public Bindable
{
public:
	void Bind() const override;
	void Debind() const override;

	void Generate();
	void Destroy();

private:
	uint32_t Id = 0;
	Vector2us Size;
	mutable uint32_t mPreviousId = 0;
};
