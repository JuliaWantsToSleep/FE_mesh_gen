#pragma once

#include "BSplineSurface.hpp"

class AbstractMethod
{
public:
    virtual ~AbstractMethod(){}
    virtual void GenMesh(const opencascade::handle
                         <BSplineSurface>& surface) const = 0;
protected:
    virtual void GenInitialPack(const opencascade::handle
                                <BSplineSurface> &surface);
};
