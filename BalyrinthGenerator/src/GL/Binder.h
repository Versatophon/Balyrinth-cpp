#pragma once

#include "Bindable.h"

class Binder
{
public:
	Binder(const Bindable& pBindable);

	~Binder();

private:
	const Bindable& mBindable;
};
