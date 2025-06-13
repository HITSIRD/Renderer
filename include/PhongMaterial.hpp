//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_PHONGMATERIAL_HPP
#define RENDERER_PHONGMATERIAL_HPP

#include "Material.hpp"

class PhongMaterial : public Material {
public:
    PhongMaterial();

    ~PhongMaterial() override = default;

    void destroy() override;
};

#endif //RENDERER_PHONGMATERIAL_HPP
