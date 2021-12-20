#include <cmath>
#include <eigen3/Eigen/Eigenvalues>

#include "SurfaceMetric.hpp"
SurfaceMetric::SurfaceMetric(const gp_Vec &Du, const gp_Vec &Dv)
{
    //Вычисление метрической матрицы (M11=u*u, M12=M21=u*v, M22=v*v;
    //u,v - координатные векторы на касательной плоскости)
    _matrix_rep[0][0] = Du.Dot(Du);
    _matrix_rep[0][1] = _matrix_rep[1][0] = Du.Dot(Dv);
    _matrix_rep[1][1] = Dv.Dot(Dv);

    double Du_len = Du.Magnitude(), Dv_len = Dv.Magnitude();
    double angle = Du.Angle(Dv);

    //Вычисление матрицы оператора преобразования в метрическое пространство
    _A[0][0] = Du_len;
    _A[0][1] = Dv_len*std::cos(angle);
    _A[1][0] = 0.;
    _A[1][1] = Dv_len*std::sin(angle);
}

SurfaceMetric::EllypticRepr SurfaceMetric::GetEllypticRepr() const
{
    Eigen::Matrix<double, 2, 2> M;
    M << _matrix_rep[0][0], _matrix_rep[0][1],
                  _matrix_rep[1][0], _matrix_rep[1][1];

    Eigen::Matrix<double, 2, 2> A;
    A << _A[0][0], _A[0][1], _A[1][0], _A[1][1];

    //Вычисление метрического тензора в параметрическом пространстве
    Eigen::Matrix<double, 2, 2> M_u_v = A * M * A.transpose();

    Eigen::EigenSolver<Eigen::Matrix<double, 2, 2>> solv(M_u_v);

    //Матрица из собственных векторов (по столбцам), тип координат которых std::complex
    auto compl_eigenvecs = solv.eigenvectors().matrix();
    auto eigenvals = solv.eigenvalues().matrix();
    double val1 = eigenvals[0].real(), val2 = eigenvals[1].real();
    double lambda = std::max(val1, val2), mu = std::min(val1, val2);
    double angle = std::acos(compl_eigenvecs(0,0).real());

//    //Матрица поворота
//    Eigen::Matrix<double, 2, 2> R;
//    R << compl_eigenvecs(0, 0).real(), compl_eigenvecs(1, 0).real(),
//         compl_eigenvecs(0, 1).real(), compl_eigenvecs(1, 1).real();
//    //транспонированная матрица поворота
//    Eigen::Matrix<double, 2, 2> R_t = R.transpose();

//    //Найдем матрицу растяжений M = R_t*U*U*R => U^2 = R_t^(-1) * M * R^(-1)

//    Eigen::Matrix<double, 2, 2> U;
//    U << 2/std::sqrt(lambda), 0, 0, 2*mu*mu;

//    //Eigen::Matrix<double, 2, 2> U = R_t.inverse() * M * R.inverse();
//    if(!U.isDiagonal())
//    {
//        std::cerr << "WARNING: Non-diagonal stretch matrix U."
//                     " Further calculations may be incorrect."
//                  << std::endl;
//    }
    return {lambda, mu, angle};
}
