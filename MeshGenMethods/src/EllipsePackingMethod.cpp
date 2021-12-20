#include <algorithm>
#include <cmath>
#include <forward_list>
#include <list>
#include <memory>

#include "EllipsePackingMethod.hpp"

void EllipsePackingMethod::GenInitialPack(const opencascade::handle
                                          <BSplineSurface> &surface)
{
    //TODO: Как подобрать начальные точки?
    //    gp_Pnt2d centre(1., 1.);
    //    gp_Pnt2d O(0., 0.);
    //    double centre_x = centre.X(), centre_y = centre.Y();
    //    double dist_O = centre.Distance(O);
    //    if (_bounds.HavePointIn(centre_x, centre_y))
    //    {
    //        SurfaceMetric::EllypticRepr M;
    //        surface->Metric(centre_x, centre_y, M);

    //    }
}

double EllipsePackingMethod::EvalH(const Ellipse &e1, const Ellipse &e2) const
{
    //Вектор, сонаправленный с lambda.
    gp_Vec2d lambda{e1.metric_data.lambda*cos(e1.metric_data.teta) + e1.C.X(),
                e1.metric_data.mu*sin(e1.metric_data.teta) + e1.C.Y()};
    //Вектор С1С2
    gp_Vec2d d_vec{e2.C.X() - e1.C.X(), e2.C.Y() - e1.C.Y()};
    double d = d_vec.Magnitude();

    //Квадрат косинуса угла между h1 и lambda
    double cos2_a_t = std::pow(lambda.Dot(d_vec)/(lambda.Magnitude()*d), 2);
    double sin2_a_t = 1 - cos2_a_t;

    return e1.metric_data.lambda*e1.metric_data.lambda*cos2_a_t +
            e1.metric_data.mu*e1.metric_data.mu*sin2_a_t;
}

double EllipsePackingMethod::EvalProximity(const Ellipse &e1,
                                           const Ellipse &e2) const
{
    double h1 = EvalH(e1, e2), h2 = EvalH(e2, e1);
    double d = e2.C.Distance(e1.C);
    double l = (h1 + h2)/d;
    return std::min(l, 1/l);
}

void EllipsePackingMethod::GetInitPoint(gp_Pnt2d& C, const Ellipse& e1,
                                        const Ellipse& e2)
{
    //TODO
}

bool EllipsePackingMethod::CheckIntersections(const Ellipse &to_check,
                                              std::forward_list<Ellipse>
                                              ::iterator &C_left,
                                              std::forward_list<Ellipse>
                                              ::iterator &C_right)
{
    bool found = false;
    auto intersect = [&, to_check](const Ellipse& elem){
        double f = elem.C.Distance(to_check.C) -
                (EvalH(to_check, elem) + EvalH(elem, to_check));
        return f < 0;
    };

    auto front_begin = _front.begin(), front_end = _front.end();
    auto iter = std::find_if(front_begin, C_left, intersect);
    if (iter != front_end)
    {
        found = true;
        C_left = iter;
    }
    while (iter != front_end)
    {
        iter = std::find_if(iter, front_end, intersect);
        if (iter != front_end)
        {
            found = true;
            C_right = iter;
        }
    }
    return found;
}

void EllipsePackingMethod::FitOne(Ellipse &to_fit, const Ellipse &C1,
                                  const Ellipse &C2)
{
    //Находим приближение для начальной точки
    GetInitPoint(to_fit.C, C1, C2);

    double proximity_lvl = 0.9, dx = 0.01, dy = 0.01, x, y;
    std::list<int> deltas = {10, 8, 6, 4, 2, 1};
    auto delta_it = deltas.begin();
    double beta = EvalProximity(C1, to_fit)*EvalProximity(to_fit, C2);
    while(beta < proximity_lvl)
    {
        x = to_fit.C.X();
        y = to_fit.C.Y();
        to_fit.C.SetX(x + dx);
        double d_beta_x = beta - EvalProximity(C1, to_fit)*
                EvalProximity(to_fit, C2);
        to_fit.C = {x, y + dy};
        double d_beta_y = beta - EvalProximity(C1, to_fit)*
                EvalProximity(to_fit, C2);
        to_fit.C.SetY(y);

        //Move C along the direction d_beta_x/dx, d_beta_y/dy
        double new_x = d_beta_x/dx, new_y = d_beta_y/dy;
        double alpha = (to_fit.metric_data.lambda + to_fit.metric_data.mu)
                *(*delta_it)*0.01/std::sqrt(new_x*new_x + new_y*new_y);
        to_fit.C = {x + new_x*alpha, y + new_y*alpha};
        if (std::next(delta_it) != deltas.end())
            ++delta_it;
    }

}

void EllipsePackingMethod::FitEllipses(const opencascade::handle
                                       <BSplineSurface> &surface)
{
    using Metric = SurfaceMetric::EllypticRepr;

    GenInitialPack(surface);

    //C1, C2
    auto C_i_it(_front.begin()),C_j_it(++_front.begin());

    gp_Pnt2d C;

    double term_val = std::min(std::abs(_bounds.U2 - _bounds.U1)/2,
                               std::abs(_bounds.V2 - _bounds.V1)/2);
    while(C_i_it->dist_to_O < term_val)
    {
        Metric M;
        surface->Metric(C.X(), C.Y(), M);
        Ellipse to_fit{C, C.Distance({0,0}), M};
        FitOne(to_fit, *C_i_it, *C_j_it);

        //Если полученный эллипс пересекается с другими, то выталкиваем его
        //С_i_it, C_j_it при этом меняются
        while(CheckIntersections(to_fit, C_i_it, C_j_it))
        {
            FitOne(to_fit, *C_i_it, *C_j_it);
        }
        //Удаляем из фронта генерации элементы, которые перекрывались эллипсом
        //Если C_i и C_j являются соседними, то ничего не удалится
        _front.erase_after(C_i_it, C_j_it);
        //Добавляем эллипс во фронт генерации
        _front.insert_after(C_i_it, to_fit);
        // Ищем элемент с минимальнм расстоянием до точки O
        C_i_it = std::min_element(_front.begin(), _front.end(),
                                  [](const Ellipse& lhs, const Ellipse& rhs)
        {return lhs.dist_to_O < rhs.dist_to_O;});
        C_j_it = std::next(C_i_it);
    }

}

void EllipsePackingMethod::GenMesh(const opencascade::handle
                                   <BSplineSurface> &surface) const
{

}
