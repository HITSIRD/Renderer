//
// Created by 闻永言 on 2021/8/28.
//

#ifndef RENDERER_FLATMATERIAL_HPP
#define RENDERER_FLATMATERIAL_HPP

#include "Material.hpp"

class FlatMaterial: public Material
{
public:
    FlatMaterial();

    ~FlatMaterial() override = default;

    void destroy() override;
};

#endif //RENDERER_FLATMATERIAL_HPP
