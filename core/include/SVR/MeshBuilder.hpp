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

    void setMesh(MeshType *mesh)
    {
	this->mesh = mesh;
        clear();
    }

    void clear()
    {
        mesh->vertices.clear();
        mesh->indices.clear();
        mesh->submeshes.clear();
        currentSubMesh = nullptr;
    }

    void beginSubMesh(PrimitiveMode primitiveMode, const MaterialPtr &material)
    {
        startIndex = mesh->indices.size();
        startVertex = mesh->vertices.size();

        // Avoid creating submeshes that can be merged.
        if(!currentSubMesh || currentSubMesh->primitiveMode != primitiveMode
            || primitivesCanBeMerged(primitiveMode)
            || currentSubMesh->material->equals(material))
        {
            mesh->submeshes.push_back(std::make_shared<SubMesh> ());
            currentSubMesh = mesh->submeshes.back();
            currentSubMesh->startIndex = startIndex;
            currentSubMesh->startVertex = startVertex;
            currentSubMesh->primitiveMode = primitiveMode;
            currentSubMesh->material = material;
        }
    }

    void addVertex(const VertexType &vertex)
    {
        mesh->vertices.push_back(vertex);
    }

    void addIndex(IndexType index)
    {
        mesh->indices.push_back(index + startVertex);
    }

    void endSubMesh()
    {
        currentSubMesh->indexCount = getIndexCount();
        currentSubMesh->endVertex = mesh->vertices.size() - 1;
    }

protected:
    MeshType *mesh;

private:
    size_t getIndexCount() const
    {
        return mesh->indices.size() - startIndex;
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
    SubMeshPtr currentSubMesh;
};

template<typename MT>
class LocalMeshBuilder: public MeshBuilder<MT>
{
public:
    LocalMeshBuilder()
    {
        localMesh = std::make_shared<MT> ();
	MeshBuilder<MT>::setMesh(localMesh.get());
    }

    std::shared_ptr<MT> localMesh;
};

typedef MeshBuilder<Mesh2D> Mesh2DBuilder;
typedef LocalMeshBuilder<Mesh2D> LocalMesh2DBuilder;

} // namespace SVR

#endif //_SVR_MESH_BUILDER_HPP_
