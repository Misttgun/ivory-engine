#pragma once

struct IComponent
{
protected:
	static int nextId;
};

template <typename T>
class Component: public IComponent
{
public:
	static int GetId()
	{
		return ++nextId;
	}
};

