#include "common.h"
#include "camera.h"
#include "material.h"
#include "window.h"
#include <chrono>

// const int SAMPLES = 1;
const int MAX_DEPTH = 10;
const double aspect_ratio = 16.0/9.0;
const int WIDTH = 1500;
const int HEIGHT = static_cast<int>(WIDTH / aspect_ratio);

const color WHITE = color(1, 1, 1);
const color YELLOW = color(1, 1, 0);
const color SKY_BLUE = color(0.5, 0.7, 1.0);
const color BLACK = color(0,0,0);

// array of pixels
const int pix_arr_size = WIDTH * HEIGHT * 3;
uint8_t render_pixels[pix_arr_size];
double pixel_avg[pix_arr_size];

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
    }
    // draw the background
	vec3 unit_direction = unit_vector(r.direction());
	auto y_linear = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - y_linear)*WHITE + y_linear*SKY_BLUE;
}

void render(hittable_list& objects, camera& cam, int& sample) {
    auto start = std::chrono::steady_clock::now();
    for (int j = HEIGHT-1; j >= 0; --j) {
    	for (int i = 0; i < WIDTH; ++i) {
            auto u = (i + random_double())/(WIDTH-1);
            auto v = (j + random_double())/(HEIGHT-1);
            ray r = cam.get_ray(u,v);
            color pixel = ray_color(r, objects, MAX_DEPTH);
            int start_position = (i + j*WIDTH)*3;
            write_color(render_pixels, pixel_avg, pixel, start_position, sample);
    	}
	}
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time to render sample: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "[ms]" << std::endl;
}


int main() {
    // CREATE WINDOW
    window win(WIDTH, HEIGHT);
    memset(render_pixels, 0, pix_arr_size);
    memset(pixel_avg, 0, pix_arr_size);

    // OBJECTS
    hittable_list objects;
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.1));
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
    // point3 lookfrom(1,1,2.5);
    point3 lookfrom(-2,2,1);
    // point3 lookat(1,0,-1);
    point3 lookat(0,0,-1);
    auto dist_to_focus = (lookfrom-lookat).length();
    double aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0,1,0), 50, aspect_ratio, aperture, dist_to_focus);

    int sample = 1;

    //Event handler
    SDL_Event e;

    bool quit = false;
    while( !quit )
    {
        render(objects, cam, sample);
        win.update(render_pixels);
        sample++;

        //Handle events on queue
        while ( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_RIGHT:
                        std::cout << "right" << std::endl;
                        break;
                    case SDLK_LEFT:
                        std::cout << "left" << std::endl;
                        break;
                    case SDLK_UP:
                        std::cout << "up" << std::endl;
                        break;
                    case SDLK_DOWN:
                        std::cout << "down" << std::endl;
                        break;
                }
            }
        }
        
    }
    win.shutdown();
}
