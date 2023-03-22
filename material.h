//
// Created by Suchet Kumar on 3/19/23.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"

class material {
public:
    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian : public material {
public:
    color albedo;

    lambertian(const color& a): albedo(a) {}

    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto dir = rec.normal + random_unit_vector();
        if (dir.near_zero()) dir = rec.normal;
        scattered = ray(rec.p, dir);
        attenuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    color albedo;
    double fuzz;

    metal(const color& a, double f) : albedo(a), fuzz(f) {}

    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto reflection = reflect(r.direction(), rec.normal);
        scattered = ray(rec.p, reflection + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

#endif