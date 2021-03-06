/**
 * @file model.hpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_MODEL_HPP_
#define _462_SCENE_MODEL_HPP_

#include "scene/scene.hpp"
#include "scene/mesh.hpp"
#include "scene/triangle.hpp"

namespace _462 {

/**
 * A mesh of triangles.
 */
class Model : public Geometry
{
public:

    const Mesh* mesh;
    const Material* material;

    Model();
    virtual ~Model();

    virtual void render() const;
    virtual bool hit(const Ray& r, real_t t0, real_t t1,hitRecord& h, bool fullRecord) const;
    virtual void hitPacket(const Packet& packet, int start, int end, real_t t0, real_t *t1, std::vector<hitRecord>& hs, bool fullRecord) const;
    virtual void InitGeometry();

	virtual float get_area();
	virtual Vector3 sample(const Vector3 &p, float r1, float r2,  float c, Vector3 *n_ptr);
	virtual float pdf(const Vector3 &p, const Vector3 &wi);

    BVHAccel* bvh;
    std::vector<Triangle> triangles;
private:
	float sum_area;
};


} /* _462 */

#endif /* _462_SCENE_MODEL_HPP_ */

