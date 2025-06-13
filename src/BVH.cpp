//
// Created by 闻永言 on 2021/12/2.
//

#include "BVH.hpp"
#include <vector>

using namespace std;

BVHNode::BVHNode(): left(nullptr), right(nullptr) {
}

BVHNode::BVHNode(std::vector<Primitive *> primitives, int axis) {
    if (primitives.empty()) {
        left = nullptr;
        right = nullptr;
        return;
    }

    axis = axis % 3;
    int size = primitives.size();
    if (size == 1) {
        left = primitives[0];
        right = nullptr;
        box = primitives[0]->box;
        center = 0.5f * (box.maxPoint + box.minPoint);
        return;
    } else if (size == 2) {
        left = primitives[0];
        right = primitives[1];
        box = BoundingBox::combine(primitives[0]->box, primitives[1]->box);
        center = 0.5f * (box.maxPoint + box.minPoint);
        return;
    } else {
        vector<Primitive *> leftList;
        vector<Primitive *> rightList;

        auto *tri = primitives[0];
        float min = tri->center[axis];
        float max = min;

        // get min and max value in given axis
        for (const auto &p: primitives) {
            float tmp = p->center[axis];
            if (tmp < min) {
                min = tmp;
            }
            if (tmp > max) {
                max = tmp;
            }
        }
        float mid = 0.5f * (min + max);
        for (const auto p: primitives) {
            if (p->center[axis] < mid) {
                leftList.push_back(p);
            } else {
                rightList.push_back(p);
            }
        }

        // change the compare axis
        axis = random();
        left = new BVHNode(leftList, axis);
        right = new BVHNode(rightList, axis);
        box = BoundingBox::combine(left->box, right->box);
        center = 0.5f * (box.maxPoint + box.minPoint);
    }
}

BVHNode::~BVHNode() = default;

bool BVHNode::hit(Ray &ray, float tMin, float tMax, HitRecord &record) {
    if (box.hit(ray, tMin, tMax)) {
        HitRecord leftRecord, rightRecord;
        bool left_hit = left && left->hit(ray, tMin, tMax, leftRecord);
        bool right_hit = right && right->hit(ray, tMin, tMax, rightRecord);
        if (left_hit && right_hit) {
            record = leftRecord.t < rightRecord.t ? leftRecord : rightRecord;
            return true;
        } else if (left_hit) {
            record = leftRecord;
            return true;
        } else if (right_hit) {
            record = rightRecord;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

void BVHNode::destroy() {
    if (left) {
        left->destroy();
    }
    if (right) {
        right->destroy();
    }
    delete this;
}
