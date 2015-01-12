#ifndef _SVR_MESH_HPP_
#define _SVR_MESH_HPP_

#include "SVR/Vertex2D.hpp"

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
};

/**
 * Mesh
 */
template<typename VertexType>
struct Mesh
{
    std::vector<VertexType> vertices;
    std::vector<int> indices;
    std::vector<SubMesh> submeshes;
};

typedef Mesh<Vertex2D> Mesh2D;

} // namespace SVR
#endif //_SVR_MESH_HPP_
