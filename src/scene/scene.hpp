/**
 * @file scene.hpp
 * @brief Class definitions for scenes.
 *
 */

#ifndef _462_SCENE_SCENE_HPP_
#define _462_SCENE_SCENE_HPP_

#include "math/vector.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "math/camera.hpp"
#include "scene/material.hpp"
#include "scene/mesh.hpp"
#include "scene/bvh.hpp"
#include "ray.hpp"
#include <string>
#include <vector>
#include <cfloat>


namespace _462 {
struct hitRecord
{
	//direction of normal of the object where the ray hits
	Vector3 n;
	//The parameter 't' of the ray when it hits an object
	real_t t;
	//Properties of the matrial of the object that was hit - ambient, diffuse, specular & texColor with refractive index.
	MaterialProp mp;
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

    // Inverse transformation matrix
	Matrix4 invMat;
    // Normal transformation matrix
	Matrix3 normMat;

    /**
     * Renders this geometry using OpenGL in the local coordinate space.
     */
    virtual void render() const = 0;
	virtual bool hit(const Ray& r, real_t t0, real_t t1, hitRecord& h, bool fullRecord) const = 0;
	virtual void InitGeometry();
	virtual void Transform(real_t translate, const Vector3 rotate);
	bool checkBoundingBoxHit(const Ray& r, real_t t0, real_t t1)const;
	BoundingBox bb;
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
    const SphereLight* get_lights() const;
    size_t num_lights() const;
    Material* const* get_materials() const;
    size_t num_materials() const;
    Mesh* const* get_meshes() const;
    size_t num_meshes() const;

    /// Clears the scene, and invokes delete on everything in geometries.
    void reset();

    // functions to add things to the scene
    // all pointers are deleted by the scene upon scene deconstruction.
    void add_geometry( Geometry* g );
    void add_material( Material* m );
    void add_mesh( Mesh* m );
    void add_light( const SphereLight& l );
	static const int maxRecursionDepth;
	Color3 getColor(const Ray& r, std::vector<real_t> refractiveStack,int depth = maxRecursionDepth, real_t t0 = 0, real_t t1 = 1e30) const;

	bool hit(const Ray& r, const real_t t0, const real_t t1, hitRecord& h, bool fullRecord) const;
	Color3 calculateDiffuseColor(Vector3 p, Vector3 n, Color3 kd)const;
	void InitGeometry();
	void buildBVH();
	void SetGlossyReflectionSamples(int val) { num_glossy_reflection_samples = val; }
	void TransformModels(real_t translate, const Vector3 rotate);
	void handleClick(int x, int y, int width, int height,int translation);
private:

    typedef std::vector< SphereLight > SphereLightList;
    typedef std::vector< Material* > MaterialList;
    typedef std::vector< Mesh* > MeshList;
    typedef std::vector< Geometry* > GeometryList;

    // list of all lights in the scene
    SphereLightList point_lights;
    // all materials used by geometries
    MaterialList materials;
    // all meshes used by models
    MeshList meshes;
    // list of all geometries. deleted in dctor, so should be allocated on heap.
    GeometryList geometries;

	bvhNode* tree;

private:
	
	int num_glossy_reflection_samples;
    // no meaningful assignment or copy
    Scene(const Scene&);
    Scene& operator=(const Scene&);

};



} /* _462 */

#endif /* _462_SCENE_SCENE_HPP_ */