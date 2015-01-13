#ifndef _SVR_MESH_HPP_
#define _SVR_MESH_HPP_

#include <stdlib.h>
#include "SVR/Vertex2D.hpp"
#include "SVR/PrimitiveMode.hpp"
#include "SVR/Material.hpp"

namespace SVR
{
/**
 * SubMesh
 */
struct SubMesh
{
    int startIndex;
    int indexCount;
    int startVertex;
    int endVertex;
    PrimitiveMode primitiveMode;
    Material material;
};

typedef std::vector<SubMesh> SubMeshes;

/**
 * Mesh
 */
template<typename VT, typename IT=unsigned short>
struct Mesh
{
    typedef VT VertexType;
    typedef IT IndexType;

    std::vector<VertexType> vertices;
    std::vector<IndexType> indices;
    SubMeshes submeshes;

    size_t getVerticesByteSize() const
    {
        return vertices.size() * sizeof(VertexType);
    }

    size_t getIndicesByteSize() const
    {
        return indices.size() * sizeof(IndexType);
    }

};

typedef Mesh<Vertex2D> Mesh2D;

} // namespace SVR
#endif //_SVR_MESH_HPP_
