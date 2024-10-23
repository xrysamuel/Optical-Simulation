#include "optics.h"

bool LightRay::Step()
{
    size_t nearest_i;
    IncidenceState nearest_s;
    bool intersect = false;
    for (size_t i = 0; i < deflectors_.size(); i++)
    {
        IncidenceState s = deflectors_[i]->Incidence(*this, ray_);
        if (i == excluded_deflector_ || s.GetNumIntersects() == Intersection::ZeroIntersection)
        {
            // No intersection points with this Deflector, or the Deflector is excluded; skip this Deflector
            continue;
        }
        else if (s.termination == true)
        {
            // The Deflector terminates the propagation of the LightRay
            path_.emplace_back(ray_.GetStart(), ray_.GetPoint(s.GetRayParameter()));
            return false;
        }
        else
        {
            if (intersect == false || s.GetRayParameter() < nearest_s.GetRayParameter())
            {
                nearest_i = i;
                nearest_s = s;
                intersect = true;
            }
        }
    }
    if (intersect)
    {
        excluded_deflector_ = nearest_i;
        path_.emplace_back(ray_.GetStart(), ray_.GetPoint(nearest_s.GetRayParameter()) - ray_.GetStart());
        ray_ = deflectors_[nearest_i]->Emergence(*this, nearest_s);
    }
    else
    {
        return false;
    }
    return true;
}

IncidenceState MirrorDeflector::Incidence(LightRay &light_ray, Ray ray) const
{
    return {GetLineIntersection(ray, mirror_.seg_), false, ray.GetDirection()};
}

Ray MirrorDeflector::Emergence(LightRay &light_ray, IncidenceState s) const
{
    Vec t = mirror_.seg_.GetDirection();
    Vec n = t.Rotate90Anticlockwise();
    Vec x = s.ray_direction.Projection(n);
    Vec y = s.ray_direction.Projection(t);
    return Ray(mirror_.seg_.GetPoint(s.GetDeflectorParameter()), y - x);
}

IncidenceState LensDeflector::Incidence(LightRay &light_ray, Ray ray) const
{
    return {GetLineIntersection(ray, lens_.seg_), false, ray.GetDirection()};
}

Ray LensDeflector::Emergence(LightRay &light_ray, IncidenceState s) const
{
    Vec t = lens_.seg_.GetDirection();
    Vec n = t.Rotate90Anticlockwise();
    Vec x = s.ray_direction.Projection(n);
    Vec y = s.ray_direction.Projection(t);
    double h = (s.GetDeflectorParameter() - 0.5) * t.Norm();
    if (lens_.focal_length_ == 0.0)
        throw ZeroDivisionException();
    Vec c;
    if (x.Dot(n) < 0.0)
        // from left
        c = x.Rotate90Anticlockwise().Scale(h / lens_.focal_length_);
    else
        // from right
        c = x.Rotate90Clockwise().Scale(h / lens_.focal_length_);
    return Ray(lens_.seg_.GetPoint(s.GetDeflectorParameter()), s.ray_direction - c);
}

IncidenceState RefractiveDeflector::Incidence(LightRay &light_ray, Ray ray) const
{
    return {GetLineIntersection(ray, refractive_.seg_), false, ray.GetDirection()};
}

Ray RefractiveDeflector::Emergence(LightRay &light_ray, IncidenceState s) const
{
    Vec t = refractive_.seg_.GetDirection();
    Vec n = t.Rotate90Anticlockwise();
    Vec x = s.ray_direction.Projection(n);
    Vec y = s.ray_direction.Projection(t);
    Vec y_n;
    if (x.Dot(n) < 0.0)
    {
        // from left
        if (refractive_.n_right_ == 0.0)
            throw ZeroDivisionException();
        y_n = y.Scale(refractive_.n_left_ / refractive_.n_right_);
    }
    else
    {
        // from right
        if (refractive_.n_left_ == 0.0)
            throw ZeroDivisionException();
        y_n = y.Scale(refractive_.n_right_ / refractive_.n_left_);
    }
    double c = s.ray_direction.NormSquare() - y_n.NormSquare();
    if (c < 0)
        return Ray(refractive_.seg_.GetPoint(s.GetDeflectorParameter()), y - x);
    Vec x_n = x.Normalize().Scale(std::sqrt(c));
    return Ray(refractive_.seg_.GetPoint(s.GetDeflectorParameter()), x_n + y_n);
}   

IncidenceState WallDeflector::Incidence(LightRay &light_ray, Ray ray) const
{
    return {GetLineIntersection(ray, wall_.seg_), true, ray.GetDirection()};
}

Ray WallDeflector::Emergence(LightRay &light_ray, IncidenceState s) const
{
    return Ray(wall_.seg_.GetPoint(s.GetDeflectorParameter()), s.ray_direction);
}

void Field::Simulation()
{
    for (auto light_ray : light_rays_)
    {
        light_ray->Reset(deflectors_);
        for (size_t step = 0; step < 1000; step++)
        {
            bool is_continue = light_ray->Step();
            if (is_continue == false)
                break;
        }
    }
}
