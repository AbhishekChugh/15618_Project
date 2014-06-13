/**
* @file scene.hpp
* @brief Class definitions for scenes.
*
*/

#ifndef _462_SCENE_SCENE_HPP_
#define _462_SCENE_SCENE_HPP_

#include "ray.hpp"
#include "math/vector.hpp"
#include "math/color.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "math/camera.hpp"
#include "scene/mesh.hpp"
#include "scene/bvh.hpp"
#include <string>
#include <vector>
#include <cfloat>
#include <malloc.h>
#include "xmmintrin.h"
#include "emmintrin.h"

namespace _462 {

#define LANES 8

class AreaLight;
class BSDF;
class Material;
class Light;

struct MaterialProp
{ 
	// ambient color (ignored if refractive_index != 0)
    Color3 ambient;

    // diffuse color
    Color3 diffuse;

    // specular (reflective) color
    Color3 specular;
	
	Color3 texColor;

    real_t refractive_index;
};

    struct hitRecord
    {
        //direction of normal of the object where the ray hits
        Vector3 n;
        //The parameter 't' of the ray when it hits an object
        real_t t;
        //Properties of the matrial of the object that was hit - ambient, diffuse, specular & texColor with refractive index.
        MaterialProp mp;

		BSDF *bsdf_ptr;
		Matrix3 shading_trans;
		Matrix3 inv_shading_trans;
		Vector3 p;
		Geometry *shape_ptr;
		uint32_t depth;

        hitRecord() {
            t = BIG_NUMBER;
			shape_ptr = NULL;
			depth = 0;
			bsdf_ptr = NULL;
        }
    };

    enum PlanePosition { FRONT, BACK, TOP, BOTTOM, LEFT, RIGHT };

    struct Plane {
        Vector3 norm;
        Vector3 point;
    };

    struct Frustum {
        Plane planes[6];
        bool isValid;
        Frustum():isValid(false) {
            planes[0].norm = Vector3::Zero();
        }
    };

    struct Packet {
        Frustum frustum;
        Ray *rays;

        float *e_x;
        float *e_y;
        float *e_z;
        float *d_x;
        float *d_y;
        float *d_z;

        uint32_t size;

        Packet(size_t packet_size);

        ~Packet();

    private:
        Packet(const Packet& packet);
        Packet& operator= (const Packet& packet);
    };

    class Geometry
    {
    public:
        Geometry();
        virtual ~Geometry();

        /*
        World transformation are applied in the following order:
        1. Scale
        2. Orientation
        3. Position
        */

        // The world position of the object.
        Vector3 position;

        // The world orientation of the object.
        // Use Quaternion::to_matrix to get the rotation matrix.
        Quaternion orientation;

        // The world scale of the object.
        Vector3 scale;

		Matrix4 transMat;
        // Inverse transformation matrix
        Matrix4 invMat;
        // Normal transformation matrix
        Matrix3 normMat;

        /**
        * Renders this geometry using OpenGL in the local coordinate space.
        */
        virtual void render() const = 0;
        virtual bool hit(const Ray& r, real_t t0, real_t t1, hitRecord& h, bool fullRecord) const = 0;
        virtual void hitPacket(const Packet& packet, int start, int end, real_t t0, real_t *t1, std::vector<hitRecord>& hs, bool fullRecord) const = 0;
        virtual void InitGeometry();
        virtual void Transform(real_t translate, const Vector3 rotate);
		virtual float get_area() = 0;
		virtual Vector3 sample(const Vector3 &p, float r1, float r2, float c, Vector3 *n_ptr) = 0;
		virtual float pdf(const Vector3 &p, const Vector3 &wi) = 0;
        bool checkBoundingBoxHit(const Ray& r, real_t t0, real_t t1)const;

        BoundingBox bb;
		AreaLight *light_ptr;
    };

    struct SphereLight
    {
        struct Attenuation
        {
            real_t constant;
            real_t linear;
            real_t quadratic;
        };

        SphereLight();

        // The position of the light, relative to world origin.
        Vector3 position;
        // The color of the light (both diffuse and specular)
        Color3 color;
        // attenuation
        Attenuation attenuation;
        real_t radius;
    };

    /**
    * The container class for information used to render a scene composed of
    * Geometries.
    */
    class Scene
    {
    public:

        /// the camera
        Camera camera;
        /// the background color
        Color3 background_color;
        /// the amibient light of the scene
        Color3 ambient_light;
        /// the refraction index of air
        real_t refractive_index;

        /// Creates a new empty scene.
        Scene();

        /// Destroys this scene. Invokes delete on everything in geometries.
        ~Scene();

        // accessor functions
        Geometry* const* get_geometries() const;
        size_t num_geometries() const;
        Light* const* get_lights() const;
        size_t num_lights() const;
        Material* const* get_materials() const;
        size_t num_materials() const;
        Mesh* const* get_meshes() const;
        size_t num_meshes() const;

		const SphereLight* get_simple_lights() const;
        size_t num_simple_lights() const;
		void add_simple_light( const SphereLight& l );

        /// Clears the scene, and invokes delete on everything in geometries.
        void reset();

        // functions to add things to the scene
        // all pointers are deleted by the scene upon scene deconstruction.
        void add_geometry( Geometry* g );
        void add_material( Material* m );
        void add_mesh( Mesh* m );
        //void add_light( const SphereLight& l );
		void add_light( Light* l );
        static const int maxRecursionDepth;
        Color3 getColor(const Ray& r, std::vector<real_t> refractiveStack, int depth = maxRecursionDepth, real_t t0 = 0, real_t t1 = 1e30) const;
        void getColors(const Packet& packet, std::vector<std::vector<real_t> >& refractiveStack, Color3* col, int depth = maxRecursionDepth, real_t t0 = 0, real_t t1 = 1e30) const;

        bool hit(const Ray& r, const real_t t0, const real_t t1, hitRecord& h, bool fullRecord) const;
		void hit(const Packet& packet, const real_t t0, const real_t t1, std::vector<hitRecord>& records, bool fullRecord) const;

        Color3 calculateDiffuseColor(Vector3 p, Vector3 n, Color3 kd)const;
        void calculateDiffuseColors(std::vector<Vector3>& p, std::vector<hitRecord>& h, Color3* col) const;

        void InitGeometry();
        void buildBVH();
        void SetGlossyReflectionSamples(int val) { num_glossy_reflection_samples = val; }
        void TransformModels(real_t translate, const Vector3 rotate);
        void handleClick(int x, int y, int width, int height,int translation);

		void bounding_sphere(Vector3 *center_ptr, float *r_ptr) const;

    private:

        typedef std::vector< SphereLight > SphereLightList;
		typedef std::vector< Light* > LightList;
        typedef std::vector< Material* > MaterialList;
        typedef std::vector< Mesh* > MeshList;
        typedef std::vector< Geometry* > GeometryList;

		SphereLightList simple_lights;

        // list of all lights in the scene
        LightList lights;
        // all materials used by geometries
        MaterialList materials;
        // all meshes used by models
        MeshList meshes;
        // list of all geometries. deleted in dctor, so should be allocated on heap.
        GeometryList geometries;

        BVHAccel* tree;

		BoundingBox world_bounding;

    private:

        int num_glossy_reflection_samples;
        // no meaningful assignment or copy
        Scene(const Scene&);
        Scene& operator=(const Scene&);

    };



} /* _462 */

#endif /* _462_SCENE_SCENE_HPP_ */
