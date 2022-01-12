#pragma once

#include <algorithm>

#include <gp_Pnt2d.hxx>

#include "SurfaceMetric.hpp"

struct SurfaceBounds
{
    double U1;
    double U2;
    double V1;
    double V2;
    bool HavePointIn(double U, double V)
    {
        return (U <= U2) && (U >= U1) && (V <= V2) && (V >= V1);
    }
    double MaxRadius()
    {
        return std::min(std::abs(U2 - U1)/2, std::abs(V2 - V1)/2);
    }
};

struct Ellipse
{
    gp_Pnt2d C;
    double dist_to_O;
    SurfaceMetric::EllypticRepr metric_data;
};
