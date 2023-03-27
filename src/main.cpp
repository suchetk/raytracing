#include "common.h"
#include "camera.h"
#include "material.h"
#include "window.h"
#include "threadpool.h"
#include <chrono>
#include <unistd.h>

const int SAMPLES = 100;
const int MAX_DEPTH = 15;
const double aspect_ratio = 3.0/2.0;
const int WIDTH = 1000;
const int HEIGHT = static_cast<int>(WIDTH / aspect_ratio);

const color WHITE = color(1, 1, 1);
const color YELLOW = color(1, 1, 0);
const color SKY_BLUE = color(0.5, 0.7, 1.0);
const color BLACK = color(0,0,0);
const color BLUE = color(0,0,0.5);
const color DARK_BLUE = color(0, 0, 0.4);

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
        } else if (rec.mat_ptr->emanate(attenuation)) {
            return attenuation;
        }
    }
    // draw the background
    // return BLACK;
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
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sample " << sample << ": " << elapsed << "ms" << std::endl;
}

class threadJob {

public:
    int i;
    int j;
    hittable_list objects;
    camera cam;
    int sample;
    threadJob(int& x, int& y, hittable_list& objs, camera& camera, int& sam) : i(x), j(y), objects(objs), cam(camera), sample(sam) {}
    
    void operator()() {
        auto u = (i + random_double())/(WIDTH-1);
        auto v = (j + random_double())/(HEIGHT-1);
        ray r = cam.get_ray(u,v);
        color pixel = ray_color(r, objects, MAX_DEPTH);
        int start_position = (i + j*WIDTH)*3;
        write_color(render_pixels, pixel_avg, pixel, start_position, sample);
        // std::cout << "job done\n";
    }
};


void thread_Job(int& i, int& j, hittable_list& objects, camera& cam, int& sample) {
    auto u = (i + random_double())/(WIDTH-1);
    auto v = (j + random_double())/(HEIGHT-1);
    ray r = cam.get_ray(u,v);
    color pixel = ray_color(r, objects, MAX_DEPTH);
    int start_position = (i + j*WIDTH)*3;
    write_color(render_pixels, pixel_avg, pixel, start_position, sample);
}

void concurrent_render(threadPool& pool, hittable_list& objects, camera& cam, int& sample) {
    auto start = std::chrono::steady_clock::now();
    for (int j = HEIGHT-1; j >= 0; --j) {
    	for (int i = 0; i < WIDTH; ++i) {
            threadJob job(i, j, objects, cam, sample);
            pool.queueJob(job);
    	}
	}
    while (pool.busy()) usleep(1000);
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sample " << sample << ": " << elapsed << "ms" << std::endl;
}

vec3 parse_key(SDL_Keycode sym) {
    vec3 vec;
    switch (sym) {
        case SDLK_RIGHT:
            std::cout << "right" << std::endl;
            vec = vec3(1,0,0);
            break;
        case SDLK_LEFT:
            std::cout << "left" << std::endl;
            vec = vec3(-1,0,0);
            break;
        case SDLK_UP:
            std::cout << "up" << std::endl;
            vec = vec3(0,1,0);
            break;
        case SDLK_DOWN:
            std::cout << "down" << std::endl;
            vec = vec3(0,-1,0);
            break;
        case SDLK_e:
            std::cout << "forward" << std::endl;
            vec = vec3(0,0,-1);
            break;
        case SDLK_d:
            std::cout << "back" << std::endl;
            vec = vec3(0,0,1);
            break;
    }
    return vec;
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -5; a < 5; a++) {
        for (int b = -5; b < 5; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.6) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.85) {
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
    // threadPool pool;
    // pool.start(4);

    // CREATE WINDOW
    window win(WIDTH, HEIGHT);
    memset(render_pixels, 0, pix_arr_size);
    memset(pixel_avg, 0, pix_arr_size);

    // // OBJECTS
    // hittable_list objects;
    // auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.1));
    // auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    // auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8), 0.3);
    // auto material_right  = make_shared<dielectric>(1.5);
    // auto material_rightmost = make_shared<lambertian>(color(0.6, 0.3, 1.0));
    // auto material_last = make_shared<metal>(color(0.2, 0.9, 0.8), 0.1);
    // auto material_light = make_shared<light>(color(1,1,1));

    // objects.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    // // objects.add(make_shared<sphere>(point3( 5.0, 5.0, 5.0), 5.0, material_light));
    // objects.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    // objects.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    // objects.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));
    // objects.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   -0.45, material_right));
    // objects.add(make_shared<sphere>(point3( 2.0,    0.0, -1.0),   0.5, material_rightmost));
    // objects.add(make_shared<sphere>(point3( 1.5,    0.0, -1.0-sqrt(3)/2),   0.5, material_last));

    hittable_list objects = random_scene();


	// CAMERA
    // point3 lookfrom(1,1,2.5);
    point3 lookfrom(13,2,3);
    // point3 lookat(1,0,-1);
    point3 lookat(0, 1, 0);
    double aperture = 0.1;
    camera cam(lookfrom, lookat, vec3(0,1,0), 20, aspect_ratio, aperture);

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
            if( e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE)
            {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                vec3 vec = parse_key(e.key.keysym.sym);
                if (vec.length_squared() > 0) {
                    cam.move(vec);
                    memset(render_pixels, 0, pix_arr_size);
                    memset(pixel_avg, 0, pix_arr_size);
                    sample = 1;
                }
            }
        }
        
    }

    // clean up
    win.shutdown();
    // pool.stop();
}
