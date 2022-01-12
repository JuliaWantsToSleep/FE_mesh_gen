#pragma once
#include <memory>
#include <string>
#include <vector>
#include "BSplineSurface.hpp"

class AbstractTranslator
{
protected:
    std::string _path;
public:
    AbstractTranslator(const std::string& path):_path(path){}
    virtual ~AbstractTranslator(){}
    virtual void Translate(std::vector<std::shared_ptr
                           <BSplineSurface>>& surfaces) const = 0;
};
