#pragma once

#include "IMoveable.h"
#include "Transform.h"

class IMoveableTransform : public Transform, public IMoveable {
protected:
  virtual ~IMoveableTransform() {};
};
