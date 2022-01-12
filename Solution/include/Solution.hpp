#pragma once

#include <memory>

#include "AbstractMethod.hpp"
#include "AbstractTranslator.hpp"

enum RetCode
{
    OK,
    INPUT_ERR,
    TRANSLATION_ERR,
    GENERATION_ERR
};

class Solution
{
private:
    std::shared_ptr<AbstractTranslator> _translator;
    std::shared_ptr<AbstractMethod> _gen_method;
public:
    Solution(std::shared_ptr<AbstractTranslator> translator = nullptr,
             std::shared_ptr<AbstractMethod> method = nullptr):
        _translator(translator), _gen_method(method){}
    void SetTranslator(std::shared_ptr<AbstractTranslator> translator);
    void SetGenMethod(std::shared_ptr<AbstractMethod> method);
    RetCode Solve();

};
