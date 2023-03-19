#include "common.h"
#include "camera.h"

const int WIDTH = 1080;
const int SAMPLES = 100;
const int MAX_DEPTH = 50;

const color WHITE = color(1, 1, 1);
const color YELLOW = color(1, 1, 0);
const color SKY_BLUE = color(0.5, 0.7, 1.0);
const color BLACK = color(0,0,0);

color ray_color(const ray& r, const hittable& objects, int depth) {
    if (depth <= 0) return BLACK;

    hit_record rec;
    if (objects.hit(r, 0.001, infinity, rec)) {
        point3 target = rec.p + rec.normal + random_unit_vector();
        return 0.5 * ray_color(ray(rec.p, target-rec.p), objects, depth-1);
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

    // OBJECTS
    hittable_list objects;
    objects.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    objects.add(make_shared<sphere>(point3(1, -500.5, -1), 500));

	// CAMERA
    camera cam;

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
