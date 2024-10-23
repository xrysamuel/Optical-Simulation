#ifndef OPTICS_H
#define OPTICS_H

#include "geometry.h"
#include <vector>
#include <memory>

// State of the LightRay incident on the Deflector
struct IncidenceState
{
    Intersection intersection; // Info about the intersection point between the Deflector and the LightRay
    bool termination = false;  // Whether the Deflector terminates the propagation of the LightRay
    Vec ray_direction;         // The direction of the incident light ray
    Intersection::NumIntersects GetNumIntersects() { return intersection.num_intersects; }
    double GetRayParameter() { return intersection.parameter1; }
    double GetDeflectorParameter() { return intersection.parameter2; }
};

class LightRay;

// Deflector interface, an abstraction for actual optical elements
class Deflector
{
public:
    Deflector() {}
    // Incident calculation, with the parameter light_ray as a reserved parameter; ray represents the ray corresponding to the incident light, returning the incidence state
    virtual IncidenceState Incidence(LightRay &light_ray, Ray ray) const = 0;
    // Emission calculation, with the parameter light_ray as a reserved parameter; s represents the incident state and returns the outgoing ray
    virtual Ray Emergence(LightRay &light_ray, IncidenceState s) const = 0;
};

// LightRay class, an abstraction for light path
class LightRay
{
protected:
    std::vector<Segment> path_;                          // The historical path of the LightRay
    Ray ray_;                                            // The ray at the end of the LightRay path
    Ray init_ray_;
    std::vector<std::shared_ptr<Deflector>> deflectors_; // All Deflectors involved in the light path calculation
    size_t excluded_deflector_;                          // Exclude the most recently encountered Deflector

public:
    LightRay(Ray ray) : ray_(ray), init_ray_(ray), excluded_deflector_(-1) {}
    // Perform a propagation calculation, which will determine which Deflector's emission calculation to invoke, returning whether the LightRay can continue to propagate
    bool Step();
    void Reset(const std::vector<std::shared_ptr<Deflector>> &deflectors)
    {
        deflectors_ = deflectors;
        excluded_deflector_ = -1;
        ray_ = init_ray_;
        path_.clear();
    }
};

struct Mirror
{
    Segment seg_;
};

struct Lens
{
    Segment seg_;
    double focal_length_;
};

struct RefractiveSurface
{
    Segment seg_;
    double n_left_;
    double n_right_;
};

struct Wall
{
    Segment seg_;
};

class MirrorDeflector : public Deflector
{
protected:
    Mirror mirror_;

public:
    MirrorDeflector(const Mirror &mirror) : mirror_(mirror) {}
    virtual IncidenceState Incidence(LightRay &light_ray, Ray ray) const override;
    virtual Ray Emergence(LightRay &light_ray, IncidenceState s) const override;
};

class LensDeflector : public Deflector
{
protected:
    Lens lens_;

public:
    LensDeflector(const Lens &lens) : lens_(lens) {}
    virtual IncidenceState Incidence(LightRay &light_ray, Ray ray) const override;
    virtual Ray Emergence(LightRay &light_ray, IncidenceState s) const override;
};

class RefractiveDeflector : public Deflector
{
protected:
    RefractiveSurface refractive_;

public:
    RefractiveDeflector(const RefractiveSurface &refractive) : refractive_(refractive) {}
    virtual IncidenceState Incidence(LightRay &light_ray, Ray ray) const override;
    virtual Ray Emergence(LightRay &light_ray, IncidenceState s) const override;
};

class WallDeflector : public Deflector
{
protected:
    Wall wall_;

public:
    WallDeflector(const Wall &wall) : wall_(wall) {}
    virtual IncidenceState Incidence(LightRay &light_ray, Ray ray) const override;
    virtual Ray Emergence(LightRay &light_ray, IncidenceState s) const override;
};

class Field
{
private:
    std::vector<std::shared_ptr<LightRay>> light_rays_;
    std::vector<std::shared_ptr<Deflector>> deflectors_;

public:
    void AddDeflector(std::shared_ptr<Deflector> deflector)
    {
        deflectors_.push_back(deflector);
    }
    void AddLightRay(std::shared_ptr<LightRay> light_ray)
    {
        light_rays_.push_back(light_ray);
    }
    void Simulation();
    void Clear()
    {
        light_rays_.clear();
        deflectors_.clear();
    }
};

#endif