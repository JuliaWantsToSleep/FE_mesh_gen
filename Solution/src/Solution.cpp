#include "Solution.hpp"

void Solution::SetTranslator(std::shared_ptr<AbstractTranslator> translator)
{
    _translator = translator;
}

void Solution::SetGenMethod(std::shared_ptr<AbstractMethod> method)
{
    _gen_method = method;
}

RetCode Solution::Solve()
{
    try
    {
        std::vector<std::shared_ptr<BSplineSurface>> surfaces;
        _translator->Translate(surfaces);
        if (surfaces.empty())
        {
            std::cout << "Nothing to generate, no surfaces found.\n";
            return TRANSLATION_ERR;
        }
        else
        {
            for (const auto& surface : surfaces)
            {
                _gen_method->GenMesh(surface);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
        return GENERATION_ERR;
    }
    return OK;

}
