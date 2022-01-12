#pragma once
#include "AbstractTranslator.hpp"

class StepTranslator : public AbstractTranslator
{
private:
    static constexpr char _default_path[] = "default_surface.stp";
public:
    StepTranslator(const std::string& path = _default_path):
        AbstractTranslator(path){}
    void Translate(std::vector<std::shared_ptr
                   <BSplineSurface>>& surfaces) const override;
    static std::string GetDefaultPath(){return _default_path;}
};

class IgesTranslator : public AbstractTranslator
{
    //TODO
};
