#include "Binder.h"

Binder::Binder(const Bindable& pBindable) :
	mBindable(pBindable)
{
	mBindable.Bind();
}

Binder::~Binder()
{
	mBindable.Debind();
}
