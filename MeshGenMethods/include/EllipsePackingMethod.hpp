#pragma once

#include <forward_list>
#include <memory>

#include "AbstractMethod.hpp"
#include "DataStructs.hpp"

class EllipsePackingMethod : public AbstractMethod
{
private:
    std::forward_list<Ellipse> _front;
    SurfaceBounds _bounds;
    void GenInitialPack(const opencascade::handle
                        <BSplineSurface> &surface) override;
    double EvalH(const Ellipse &e1, const Ellipse &e2) const;
    double EvalProximity(const Ellipse &e1, const Ellipse &e2) const;
    void GetInitPoint(gp_Pnt2d& C, const Ellipse& e1, const Ellipse& e2);
    bool CheckIntersections(const Ellipse& to_check,
                            std::forward_list<Ellipse>::iterator& C_left,
                            std::forward_list<Ellipse>::iterator& C_right);
    void FitOne(Ellipse& to_fit, const Ellipse& C1, const Ellipse& C2);
    void FitEllipses(const opencascade::handle
                     <BSplineSurface> &surface);
public:
    void GenMesh(const opencascade::handle
                 <BSplineSurface>& surface) const override;
};
