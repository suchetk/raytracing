//
// Created by Suchet Kumar on 3/19/23.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"

class material {
public:
    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
    virtual bool emanate(color& attenuation) const = 0;
};

class lambertian : public material {
public:
    color albedo;

    lambertian(const color& a): albedo(a) {}

    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto dir = rec.normal + random_unit_vector()*random_scatter_scalar;
        if (dir.near_zero()) dir = rec.normal;
        scattered = ray(rec.p, dir);
        attenuation = albedo;
        return true;
    }

    virtual bool emanate(color& attenuation) const override { return false; }
};

class metal : public material {
public:
    color albedo;
    double fuzz;

    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
        auto reflection = reflect(r.direction(), rec.normal);
        scattered = ray(rec.p, reflection + fuzz*random_in_unit_sphere()*random_scatter_scalar);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

    virtual bool emanate(color& attenuation) const override { return false; }
};

class dielectric : public material {
private:
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1 - cosine),5);
    }
public:
    double ir;

    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

        vec3 unit_direction = unit_vector(r.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;
        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = ray(rec.p, direction);
        return true;
    }

    virtual bool emanate(color& attenuation) const override { return false; }
};

class light : public material {
public:
    color albedo;
    light(const color& a) : albedo(a) {}
    virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
        return false;
    }

    virtual bool emanate(color& attenuation) const override { 
        attenuation = albedo;
        return true; 
    }
};

#endif