#pragma once

#include "Bindable.h"

class Binder
{
public:
	inline Binder(const Bindable& pBindable):
		mBindable(pBindable)
	{
		mBindable.Bind();
	}

	inline ~Binder()
	{
		mBindable.Debind();
	}

private:
	const Bindable& mBindable;
};
