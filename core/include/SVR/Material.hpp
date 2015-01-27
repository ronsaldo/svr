#ifndef _SVR_MATERIAL_HPP_
#define _SVR_MATERIAL_HPP_

#include "SVR/Texture.hpp"

namespace SVR
{
DECLARE_CLASS(Material)

/**
 * Material type
 */
enum class MaterialType
{
    SolidColor = 0,
    Textured,
    LinearGradient,
    GammaCorrected
};

/**
 * Material structure
 */
class Material
{
public:
    MaterialType type;
    TexturePtr texture;
    glm::vec2 firstPoint, secondPoint;
    float gamma;

    Material() {}
    Material(MaterialType materialType)
        : type(materialType) {}

    bool equals(const MaterialPtr &other) const
    {
        return type == other->type && texture == other->texture;
    }

    static Material solidColor()
    {
        return Material(MaterialType::SolidColor);
    }
};

} // namespace SVR

#endif //_SVR_MATERIAL_HPP_

