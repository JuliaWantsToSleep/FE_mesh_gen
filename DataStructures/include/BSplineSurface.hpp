#pragma once

#include <optional>

#include <Geom_BSplineSurface.hxx>

#include "SurfaceMetric.hpp"

class BSplineSurface : public Geom_BSplineSurface
{
public:
    BSplineSurface(Geom_BSplineSurface&& surface):Geom_BSplineSurface(surface){}
    [[nodiscard]] SurfaceMetric::EllypticRepr Metric(double U, double V) const;
};
