#include <algorithm>
#include "SVR/Logging.hpp"
#include "FreeTypeFont.hpp"

namespace SVR
{
const int GlyphSheetWidth = 512;
const int GlyphSheetHeight = 512;

GlyphSheet::GlyphSheet(int width, int height)
    : width(width), height(height), imageData(new uint8_t[width*height])
{
    memset(imageData.get(), 0, width*height);

    rowHeight = 0;
    rowPosition = 0;
    columnPosition = 0;
    pixels = imageData.get() + width*(height - 1);
    pitch = -width;

    changed = true;
}

GlyphSheet::~GlyphSheet()
{
}

int GlyphSheet::getWidth() const
{
    return width;
}

int GlyphSheet::getHeight() const
{
    return height;
}

bool GlyphSheet::allocate(int glyphWidth, int glyphHeight, int *x, int *y)
{
    // Allocate in the current row
    if(columnPosition + glyphWidth <= width)
    {
        auto newRowHeight = std::max(rowHeight, glyphHeight);
        if(newRowHeight <= height)
        {
            rowHeight = newRowHeight;

            *y = rowPosition;
            *x = columnPosition;

            // Move to the next column
            columnPosition += glyphWidth;
            return true;
        }
    }

    // Allocate in the next row.
    if(rowHeight + rowPosition + glyphHeight <= height &&
       glyphWidth <= width)
    {
        rowPosition += rowHeight;
        columnPosition = 0;
        rowHeight = glyphHeight;

        *y = rowPosition;
        *x = columnPosition;

        // Move to the next column
        columnPosition += glyphWidth;
        return true;
    }

    return false;
}

void GlyphSheet::copyBitmap(int sourceWidth, int sourceHeight, int sourcePitch, uint8_t *sourceBuffer, int destX, int destY)
{
    assert(sourceWidth + destX <= width);
    assert(sourceHeight + destY <= height);

    for(int y = 0; y < sourceHeight; ++y)
    {
        auto *sourceRow = sourceBuffer + sourcePitch*y;
        auto *destRow = pixels + pitch*(destY + y) + destX;
        for(int x = 0; x < sourceWidth; ++x)
            *destRow++ = *sourceRow++;
    }

    changed = true;
}

void GlyphSheet::update(Renderer *renderer)
{
    if(!changed)
        return;

    // Create the texture if it was not already created.
    if(!texture)
    {
        texture = renderer->createTexture2D(width, height, PixelFormat::I8);
        texture->setMagnificationFilter(TextureFilter::Linear);
        texture->setMinificationFilter(TextureFilter::Linear);
    }

    texture->upload(PixelFormat::I8, width*height, imageData.get());
    changed = false;
}

const Texture2DPtr &GlyphSheet::getTexture() const
{
    return texture;
}

FreeTypeFont::FreeTypeFont(FT_Library library, FT_Face face, int fontHeight)
    : library(library), face(face), fontHeight(fontHeight)
{
}

FreeTypeFont::~FreeTypeFont()
{
}

glm::vec2 FreeTypeFont::drawText(Renderer *renderer, const glm::vec2 &position, const std::string &text)
{
    // Ensure all the characters are available
    for(auto character: text)
        checkCache(character);

    // Draw them.
    glm::vec2 currentPosition = position;
    for(auto character: text)
        currentPosition = drawCharacter(renderer, currentPosition, character);
    return currentPosition;
}

glm::vec2 FreeTypeFont::drawCharacter(Renderer *renderer, const glm::vec2 &position, int character)
{
    glm::vec2 newPosition = position;

    // Get the glyph description.
    auto it = glyphDescriptions.find(character);
    if(it == glyphDescriptions.end())
        return newPosition;

    // Use the glyph sheet.
    auto &glyphDesc = it->second;
    auto &sheet = glyphDesc.sheet;
    sheet->update(renderer);
    renderer->setTexture(sheet->getTexture());

    // Draw the glyph.
    glm::vec2 disp = glm::vec2(glyphDesc.bearing.x, glyphDesc.bearing.y - glyphDesc.extent.y);
    renderer->drawGlyph(glyphDesc.sheetPosition, glyphDesc.sheetExtent, position + disp, glyphDesc.extent);

    //printf("Bearing advance: %f %f | %f %f\n", glyphDesc.bearing.x, glyphDesc.bearing.y, glyphDesc.advance.x, glyphDesc.advance.y);

    return newPosition + glyphDesc.advance;
}

void FreeTypeFont::checkCache(int character)
{
    // Check for the existing glyph.
    auto it = glyphDescriptions.find(character);
    if(it != glyphDescriptions.end())
        return;

    // Get the glyph index.
    auto glyphIndex = FT_Get_Char_Index(face, character);

    // Load the glyph image.
    auto error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
    if(error)
    {
        logError("Failed to load glyph");
        return;
    }

    // Render the glyph.
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if(error)
    {
        logError("Failed to render glyph");
        return;
    }

    // Get the bitmap info.
    auto &bitmap = face->glyph->bitmap;
    auto width = bitmap.width;
    auto height = bitmap.rows;

    // Allocate space in a glyph sheet.
    int x, y;
    GlyphSheetPtr sheet;
    for(const auto &currentSheet: glyphSheets)
    {
        if(currentSheet->allocate(width+1, height+1, &x, &y))
        {
            sheet = currentSheet;
            break;
        }
    }

    // Add a new sheet if needed.
    if(!sheet)
    {
        sheet = std::make_shared<GlyphSheet> (GlyphSheetWidth, GlyphSheetHeight);
        sheet->allocate(width+1, height+1, &x, &y);
        assert(x == 0 && y == 0);
        glyphSheets.push_back(sheet);
    }

    // Copy the bitmap to the sheet.
    sheet->copyBitmap(width, height, bitmap.pitch, bitmap.buffer, x, y);

    // Create the glyph description
    GlyphDescription desc;
    desc.sheet = sheet;
    desc.sheetExtent = glm::vec2(width, height) / glm::vec2(sheet->getWidth(), sheet->getHeight());
    desc.sheetPosition = glm::vec2(x, y) / glm::vec2(sheet->getWidth(), sheet->getHeight());
    desc.sheetPosition.y = 1.0 - desc.sheetPosition.y - desc.sheetExtent.y;
    desc.extent = glm::vec2(width, height);

    auto &metrics = face->glyph->metrics;
    desc.bearing = glm::vec2(metrics.horiBearingX, metrics.horiBearingY) / 64.0f;
    desc.advance = glm::vec2(metrics.horiAdvance, 0.0) / 64.0f;

    // Store the glyph in the cache.
    glyphDescriptions.insert(std::make_pair(character, desc));
}

FreeTypeFontPtr FreeTypeFont::loadFromFile(const std::string &path)
{
    FT_Library library;
    auto error = FT_Init_FreeType(&library);
    if(error)
    {
        fatalError("Failed to initialize FreeType");
        return FreeTypeFontPtr();
    }

    FT_Face face;
    error = FT_New_Face(library, path.c_str(),0, &face);
    if(error)
    {
        logError("Failed to open font");
        return FreeTypeFontPtr();
    }

    error = FT_Set_Char_Size(face, 0, 16*64, 72, 72);
    if(error)
    {
        logError("Failed to set font size");
        return FreeTypeFontPtr();
    }

    return FreeTypeFontPtr(new FreeTypeFont (library, face, 18));
}


} // namespace SVR

