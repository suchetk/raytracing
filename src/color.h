//
// Created by Suchet Kumar on 3/18/23.
//

#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>

void write_color(std::ostream &out, color pixel_color, int samples) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    auto scale = 1.0/samples;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
} 

void write_color(uint8_t* render_pixels, double* pixel_avg, color& pixel_color, int& start, int& sample) {
    pixel_avg[start] += (pixel_color.x() - pixel_avg[start])/sample;
    pixel_avg[start+1] += (pixel_color.y() - pixel_avg[start+1])/sample;
    pixel_avg[start+2] += (pixel_color.z() - pixel_avg[start+2])/sample;

    render_pixels[start] = static_cast<uint8_t>(256 * clamp(sqrt(pixel_avg[start]), 0.0, 0.999));
    render_pixels[start+1] = static_cast<uint8_t>(256 * clamp(sqrt(pixel_avg[start+1]), 0.0, 0.999));
    render_pixels[start+2] = static_cast<uint8_t>(256 * clamp(sqrt(pixel_avg[start+2]), 0.0, 0.999));
}

#endif
