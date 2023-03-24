#include "common.h"
#include "camera.h"
#include "material.h"
#include <SDL2/SDL.h>

const int WIDTH = 500;
const int SAMPLES = 50;
const int MAX_DEPTH = 10;
const double aspect_ratio = 16.0/9.0;

const color WHITE = color(1, 1, 1);
const color YELLOW = color(1, 1, 0);
const color SKY_BLUE = color(0.5, 0.7, 1.0);
const color BLACK = color(0,0,0);

color ray_color(const ray& r, const hittable& objects, int depth) {
    if (depth <= 0) return BLACK;

    hit_record rec;
    if (objects.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, objects, depth-1);
        }
        return BLACK;
        // point3 target = rec.p + rec.normal + random_unit_vector();
        // return 0.5 * ray_color(ray(rec.p, target-rec.p), objects, depth-1);
    }

    // draw the background
	vec3 unit_direction = unit_vector(r.direction());
	auto y_linear = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - y_linear)*WHITE + y_linear*SKY_BLUE;
}


int main() {
	// IMAGE DIMENSIONS
	const int image_width = WIDTH;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

    // CREATE WINDOW
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize the SDL2 library\n";
        return -1;
    }
    SDL_Window* window = SDL_CreateWindow("Ray Tracer", 0, 0, image_width,image_height, 0);
        if(!window)
    {
        std::cout << "Failed to create window\n";
        return -1;
    }

    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if(!window_surface)
    {
        std::cout << "Failed to get the surface from the window\n";
        return -1;
    }

    SDL_UpdateWindowSurface(window);

    SDL_Delay(5000);

    // OBJECTS
    hittable_list objects;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    auto material_right  = make_shared<dielectric>(1.5);
    auto material_rightmost = make_shared<lambertian>(color(0.6, 0.3, 1.0));
    auto material_last = make_shared<metal>(color(0.2, 0.9, 0.8), 0.1);

    objects.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    objects.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    objects.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    objects.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    objects.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   -0.45, material_right));
    objects.add(make_shared<sphere>(point3( 2.0,    0.0, -1.0),   0.5, material_rightmost));
    objects.add(make_shared<sphere>(point3( 1.5,    0.0, -1.0-sqrt(3)/2),   0.5, material_last));

	// CAMERA
    point3 lookfrom(1,1,2.5);
    point3 lookat(1,0,-1);
    auto dist_to_focus = (lookfrom-lookat).length();
    camera cam(lookfrom, lookat, vec3(0,1,0), 50, aspect_ratio, 0.1, dist_to_focus);

	// RENDERING
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
	for (int j = image_height-1; j >= 0; --j) {
        std::cerr << image_height - j << " of " << image_height <<"\n";
		for (int i = 0; i < image_width; ++i) {
            color pixel(0,0,0);
            for (int s = 0; s < SAMPLES; ++s) {
                auto u = (i + random_double())/(image_width-1);
                auto v = (j + random_double())/(image_height-1);
                ray r = cam.get_ray(u,v);
                pixel += ray_color(r, objects, MAX_DEPTH);
            }
            write_color(std::cout, pixel, SAMPLES);
		}
	}
}
