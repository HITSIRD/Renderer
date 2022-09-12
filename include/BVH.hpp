//
// Created by 闻永言 on 2021/12/2.
//

#ifndef RENDERER_BVH_HPP
#define RENDERER_BVH_HPP

#include "Type.hpp"
#include "Primitive.hpp"
#include "BoundingBox.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"

class BVHNode: public Primitive
{
public:
    Primitive *left;
    Primitive *right;

    BVHNode();

    /**
     *
     * @param primitives
     * @param axis
     */
    BVHNode(std::vector<Primitive *> primitives, int axis);

    ~BVHNode() override;

    /**
     *
     * @param ray
     * @param tMin
     * @param tMax
     * @param record
     * @return
     */
    bool hit(Ray &ray, float tMin, float tMax, HitRecord &record) override;

    /**
     * Destroy the node.
     */
    void destroy() override;
};

#endif //RENDERER_BVH_HPP
