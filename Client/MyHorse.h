#pragma once
#include "Horse.h"

class MyHorse
	:public Horse
{
public:
	MyHorse();
	~MyHorse();
public:
	virtual void update()override;
	virtual void component_update()override;
};

