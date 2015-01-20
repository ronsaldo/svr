#include "GLRenderer.hpp"
#include "FreeTypeFont.hpp"

namespace SVR
{
namespace OpenGL
{

/**
 * The OpenGL renderer
 */
class GLRenderer: public Renderer
{
public:
    GLRenderer();
    ~GLRenderer();

    virtual bool initialize(int argc, const char **argv);
    virtual void shutdown();

    virtual void setScreenSize(const glm::vec2 &newScreenSize);

    virtual void clearColor(const glm::vec4 &color);
    virtual void clear();

    virtual void setTexture(const Texture2DPtr &texture);
    virtual void setColor(const glm::vec4 &color);
    virtual void setLinearGradient(const Texture1DPtr &texture, const glm::vec2 &start, const glm::vec2 &end);
    virtual void setGammaCorrection(float gammaCorrection);

    virtual void drawLine(const glm::vec2 &start, const glm::vec2 &end);
    virtual void drawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c);
    virtual void drawRectangle(const glm::vec2 &min, const glm::vec2 &max);
    virtual void drawText(const glm::vec2 &position, const std::string &text);
    virtual void drawGlyph(const glm::vec2 &sheetPosition, const glm::vec2 &sheetExtent, const glm::vec2 &position, const glm::vec2 &extent);

    virtual void flushCommands();

    virtual Texture1DPtr createTexture1D(int width, PixelFormat pixelFormat);
    virtual Texture2DPtr createTexture2D(int width, int height, PixelFormat pixelFormat);

    virtual void beginCompute();
    virtual void endCompute();

    virtual const glm::mat3 &getModelViewMatrix() const;
    virtual void setModelViewMatrix(const glm::mat3 &matrix);

    virtual void useMainFramebuffer();
    virtual FramebufferPtr createFramebuffer(int width, int height);

private:
    bool loadPrograms();
    void create2DCanvas();

    void updateTextureMatrix();
    void useProgram(const ProgramPtr &program);
    void useMaterial(const Material &material);
    void drawSubMeshes(const SubMeshes &submeshes, size_t elementSize, GLenum elementType);

    void useSolidColorMaterial(const Material &material);
    void useTexturedMaterial(const Material &material);
    void useLinearGradientMaterial(const Material &material);
    void useGammaCorrectedMaterial(const Material &material);

    Vertex2D vertexForPosition(const glm::vec2 &position);
    Vertex2D vertexForGlyph(const glm::vec2 &sheetPosition, const glm::vec2 &position);

    ProgramPtr colorProgram;
    ProgramPtr texturedProgram;
    ProgramPtr linearGradientProgram;
    ProgramPtr gammaCorrectionProgram;

    LocalMesh2DBuilder builder;
    BufferObjectPtr vertexBuffer;
    BufferObjectPtr indexBuffer;

    // Font
    FreeTypeFontPtr defaultFont;

    // Render state
    glm::vec2 screenSize;
    glm::mat4 projectionMatrix;

    glm::mat3 modelViewMatrix;
    glm::mat3 textureMatrix;
    glm::mat3 textureTransformMatrix;
    glm::mat3 textureCoordinateMatrix;
    glm::vec4 currentColor;
    Material currentMaterial;
};

GLRenderer::GLRenderer()
{
}

GLRenderer::~GLRenderer()
{
}

bool GLRenderer::initialize(int argc, const char **argv)
{
    auto glewError = glewInit();
    if (GLEW_OK != glewError)
    {
        logError("Failed to load OpenGL extensions");
        return false;
    }

    if(!loadPrograms())
        return false;

    defaultFont = FreeTypeFont::loadFromFile("data/fonts/dejavu/DejaVuSans.ttf");
    if(!defaultFont)
        return false;

    create2DCanvas();

    // Enable alpha blending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void GLRenderer::create2DCanvas()
{
    vertexBuffer = BufferObject::create(GL_ARRAY_BUFFER, GL_STREAM_DRAW);
    indexBuffer = BufferObject::create(GL_ELEMENT_ARRAY_BUFFER, GL_STREAM_DRAW);
}

void GLRenderer::shutdown()
{
}

bool GLRenderer::loadPrograms()
{
    colorProgram = Program::loadFromFile("data/shaders/color.glslv", "data/shaders/color.glslf");
    texturedProgram = Program::loadFromFile("data/shaders/textured.glslv", "data/shaders/textured.glslf");
    linearGradientProgram = Program::loadFromFile("data/shaders/linearGradient.glslv", "data/shaders/linearGradient.glslf");
    gammaCorrectionProgram = Program::loadFromFile("data/shaders/gammaCorrection.glslv", "data/shaders/gammaCorrection.glslf");
    return colorProgram && texturedProgram && linearGradientProgram && gammaCorrectionProgram;
}

void GLRenderer::updateTextureMatrix()
{
    textureMatrix = textureCoordinateMatrix * textureTransformMatrix;
}

void GLRenderer::setScreenSize(const glm::vec2 &newScreenSize)
{
    screenSize = newScreenSize;
    projectionMatrix = glm::ortho(0.0f, screenSize.x, 0.0f, screenSize.y, -1.0f, 1.0f);
    glViewport(0, 0, int(screenSize.x), int(screenSize.y));
}

void GLRenderer::clearColor(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void GLRenderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLRenderer::setColor(const glm::vec4 &color)
{
    currentMaterial = Material(MaterialType::SolidColor);
    currentColor = color;
}

void GLRenderer::setTexture(const Texture2DPtr &texture)
{
    currentMaterial = Material(MaterialType::Textured);
    currentMaterial.texture = texture;
    currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

    textureCoordinateMatrix = glm::mat3();
    textureCoordinateMatrix[0][0] = 1.0 / texture->getWidth();
    textureCoordinateMatrix[1][1] = 1.0 / texture->getHeight();
    updateTextureMatrix();
}

void GLRenderer::setLinearGradient(const Texture1DPtr &texture, const glm::vec2 &start, const glm::vec2 &end)
{
    currentMaterial = Material(MaterialType::LinearGradient);
    currentMaterial.texture = texture;
    currentMaterial.firstPoint = start;
    currentMaterial.secondPoint = end;
    currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

    textureCoordinateMatrix = glm::mat3();
    updateTextureMatrix();
}

void GLRenderer::setGammaCorrection(float gammaCorrection)
{
    currentMaterial.type = MaterialType::GammaCorrected;
    currentMaterial.gamma = gammaCorrection;
}

void GLRenderer::drawLine(const glm::vec2 &start, const glm::vec2 &end)
{
    builder.beginSubMesh(PrimitiveMode::Lines, currentMaterial);
    builder.addVertex(vertexForPosition(start));
    builder.addVertex(vertexForPosition(end));
    builder.addIndex(0);
    builder.addIndex(1);
    builder.endSubMesh();
}

void GLRenderer::drawTriangle(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &c)
{
    builder.beginSubMesh(PrimitiveMode::Triangles, currentMaterial);
    builder.addVertex(vertexForPosition(a));
    builder.addVertex(vertexForPosition(b));
    builder.addVertex(vertexForPosition(c));
    builder.addIndex(0);
    builder.addIndex(1);
    builder.addIndex(2);
    builder.endSubMesh();
}

void GLRenderer::drawRectangle(const glm::vec2 &min, const glm::vec2 &max)
{
    auto bottomLeft = min;
    auto topLeft = glm::vec2(min.x, max.y);
    auto topRight = max;
    auto bottomRight = glm::vec2(max.x, min.y);

    builder.beginSubMesh(PrimitiveMode::Triangles, currentMaterial);
    builder.addVertex(vertexForPosition(bottomLeft));
    builder.addVertex(vertexForPosition(topLeft));
    builder.addVertex(vertexForPosition(topRight));
    builder.addVertex(vertexForPosition(bottomRight));

    builder.addIndex(0);
    builder.addIndex(1);
    builder.addIndex(2);

    builder.addIndex(2);
    builder.addIndex(3);
    builder.addIndex(0);
    builder.endSubMesh();
}

void GLRenderer::drawText(const glm::vec2 &position, const std::string &text)
{
    defaultFont->drawText(this, position, text);
}

void GLRenderer::drawGlyph(const glm::vec2 &sheetPosition, const glm::vec2 &sheetExtent, const glm::vec2 &position, const glm::vec2 &extent)
{
    auto glyphBottomLeft = sheetPosition;
    auto glyphTopLeft = sheetPosition + glm::vec2(0.0, sheetExtent.y);
    auto glyphTopRight = sheetPosition + sheetExtent;
    auto glyphBottomRight = sheetPosition + glm::vec2(sheetExtent.x, 0.0);

    auto bottomLeft = position;
    auto topLeft = position + glm::vec2(0.0, extent.y);
    auto topRight = position + extent;
    auto bottomRight = position + glm::vec2(extent.x, 0.0);

    builder.beginSubMesh(PrimitiveMode::Triangles, currentMaterial);
    builder.addVertex(vertexForGlyph(glyphBottomLeft, bottomLeft));
    builder.addVertex(vertexForGlyph(glyphTopLeft, topLeft));
    builder.addVertex(vertexForGlyph(glyphTopRight, topRight));
    builder.addVertex(vertexForGlyph(glyphBottomRight, bottomRight));

    builder.addIndex(0);
    builder.addIndex(1);
    builder.addIndex(2);

    builder.addIndex(2);
    builder.addIndex(3);
    builder.addIndex(0);
    builder.endSubMesh();
}

Vertex2D GLRenderer::vertexForPosition(const glm::vec2 &position)
{
    glm::vec3 transformedPosition = modelViewMatrix * glm::vec3(position, 1.0);
    glm::vec3 transformedTexcoord = textureMatrix * glm::vec3(position, 1.0);

    Vertex2D vertex;
    vertex.position = glm::vec2(transformedPosition);
    vertex.color = currentColor;
    vertex.texcoord = glm::vec2(transformedTexcoord);
    return vertex;
}

Vertex2D GLRenderer::vertexForGlyph(const glm::vec2 &sheetPosition, const glm::vec2 &position)
{
    glm::vec3 transformedPosition = modelViewMatrix * glm::vec3(position, 1.0);

    Vertex2D vertex;
    vertex.position = glm::vec2(transformedPosition);
    vertex.color = currentColor;
    vertex.texcoord = sheetPosition;
    return vertex;
}

void GLRenderer::flushCommands()
{
    typedef Vertex2D VertexType;

    auto &mesh = builder.localMesh;
    if(mesh.vertices.empty() || mesh.indices.empty() || mesh.submeshes.empty())
        return;

    // Upload the vertices and the indices.
    vertexBuffer->uploadData(mesh.getVerticesByteSize(), &mesh.vertices[0]);
    indexBuffer->uploadData(mesh.getIndicesByteSize(), &mesh.indices[0]);

    // Enable the vertex attributes
    glEnableVertexAttribArray((GLuint)VertexAttribute::Position);
    glVertexAttribPointer((GLuint)VertexAttribute::Position, 2, GL_FLOAT, GL_FALSE, sizeof(VertexType),
            reinterpret_cast<void*> (offsetof(VertexType, position)));

    glEnableVertexAttribArray((GLuint)VertexAttribute::TexCoord);
    glVertexAttribPointer((GLuint)VertexAttribute::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(VertexType),
            reinterpret_cast<void*> (offsetof(VertexType, texcoord)));

    glEnableVertexAttribArray((GLuint)VertexAttribute::Color);
    glVertexAttribPointer((GLuint)VertexAttribute::Color, 4, GL_FLOAT, GL_FALSE, sizeof(VertexType),
            reinterpret_cast<void*> (offsetof(VertexType, color)));

    // Draw the submeshes
    drawSubMeshes(mesh.submeshes, 2, GL_UNSIGNED_SHORT);

    // Clear the canvas.
    builder.clear();
}

Texture1DPtr GLRenderer::createTexture1D(int width, PixelFormat pixelFormat)
{
    return GLTexture1D::create(width, pixelFormat);
}

Texture2DPtr GLRenderer::createTexture2D(int width, int height, PixelFormat pixelFormat)
{
    return GLTexture2D::create(width, height, pixelFormat);
}

void GLRenderer::useProgram(const ProgramPtr &program)
{
    // Use the program.
    glUseProgram(program->getHandle());

    // Set the program attributes.
    program->uniformMat4("projectionMatrix", projectionMatrix);
}

void GLRenderer::useMaterial(const Material &material)
{
    switch(material.type)
    {
    case MaterialType::SolidColor:
        useSolidColorMaterial(material);
        break;
    case MaterialType::Textured:
        useTexturedMaterial(material);
        break;
    case MaterialType::LinearGradient:
        useLinearGradientMaterial(material);
        break;
    case MaterialType::GammaCorrected:
        useGammaCorrectedMaterial(material);
        break;
    default:
        abort();
    }

}

void GLRenderer::useSolidColorMaterial(const Material &material)
{
    useProgram(colorProgram);
}

void GLRenderer::useTexturedMaterial(const Material &material)
{
    useProgram(texturedProgram);

    auto glTexture = std::static_pointer_cast<GLTexture2D> (material.texture);
    glTexture->bind();
}

void GLRenderer::useLinearGradientMaterial(const Material &material)
{
    useProgram(linearGradientProgram);

    auto glTexture = std::static_pointer_cast<GLTexture1D> (material.texture);
    glTexture->bind();

    auto direction = material.secondPoint - material.firstPoint;

    linearGradientProgram->uniformVec2("gradientStart", material.firstPoint);
    linearGradientProgram->uniformVec2("gradientDirection", glm::normalize(direction));
    linearGradientProgram->uniformFloat("gradientLength", glm::length(direction));
}

void GLRenderer::useGammaCorrectedMaterial(const Material &material)
{
    useProgram(gammaCorrectionProgram);

    auto glTexture = std::static_pointer_cast<GLTexture2D> (material.texture);
    glTexture->bind();

    gammaCorrectionProgram->uniformFloat("inverseGammaFactor", 1.0 / material.gamma);
}

void GLRenderer::drawSubMeshes(const SubMeshes &submeshes, size_t elementSize, GLenum elementType)
{
    // TODO: Select the proper program.
    for(auto &submesh: submeshes)
    {
        useMaterial(submesh.material);

        auto mode = mapPrimitiveMode(submesh.primitiveMode);
        size_t startIndexOffset = elementSize * submesh.startIndex;
        glDrawRangeElements(mode, submesh.startVertex, submesh.endVertex,
                            submesh.indexCount, elementType,
                            reinterpret_cast<void*> (startIndexOffset));

    }
}

void GLRenderer::beginCompute()
{
    glFlush();
}

void GLRenderer::endCompute()
{
}

const glm::mat3 &GLRenderer::getModelViewMatrix() const
{
    return modelViewMatrix;
}

void GLRenderer::setModelViewMatrix(const glm::mat3 &matrix)
{
    modelViewMatrix = matrix;
}

FramebufferPtr GLRenderer::createFramebuffer(int width, int height)
{
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    return std::make_shared<GLFramebuffer> (fbo, width, height);
}

void GLRenderer::useMainFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace OpenGL

RendererPtr createRenderer()
{
    return std::make_shared<OpenGL::GLRenderer> ();
}
} // namespace SVR

