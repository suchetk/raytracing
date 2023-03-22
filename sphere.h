//
// Created by Suchet Kumar on 3/18/23.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
public:
    point3 center;
    double rad;
    shared_ptr<material> mat;

    sphere() = default;
    sphere(point3 c, double r, shared_ptr<material> m) : center(c), rad(r), mat(m) {}
    bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
        // Surface of the sphere is (P-C) * (P-C) - r*r = 0
        // P = O + D*t where O is the ray origin and D is the direction
        // Substitute and get t*t*(D*D) + 2*t*(D*(O-C)) + (O-C)*(O-C) - r*r = 0
        // Solving for t, if there is a root, there is an intersection. Can just check discriminant at this point.
        vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - rad*rad;

        auto discriminant = half_b*half_b - a*c;
        if (discriminant < 0) return false;

        // if hit, populate the hit record
        auto root = (-half_b - sqrt(discriminant))/a;

        // handle t_min, t_max
        if (root < t_min || root > t_max) {
            root =  (-half_b + sqrt(discriminant))/a;
            if (root < t_min || root > t_max) return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p-center)/rad;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat;
        return true;
    }
};

#endif
