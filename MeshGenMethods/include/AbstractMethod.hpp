#pragma once

#include <memory>

#include "BSplineSurface.hpp"

class AbstractMethod
{
public:
    virtual ~AbstractMethod(){}
    virtual void GenMesh(std::shared_ptr<BSplineSurface> surface) = 0;
protected:
    virtual void GenInitialPack() = 0;
};
