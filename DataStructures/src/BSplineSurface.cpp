#include "BSplineSurface.hpp"

void BSplineSurface::Metric(double U, double V,
                            SurfaceMetric::EllypticRepr &e_repr) const
{
    gp_Vec Du, Dv;
    gp_Pnt point;
    D1(U, V, point, Du, Dv);
    e_repr = SurfaceMetric(Du, Dv).GetEllypticRepr();
}
