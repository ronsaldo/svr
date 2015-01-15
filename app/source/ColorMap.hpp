#ifndef _SVR_COLOR_MAP_HPP_
#define _SVR_COLOR_MAP_HPP_

#include <glm/glm.hpp>
#include <vector>
#include <math.h>

namespace SVR
{
/**
 * Color map
 */
class ColorMap
{
public:
    ColorMap() {}
    ~ColorMap() {}

    template<int N>
    ColorMap(glm::vec4 (&theColors) [N])
    {
        colors.reserve(N);
        for(int i = 0; i < N; ++i)
            colors.push_back(uncorrectColor(theColors[i]));
    }

    std::vector<glm::vec4> colors;

public:
    static ColorMap gray()
    {
        glm::vec4 colors[] {
            glm::vec4(0.0, 0.0, 0.0, 1.0),
            glm::vec4(1.0, 1.0, 1.0, 1.0),
        };

        return ColorMap(colors);
    }

    static glm::vec4 uncorrectColor(const glm::vec4 &color)
    {
        const float gamma = 2.2;
        return glm::vec4(pow(color.r, gamma), pow(color.g, gamma), pow(color.b, gamma), color.a);
    }
};

} // namespace SVR

#endif //_SVR_COLOR_MAP_HPP_
