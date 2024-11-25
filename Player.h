#pragma once
#include "CObject.h"
#include "Model.h"

class Player : public CObject {
public:
	virtual void Update() override;
};