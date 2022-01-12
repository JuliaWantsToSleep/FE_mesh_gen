#include <algorithm>
#include <cassert>
#include <cmath>
#include <forward_list>
#include <list>
#include <memory>
#include <random>

#include "EllipsePackingMethod.hpp"

std::optional<double> EllipsePackingMethod::Tangent(const Ellipse &e1,
                                     double phi1,
                                     double phi2)
{
    for (double psi = phi1; psi < phi2; psi += 0.1)
    {
        double x_local = e1.metric_data.lambda*std::cos(psi);
        double y_local = e1.metric_data.mu*std::sin(psi);
        if (std::abs(y_local) < 0.1) continue;

        //Из канонического уравнения эллипса: y'=-(a^2/b^2)*(x/y)
        double dy_local = -(e1.metric_data.mu*e1.metric_data.mu)
                /(e1.metric_data.lambda*e1.metric_data.lambda)
                *(x_local/y_local);

        //Обратное преобразование поворота на угол teta + сдвиг
        double x_global = std::cos(e1.metric_data.teta)*x_local
                        - std::sin(e1.metric_data.teta)*y_local + e1.C.X();
        double y_global = std::sin(e1.metric_data.teta)*x_local
                        + std::cos(e1.metric_data.teta)*y_local + e1.C.Y();

        //Угол наклона касательной в глобальной с.к.
        double global_angle = std::atan(dy_local) + e1.metric_data.teta;
        //Чтобы тангенс не обращался в бесконечность
        if (std::abs(global_angle - M_PI/2.) < 0.1) continue;
        //Если касательная проходит через точку О, то возвращаем угол наклона
        if (std::abs(y_global - x_global*tan(global_angle)) < 0.01)
        {
            return global_angle;
        }

    }
    return std::nullopt;
}

std::optional<Ellipse> EllipsePackingMethod::EvalC2(const Ellipse &C1)
{
    //Ограничиваем дальнейшую область поиска касательными к эллипсу С1 из О
    //Таких касательных может быть только две
    auto psi_start = Tangent(C1, 0, M_PI*2.);
    assert(psi_start);
    double psi_start_val = psi_start.value();

    auto psi_end = Tangent(C1, psi_start_val, M_PI*2.);
    assert(psi_end);
    double psi_end_val = psi_end.value();

    double term_val = _bounds.MaxRadius();
    double dr = 0.1;
    SurfaceMetric::EllypticRepr metric;
    Ellipse C2;
    for (double r = C1.dist_to_O; r <= term_val; r += dr)
    {
        for (double psi = psi_start_val; psi < psi_end_val; psi += 0.1)
        {
            double C_x = r*std::cos(psi), C_y = r*std::sin(psi);
            metric = _surface->Metric(C_x, C_y);
            C2 = {{C_x, C_y}, r, metric};
            if (std::abs(EvalProximity(C1, C2) - 1) < 0.01)
            {
                return C2;
            }
        }
    }
    return std::nullopt;
}

std::optional<Ellipse> EllipsePackingMethod::EvalC1()
{
    double r = 0.1, dr = 0.1;
    double C_x = r, C_y = 0;
    auto metric = _surface->Metric(C_x, C_y);

    Ellipse C1 {{C_x, C_y}, r, metric};
    Ellipse O {{0, 0}, 0, {0, 0, 0}};


    //Ищем точку C1 на окружности радиуса r
    //(Эллипс с центром в C1 не должен накладываться на O)
    for (; r <= EvalH(C1, O); r += dr)
    {
        for(double psi = 0.; psi < 2*M_PI; psi += 0.1)
        {
            //Координаты
            C_x = r*std::cos(psi);
            C_y = r*std::sin(psi);
            //Метрика
            metric = _surface->Metric(C_x, C_y);
            C1 = {{C_x, C_y}, r, metric};
            if (r <= EvalH(C1, O)) return C1;
        }
    }

    return std::nullopt;
}

void EllipsePackingMethod::GenInitialPack()
{

        auto C1_opt = EvalC1();
        //Предположительно первый эллипс должен находиться всегда
        //Если таковой не найден, значит есть ошибка в коде
        assert(C1_opt);
        Ellipse C1 = C1_opt.value();
        _front.push_front(C1);

        auto C2_opt = EvalC2(C1);
        assert(C2_opt);
        Ellipse C2 = C2_opt.value();
        _front.push_front(C2);
        _front.emplace_front(FitOne(C1, C2));

//        std::default_random_engine generator;
//        std::uniform_int_distribution<int> distribution(1,6);
//        int dice_roll = distribution(generator);
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

std::optional<gp_Pnt2d> EllipsePackingMethod::GetInitPoint(const Ellipse& e1,
                                                           const Ellipse& e2)
{
    //TODO
    return {};
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

Ellipse EllipsePackingMethod::FitOne(const Ellipse &C1,
                                     const Ellipse &C2)
{
    using Metric = SurfaceMetric::EllypticRepr;
    //Находим приближение для начальной точки
    auto init_point_opt = GetInitPoint(C1, C2);
    assert(init_point_opt);
    auto init_point = init_point_opt.value();

    Metric metric = _surface->Metric(init_point.X(), init_point.Y());
    Ellipse to_fit{init_point, init_point.Distance({0,0}), metric};

    double proximity_lvl = 0.9, dx = 0.01, dy = 0.01, x, y;
    std::list<int> deltas = {10, 8, 6, 4, 2, 1};
    auto delta_it = deltas.begin();
    //beta - коэффициент близости двух эллипсов (чем он больше, тем ближе)
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

    return to_fit;

}

void EllipsePackingMethod::FitEllipses()
{
    using Metric = SurfaceMetric::EllypticRepr;

    GenInitialPack();

    //C1, C2
    auto C_i_it{_front.begin()}, C_j_it{std::next(C_i_it)};

    gp_Pnt2d C;

    double term_val = std::min(std::abs(_bounds.U2 - _bounds.U1)/2,
                               std::abs(_bounds.V2 - _bounds.V1)/2);
    while(C_i_it->dist_to_O < term_val)
    {

        auto new_packed = FitOne(*C_i_it, *C_j_it);

        //Если полученный эллипс пересекается с другими, то выталкиваем его
        //С_i_it, C_j_it при этом меняются
        while(CheckIntersections(new_packed, C_i_it, C_j_it))
        {
            new_packed = FitOne(*C_i_it, *C_j_it);
        }
        //Удаляем из фронта генерации элементы, которые перекрывались эллипсом
        //Если C_i и C_j являются соседними, то ничего не удалится
        _front.erase_after(C_i_it, C_j_it);
        //Добавляем эллипс во фронт генерации
        _front.insert_after(C_i_it, new_packed);
        // Ищем элемент с минимальнм расстоянием до точки O
        C_i_it = std::min_element(_front.begin(), _front.end(),
                                  [](const Ellipse& lhs, const Ellipse& rhs)
                                  {return lhs.dist_to_O < rhs.dist_to_O;});
        C_j_it = std::next(C_i_it);
    }

}

void EllipsePackingMethod::SetSurface(std::shared_ptr<BSplineSurface> surface)
{
    _surface = surface;
}

void EllipsePackingMethod::GenMesh(std::shared_ptr<BSplineSurface> surface)
{
    SetSurface(surface);
    FitEllipses();
}
