#include <iostream>

#include "Solution.hpp"
#include "StepTranslator.hpp"
#include "EllipsePackingMethod.hpp"


int main(int argc, char* argv[])
{
    std::string input;
    if (argc == 0)
    {
        std::cout << "Starting with default surface\n";
        input = StepTranslator::GetDefaultPath();
    }
    else
    {
        input = argv[0];
        if (input.empty())
        {
            std::cerr << "Invalid input.\n";
            return INPUT_ERR;
        }
    }
    std::shared_ptr<StepTranslator> stp_translator =
            std::make_shared<StepTranslator>(input);
    std::shared_ptr<EllipsePackingMethod> pack_method =
            std::make_shared<EllipsePackingMethod>();
    Solution sol{stp_translator, pack_method};
    auto ret_code = sol.Solve();


    return ret_code;
}
