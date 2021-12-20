#pragma once

#include <Geom_BSplineSurface.hxx>

#include "SurfaceMetric.hpp"

class BSplineSurface : public Geom_BSplineSurface
{
public:
    BSplineSurface(Geom_BSplineSurface&& surface):Geom_BSplineSurface(surface){}
    void Metric(double U, double V,
                SurfaceMetric::EllypticRepr& e_repr) const;
};
