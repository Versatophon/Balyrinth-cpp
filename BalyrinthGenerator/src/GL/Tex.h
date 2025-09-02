#pragma once

#include "Bindable.h"
#include "../Vec2.h"

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
	Vec2u Size;
};
