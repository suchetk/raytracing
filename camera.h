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
public:
    camera(
        point3 look_from,
        point3 look_at,
        vec3 vup,
        double vfov, 
        double aspect_ratio
        ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto view_height = 2.0 * h;
        auto view_width = aspect_ratio * view_height;

        auto w = unit_vector(look_from - look_at);
        auto u = unit_vector(cross(vup, w));
        auto v = cross(w, u);

        origin = look_from;
        horizontal = view_width * u;
        vertical = view_height * v;
        lower_left = origin - horizontal/2 - vertical/2 - w;
    }

    // origin is the camera position, each ray calculates a pixel of the view pane
    ray get_ray(double s, double t) const {
        return ray(origin, lower_left + s*horizontal + t*vertical - origin);
    }
};

#endif
