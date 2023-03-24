//
// Created by Suchet Kumar on 3/18/23.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

class camera {
private:
    point3 origin;
    point3 lower_left;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    double lens_radius;
public:
    camera(
        point3 look_from,
        point3 look_at,
        vec3 vup,
        double vfov, 
        double aspect_ratio,
        double aperture, 
        double focus_dist
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto view_height = 2.0 * h;
        auto view_width = aspect_ratio * view_height;

        w = unit_vector(look_from - look_at);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        origin = look_from;
        horizontal = focus_dist * view_width * u;
        vertical = focus_dist * view_height * v;
        lower_left = origin - horizontal/2 - vertical/2 - focus_dist * w;

        lens_radius = aperture / 2;
    }

    // origin is the camera position, each ray calculates a pixel of the view pane
    ray get_ray(double s, double t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin+offset, lower_left + s*horizontal + t*vertical - origin - offset);
    }
};

#endif
