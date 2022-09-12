//
// Created by 闻永言 on 2021/12/7.
//

#ifndef RENDERER_BOUNDINGBOX_HPP
#define RENDERER_BOUNDINGBOX_HPP

#include "Type.hpp"
#include "Ray.hpp"

/**
 * AABB
 */
class BoundingBox
{
public:
    float3 minPoint;
    float3 maxPoint;

    BoundingBox();

    /**
     *
     * @param bound
     */
    explicit BoundingBox(float bound[6]);

    /**
     *
     * @param bound
     */
    void setBound(float bound[6]);

    ~BoundingBox();

    /**
     *
     * @param xMin
     * @param xMax
     * @param yMin
     * @param yMax
     * @param zMin
     * @param zMax
     */
    void setBound(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);

    /**
     *
     * @param ray
     * @param tMin minimum hit time
     * @param tMax maximum hit time
     * @return
     */
    bool hit(const Ray &ray, float tMin, float tMax);

    /**
     *
     * @param box0
     * @param box1
     * @return
     */
    static BoundingBox combine(const BoundingBox &box0, const BoundingBox &box1);
};

#endif //RENDERER_BOUNDINGBOX_HPP
