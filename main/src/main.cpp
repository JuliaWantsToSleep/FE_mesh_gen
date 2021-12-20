#include <iostream>

#include "StepTranslator.hpp"

int main(int argc, char* argv[])
{
    std::vector<std::shared_ptr<BSplineSurface>> surf_vec;
    StepTranslator tr;
    tr.Translate("for_parser.step", surf_vec);
    return 0;
}
