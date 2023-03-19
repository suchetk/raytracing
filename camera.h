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
    camera() {
        auto view_height = 2.0;
        auto view_width = aspect_ratio * view_height;
        auto focal_length = 1.0;

        origin = point3(0, 0, 0);
        horizontal = vec3(view_width, 0, 0);
        vertical = vec3(0, view_height, 0);
        auto depth = vec3(0, 0, focal_length);
        lower_left = origin - horizontal/2 - vertical/2 - depth;
    }

    // origin is the camera position, each ray calculates a pixel of the view pane
    ray get_ray(double u, double v) const {
        return ray(origin, lower_left + u*horizontal + v*vertical - origin);
    }
};

#endif
