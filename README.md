# Ray Tracing from Scratch

![](./png/img11.png)
*(~100 samples/pixel)*

Building raytracing engine from scratch, following https://raytracing.github.io/books/RayTracingInOneWeekend.html as a starting point.

This project extends the starter code by implementing real-time rendering and displaying with [SDL](https://www.libsdl.org/) library. Also allows user to control camera position with arrow keys to move up/left/down/right and `E` & `D` keys to control depth.

The shading algorithm loosely follows the [Phong reflection model](https://en.wikipedia.org/wiki/Phong_reflection_model), by multiplying `Color` vectors together when light rays hit surfaces, according to the material. The `Color` vectors are then normalized with gamma correction.


Ran experiments with custom-built ThreadPool to speed up rendering. However, in practice it actually slowed down rendering. This could be due to bad design of the thread jobs.


## Progress Log

1. Implemented Spherical objects and light bouncing. The light bounces off these objects with Lambertian diffusion. A random vector in the unit sphere around the normal vector is chosen for bouncing off objects.

![](./png/img.png)

![](./png/img2.png)

2. Added coloring and different materials. The left and right spheres are reflective metals.

![](./png/img3.png)

3. Added fuzz texture to metal materials. The more fuzz, the more blurry the reflection.

![](./png/img4.png)

4. Added glass material. Light refracts through this sphere.

![](./png/img5.png)

5. Implemented real-time sample-by-sample rendering with ability to move the camera with keyboard input

![](./png/img6.png)
![](./png/img7.png)
![](./png/img8.png)

6. Scale down random vector in light scattering - this unintentionally had the effect of intensifying shadows because it is harder for light to escape the cracks.

![](./png/img9.png)

7. Render final scene. Moved the camera and captured two perspectives of the same scene. 

![](./png/img10.png)
![](./png/img11.png)
