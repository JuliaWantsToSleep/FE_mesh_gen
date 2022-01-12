#include "BSplineSurface.hpp"

SurfaceMetric::EllypticRepr BSplineSurface::Metric(double U, double V) const
{
    gp_Vec Du, Dv;
    gp_Pnt point;

    D1(U, V, point, Du, Dv);

    return SurfaceMetric(Du, Dv).GetEllypticRepr();
}
