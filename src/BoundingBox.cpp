//
// Created by 闻永言 on 2021/12/7.
//

#include "BoundingBox.hpp"

using namespace std;

BoundingBox::BoundingBox(): minPoint(0, 0, 0), maxPoint(0, 0, 0) {
}

BoundingBox::BoundingBox(float *bound) {
    minPoint = float3(bound[0], bound[2], bound[4]);
    maxPoint = float3(bound[1], bound[3], bound[5]);
}

BoundingBox::~BoundingBox() = default;

void BoundingBox::setBound(float *bound) {
    minPoint = float3(bound[0], bound[2], bound[4]);
    maxPoint = float3(bound[1], bound[3], bound[5]);
}

void BoundingBox::setBound(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
    minPoint = float3(xMin, yMin, zMin);
    maxPoint = float3(xMax, yMax, zMax);
}

bool BoundingBox::hit(const Ray &ray, float tMin, float tMax) {
    for (int axis = 0; axis < 3; axis++) {
        float d_inv = 1.0f / ray.direction[axis];
        float t_0 = (minPoint[axis] - ray.origin[axis]) * d_inv;
        float t_1 = (maxPoint[axis] - ray.origin[axis]) * d_inv;
        float tmpMin = min(t_0, t_1);
        float tmpMax = max(t_0, t_1);
        tMin = max(tmpMin, tMin);
        tMax = min(tmpMax, tMax);

        if (tMax < tMin) {
            return false;
        }
    }
    return true;
}

BoundingBox BoundingBox::combine(const BoundingBox &box0, const BoundingBox &box1) {
    float xMin = min(box0.minPoint.x(), box1.minPoint.x());
    float xMax = max(box0.maxPoint.x(), box1.maxPoint.x());
    float yMin = min(box0.minPoint.y(), box1.minPoint.y());
    float yMax = max(box0.maxPoint.y(), box1.maxPoint.y());
    float zMin = min(box0.minPoint.z(), box1.minPoint.z());
    float zMax = max(box0.maxPoint.z(), box1.maxPoint.z());
    float bound[6] = {xMin, xMax, yMin, yMax, zMin, zMax};
    return BoundingBox(bound);
}
