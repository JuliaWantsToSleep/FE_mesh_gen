#pragma once
#include <memory>
#include <string>
#include <vector>
#include "BSplineSurface.hpp"

class AbstractTranslator
{
public:
    virtual ~AbstractTranslator(){}
    virtual void Translate(const std::string& path, std::vector<std::shared_ptr
                           <BSplineSurface>>& surfaces) const = 0;
};

class StepTranslator : public AbstractTranslator
{
public:
    void Translate(const std::string& path, std::vector<std::shared_ptr
                   <BSplineSurface>>& surfaces) const override;
};

class IgesTranslator : public AbstractTranslator
{
    //TODO
};
