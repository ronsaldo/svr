#ifndef _SVR_MATERIAL_HPP_
#define _SVR_MATERIAL_HPP_

#include "SVR/Texture.hpp"

namespace SVR
{

/**
 * Material type
 */
enum class MaterialType
{
    SolidColor = 0,
    Textured,
    LinearGradient
};

/**
 * Material structure
 */
struct Material
{
    MaterialType type;
    TexturePtr texture;
    glm::vec2 firstPoint, secondPoint;

    Material() {}
    Material(MaterialType materialType)
        : type(materialType) {}

    bool operator==(const Material &other) const
    {
        return type == other.type && texture == other.texture;
    }

    bool operator!=(const Material &other) const
    {
        return type != other.type || texture != other.texture;
    }

    static Material solidColor()
    {
        return Material(MaterialType::SolidColor);
    }
};

} // namespace SVR

#endif //_SVR_MATERIAL_HPP_

