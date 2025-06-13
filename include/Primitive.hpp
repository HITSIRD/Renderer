//
// Created by 闻永言 on 2021/12/7.
//

#ifndef RENDERER_PRIMITIVE_HPP
#define RENDERER_PRIMITIVE_HPP

#include "BoundingBox.hpp"
#include "HitRecord.hpp"
#include "Vertex.hpp"

class Primitive {
protected:
    Primitive() {
    }

public:
    BoundingBox box;
    float3 center; // primitive center coordinate in world space

    virtual ~Primitive() = default;

    /**
     *
     * @param ray
     * @param tMin
     * @param tMax
     * @param record
     * @return if hit a primitive object
     */
    virtual bool hit(Ray &ray, float tMin, float tMax, HitRecord &record) {
        return true;
    }

    /**
     * Destroy the node.
     * @param node
     */
    virtual void destroy() {
        delete this;
    }
};

#endif //RENDERER_PRIMITIVE_HPP
