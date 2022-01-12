#pragma once

#include <forward_list>
#include <memory>
#include <optional>

#include "AbstractMethod.hpp"
#include "DataStructs.hpp"

class EllipsePackingMethod : public AbstractMethod
{
private:
    std::shared_ptr<BSplineSurface> _surface;
    std::forward_list<Ellipse> _front;
    SurfaceBounds _bounds;
    //Рассчитывает угол наклона касательной к эллипсу из точки О(0,0)
    //phi1, phi2 ограничивают арку эллипса
    //Возвращает std::nullopt если угол не найден
    std::optional<double> Tangent(const Ellipse& e1, double phi1, double phi2);
    [[nodiscard]] std::optional<Ellipse> EvalC2(const Ellipse& C1);
    [[nodiscard]] std::optional<Ellipse> EvalC1();
    void GenInitialPack() override;
    double EvalH(const Ellipse &e1, const Ellipse &e2) const;
    double EvalProximity(const Ellipse &e1, const Ellipse &e2) const;
    [[nodiscard]] std::optional<gp_Pnt2d> GetInitPoint(const Ellipse& e1, const Ellipse& e2);
    bool CheckIntersections(const Ellipse& to_check,
                            std::forward_list<Ellipse>::iterator& C_left,
                            std::forward_list<Ellipse>::iterator& C_right);
    [[nodiscard]] Ellipse FitOne(const Ellipse& C1, const Ellipse& C2);
    void FitEllipses();
public:
    void SetSurface(std::shared_ptr<BSplineSurface> surface);
    void GenMesh(std::shared_ptr<BSplineSurface> surface) override;
};
