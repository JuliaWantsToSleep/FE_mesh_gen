#pragma once
#include <array>

#include <gp_Vec.hxx>

class SurfaceMetric
{
private:
    std::array<std::array<double, 2>, 2> _matrix_rep;
    std::array<std::array<double, 2>, 2> _A;
public:
    struct EllypticRepr
    {
        double lambda;  //растяжение вдоль 1й координатной оси
        double mu;      //растяжение вдоль 2й координатной оси
        double teta;    //Угол поворота относительно координатных осей
    };
    explicit SurfaceMetric(const gp_Vec& Du, const gp_Vec& Dv);
    EllypticRepr GetEllypticRepr() const;
    std::array<std::array<double, 2>, 2> GetMatrixRepr() const
    {return _matrix_rep;}
};
