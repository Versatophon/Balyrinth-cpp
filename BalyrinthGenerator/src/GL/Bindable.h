#pragma once

class Bindable
{
public:
	virtual void Bind() const = 0;
	virtual void Debind() const = 0;
};
