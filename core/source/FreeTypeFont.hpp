#ifndef _SVR_FREETYPE_FONT_HPP_
#define _SVR_FREETYPE_FONT_HPP_

#include <string>
#include <vector>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdint.h>
#include "SVR/Renderer.hpp"

namespace SVR
{
DECLARE_CLASS(FreeTypeFont);
DECLARE_CLASS(GlyphSheet);

/**
 * Glyph sheet
 */
class GlyphSheet
{
public:
    GlyphSheet(int width, int height);
    ~GlyphSheet();

    int getWidth() const;
    int getHeight() const;

    bool allocate(int glyphWidth, int glyphHeight, int *x, int *y);

    void copyBitmap(int sourceWidth, int sourceHeight, int sourcePitch, uint8_t *sourceBuffer, int destX, int destY);

    void update(Renderer *renderer);
    const Texture2DPtr &getTexture() const;

private:
    int width, height;
    std::unique_ptr<uint8_t[]> imageData;

    int rowHeight;
    int rowPosition;
    int columnPosition;

    uint8_t *pixels;
    int pitch;

    Texture2DPtr texture;
    bool changed;
};

/**
 * FreeType Font
 */
class FreeTypeFont
{
public:
    ~FreeTypeFont();
    glm::vec2 drawText(Renderer *renderer, const glm::vec2 &position, const std::string &text);

    static FreeTypeFontPtr loadFromFile(const std::string &path);

private:

    /**
     * Glyph description
     */
    struct GlyphDescription
    {
        GlyphSheetPtr sheet;
        glm::vec2 sheetPosition;
        glm::vec2 sheetExtent;
        glm::vec2 extent;

        glm::vec2 bearing;
        glm::vec2 advance;        
    };

    FreeTypeFont(FT_Library library, FT_Face face, int fontHeight);

    void checkCache(int character);
    glm::vec2 drawCharacter(Renderer *renderer, const glm::vec2 &position, int character);

    FT_Library library;
    FT_Face face;
    int fontHeight;

    std::vector<GlyphSheetPtr> glyphSheets;
    std::map<int, GlyphDescription> glyphDescriptions;

};

} // namespace SVR

#endif //_SVR_FREETYPE_FONT_HPP_
