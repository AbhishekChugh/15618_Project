/**
 * @file raytacer.hpp
 * @brief Raytracer class
 *
 * Implement these functions for project 3.
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#ifndef _462_RAYTRACER_HPP_
#define _462_RAYTRACER_HPP_

#define MAX_DEPTH 5

#include "math/color.hpp"
#include "math/random462.hpp"

namespace _462 {

class Scene;
class Ray;
struct Intersection;
struct Options;
struct Packet;

class Raytracer
{
public:

    Raytracer();

    ~Raytracer();

    bool initialize(Scene* scene,int num_samples, int num_glossy_reflection_samples, 
		size_t width, size_t height);

    bool raytrace(unsigned char* buffer, real_t* max_time, bool packet_tracing);

private:

    Color3 trace_pixel(const Scene* scene,
		       size_t x,
		       size_t y,
		       size_t width,
		       size_t height);

    Packet build_packet(int x, int y, size_t width, size_t height);

    // when multiple packets inside one pixel
    void trace_small_packet(unsigned char* buffer,
			    size_t width,
			    size_t height);

    // when a packet is over multiple pixels
    void trace_large_packet(unsigned char* buffer,
			    size_t width,
			    size_t height);

    // the scene to trace
    Scene* scene;

    // the dimensions of the image to trace
    size_t width, height;

    // the next row to raytrace
    size_t current_row;
	
    unsigned int num_samples;
    unsigned int num_dof_samples;

    // work size
    size_t pixel_width;
    // packet size (ray)
    size_t packet_width_ray;
    // packet size (pixel)
    size_t packet_width_pixel;
};

} /* _462 */

#endif /* _462_RAYTRACER_HPP_ */
