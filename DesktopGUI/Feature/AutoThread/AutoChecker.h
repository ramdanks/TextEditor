#pragma once
#include "AutoThread.h"

class AutoChecker : public AutoThread
{
public:
	void Routine() override;
};