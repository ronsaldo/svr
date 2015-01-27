#ifndef _SVR_MESH_HPP_
#define _SVR_MESH_HPP_

#include <stdlib.h>
#include "SVR/Vertex2D.hpp"
#include "SVR/PrimitiveMode.hpp"
#include "SVR/Material.hpp"

namespace SVR
{
DECLARE_CLASS(SubMesh)

/*
 * SubMesh
 */
class SubMesh
{
public:
    int startIndex;
    int indexCount;
    int startVertex;
    int endVertex;
    PrimitiveMode primitiveMode;
    MaterialPtr material;
};

typedef std::vector<SubMeshPtr> SubMeshes;

/**
 * Mesh
 */
template<typename VT, typename IT=unsigned short>
class Mesh
{
public:
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
