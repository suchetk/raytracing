#include "common.h"
#include "camera.h"
#include "material.h"

const int WIDTH = 1000;
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


hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main() {
	// IMAGE DIMENSIONS
    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // OBJECTS
    hittable_list objects = random_scene();
    // auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    // auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    // auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    // auto material_right  = make_shared<dielectric>(1.5);
    // auto material_rightmost = make_shared<lambertian>(color(0.6, 0.3, 1.0));
    // auto material_last = make_shared<metal>(color(0.2, 0.9, 0.8), 0.1);

    // objects.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    // objects.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    // objects.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // objects.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    // objects.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   -0.45, material_right));
    // objects.add(make_shared<sphere>(point3( 2.0,    0.0, -1.0),   0.5, material_rightmost));
    // objects.add(make_shared<sphere>(point3( 1.5,    0.0, -1.0-sqrt(3)/2),   0.5, material_last));

	// CAMERA
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

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
