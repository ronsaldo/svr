#ifndef _SVR_MESH_BUILDER_HPP_
#define _SVR_MESH_BUILDER_HPP_

#include "SVR/Mesh.hpp"

namespace SVR
{
/**
 * MeshBuilder
 */
template<typename MT>
class MeshBuilder
{
public:
    typedef MT MeshType;
    typedef typename MeshType::VertexType VertexType;
    typedef typename MeshType::IndexType IndexType;

    MeshBuilder(MeshType &mesh)
        : mesh(mesh)
    {
        clear();
    }

    void clear()
    {
        mesh.vertices.clear();
        mesh.indices.clear();
        mesh.submeshes.clear();
        currentSubMesh = nullptr;
    }

    void beginSubMesh(PrimitiveMode primitiveMode, Material material)
    {
        startIndex = mesh.indices.size();
        startVertex = mesh.vertices.size();

        // Avoid creating submeshes that can be merged.
        if(!currentSubMesh || currentSubMesh->primitiveMode != primitiveMode
            || primitivesCanBeMerged(primitiveMode)
            ||currentSubMesh->material != material)
        {
            mesh.submeshes.push_back(SubMesh());
            currentSubMesh = &mesh.submeshes.back();
            currentSubMesh->startIndex = startIndex;
            currentSubMesh->startVertex = startVertex;
            currentSubMesh->primitiveMode = primitiveMode;
            currentSubMesh->material = material;
        }
    }

    void addVertex(const VertexType &vertex)
    {
        mesh.vertices.push_back(vertex);
    }

    void addIndex(IndexType index)
    {
        mesh.indices.push_back(index + startVertex);
    }

    void endSubMesh()
    {
        currentSubMesh->indexCount = getIndexCount();
        currentSubMesh->endVertex = mesh.vertices.size() - 1;
    }

private:
    MeshType &mesh;

    size_t getIndexCount() const
    {
        return mesh.indices.size() - startIndex;
    }

    bool primitivesCanBeMerged(PrimitiveMode mode)
    {
        switch(mode)
        {
        case PrimitiveMode::Points:
        case PrimitiveMode::Lines:
        case PrimitiveMode::Triangles:
            return true;
        default:
            return false;
        }
    }

    size_t startIndex;
    size_t startVertex;
    SubMesh *currentSubMesh;
};

template<typename MT>
class LocalMeshBuilder: public MeshBuilder<MT>
{
public:
    LocalMeshBuilder()
        : MeshBuilder<MT> (localMesh) {}

    MT localMesh;
};

typedef MeshBuilder<Mesh2D> Mesh2DBuilder;
typedef LocalMeshBuilder<Mesh2D> LocalMesh2DBuilder;

} // namespace SVR

#endif //_SVR_MESH_BUILDER_HPP_
