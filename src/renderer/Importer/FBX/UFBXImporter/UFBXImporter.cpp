#include "renderer/Precomp.h"
#include "UFBXImporter.h"
#include "core/Timer/Timer.h"
#include "renderer/Importer/SceneImporterData.h"
#include "core/File/File.h"
#include "ufbx/ufbx.c"

using namespace vg::core;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    UFBXImporter::UFBXImporter()
    {

    }

    //--------------------------------------------------------------------------------------
    UFBXImporter::~UFBXImporter()
    {

    }

    //--------------------------------------------------------------------------------------
    bool UFBXImporter::importFBX(const string & _path, SceneImporterData & _data)
    {
        ufbx_load_opts opts = {};
        opts.load_external_files = false,
        opts.allow_null_material = true,
        opts.generate_missing_normals = true,
        opts.evaluate_skinning = true,
        opts.target_axes.right = UFBX_COORDINATE_AXIS_POSITIVE_X;
        opts.target_axes.up = UFBX_COORDINATE_AXIS_POSITIVE_Y;
        opts.target_axes.front = UFBX_COORDINATE_AXIS_POSITIVE_Z;
        opts.target_unit_meters = 1.0f;

        ufbx_error error;
        ufbx_scene * scene = ufbx_load_file(_path.c_str(), &opts, &error);

        if (nullptr != scene)
        {
            const auto meshCount = (uint)scene->meshes.count;

            for (uint i = 0; i < meshCount; i++)
            {
                MeshImporterData meshImporterData;
                if (loadFBXMesh(scene->meshes[i], meshImporterData, scene->settings.unit_meters))
                    _data.meshes.push_back(meshImporterData);
            }

            ufbx_free_scene(scene);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    float2 getUFBXAttribute(uint _index, const ufbx_vertex_vec2 & _table, const float2 & _default)
    {
        if (_table.exists)
        {
            const auto & val = _table.values.data[_table.indices.data[_index]];
            return float2(val.x, val.y);
        }
        else
        {
            return _default;
        }
    }

    //--------------------------------------------------------------------------------------
    float3 getUFBXAttribute(uint _index, const ufbx_vertex_vec3 & _table, const float3 & _default)
    {
        if (_table.exists)
        {
            const auto & val = _table.values.data[_table.indices.data[_index]];
            return float3(val.x, val.y, val.z);
        }
        else
        {
            return _default;
        }
    }

    //--------------------------------------------------------------------------------------
    float4 getUFBXAttribute(uint _index, const ufbx_vertex_vec4 & _table, const float4 & _default)
    {
        if (_table.exists)
        {
            const auto & val = _table.values.data[_table.indices.data[_index]];
            return float4(val.x, val.y, val.z, val.w);
        }
        else
        {
            return _default;
        }
    }

    //--------------------------------------------------------------------------------------
    bool UFBXImporter::loadFBXMesh(const ufbx_mesh * _UFbxMesh, MeshImporterData & _data, double _scale)
    {
        const auto start = Timer::getTick();

        string name = _UFbxMesh->name.data;
        if (_UFbxMesh->instances.count > 0)
            name = _UFbxMesh->instances.data[0]->name.data;

        VG_DEBUGPRINT("[UFBXImporter] Importing FBX Mesh \"%s\" ...", name.c_str());
        _data.name = name;

        uint maxMaterialCount = 0, maxTriangleCount = 0, totalTriangleCount = 0;;
        for (size_t i = 0; i < _UFbxMesh->materials.count; i++)
        {
            const ufbx_mesh_material * UFbxMeshMat = &_UFbxMesh->materials.data[i];
            if (0 != UFbxMeshMat->num_triangles)
            {
                maxMaterialCount++;
                totalTriangleCount += (uint)UFbxMeshMat->num_triangles;
                maxTriangleCount = max(maxTriangleCount, (uint)UFbxMeshMat->num_triangles);
            }
        }

        // Reserve IB, VB and materials
        vector<u32> indexBuffer;
        indexBuffer.reserve(totalTriangleCount * 3);

        vector<MeshImporterVertex> vertexBuffer;
        vertexBuffer.reserve(totalTriangleCount);

        vector<Batch> batches;
        batches.reserve(maxMaterialCount);

        // Temp buffer for face triangulation
        size_t maxIndexCountPerFace = _UFbxMesh->max_face_triangles * 3;
        uint32_t * triangleIndexTmp = new uint32_t[maxIndexCountPerFace];

        // Bounding Box
        AABB aabb = AABB((float3)MAX_FLOAT, (float3)MIN_FLOAT);

        uint vertexIndex = 0;

        for (size_t i = 0; i < _UFbxMesh->materials.count; i++)
        {
            const ufbx_mesh_material * UFbxMeshMat = &_UFbxMesh->materials.data[i];

            if (UFbxMeshMat->num_triangles == 0)
                continue;

            uint batchIndexCount = 0;
            uint startIndex = (uint)indexBuffer.size();

            for (size_t j = 0; j < UFbxMeshMat->num_faces; j++)
            {
                ufbx_face face = _UFbxMesh->faces.data[UFbxMeshMat->face_indices.data[j]];
                uint triangleCount = ufbx_triangulate_face(triangleIndexTmp, maxIndexCountPerFace, _UFbxMesh, face);

                for (uint k = 0; k < triangleCount * 3; k++)
                {
                    uint index = triangleIndexTmp[2 - k];
                    //uint ix = _UFbxMesh->vertex_indices.data[index]; 

                    const auto pos = getUFBXAttribute(index, _UFbxMesh->vertex_position, float3(0, 0, 0));
                    const auto nrm = getUFBXAttribute(index, _UFbxMesh->vertex_normal,   float3(1, 0, 0));
                    const auto bin = getUFBXAttribute(index, _UFbxMesh->vertex_bitangent,float3(0, 1, 0));
                    const auto tan = getUFBXAttribute(index, _UFbxMesh->vertex_tangent,  float3(0, 0, 1));
                    const auto col = getUFBXAttribute(index, _UFbxMesh->vertex_color,    float4(1,1,1,1));
                    const auto uv0 = getUFBXAttribute(index, _UFbxMesh->vertex_uv,       float2(0, 0));

                    MeshImporterVertex vertex;

                    vertex.pos = float4(pos.x * _scale, pos.y * _scale, pos.z * _scale, 1.0f);
                    vertex.nrm = float4(nrm.x, nrm.y, nrm.z, 1.0f);
                    vertex.bin = float4(bin.x, bin.y, bin.z, 1.0f);
                    vertex.tan = float4(tan.x, tan.y, tan.z, 1.0f);
                    vertex.color = float4(col.x, col.y, col.z, col.w);
                    vertex.uv[0] = float2(uv0.x, 1.0f-uv0.y);
                    vertex.uv[1] = float2(0,1.0f - 0); // TODO: Support UV1+

                    vertexBuffer.push_back(vertex);
                    indexBuffer.push_back(vertexIndex);

                    aabb.grow(vertex.pos.xyz);

                    vertexIndex++;
                }

                batchIndexCount += (uint)triangleCount * 3;
            }

            Batch batch;
                  batch.count = batchIndexCount;
                  batch.offset = startIndex;
            batches.push_back(batch);
        }

        vector<MaterialImporterData> materials;
        for (auto i = 0; i < _UFbxMesh->materials.count; ++i)
        {
            const ufbx_mesh_material * UFbxMeshMat = &_UFbxMesh->materials[i];
            if (0 != UFbxMeshMat->num_triangles)
            {
                if (nullptr != UFbxMeshMat->material)
                {
                    MaterialImporterData matImportData;

                    matImportData.name = UFbxMeshMat->material->name.data;

                    const auto & albedo = UFbxMeshMat->material->pbr.maps[UFBX_MATERIAL_PBR_BASE_COLOR];
                    const auto & normal = UFbxMeshMat->material->pbr.maps[UFBX_MATERIAL_PBR_NORMAL_MAP];

                    matImportData.texturePath[(int)MaterialTextureType::Albedo] = io::cleanPath(albedo.texture ? albedo.texture->absolute_filename.data : "");
                    matImportData.texturePath[(int)MaterialTextureType::Albedo] = io::getRelativePath(matImportData.texturePath[(int)MaterialTextureType::Albedo]);

                    matImportData.texturePath[(int)MaterialTextureType::Normal] = io::cleanPath(normal.texture ? normal.texture->absolute_filename.data : "");
                    matImportData.texturePath[(int)MaterialTextureType::Normal] = io::getRelativePath(matImportData.texturePath[(int)MaterialTextureType::Normal]);

                    materials.push_back(matImportData);
                }
            }
        }

        bool optimize = true;

        core::unordered_map<MeshImporterVertex, uint, MeshImporterVertex::hash> vertexHash;

        if (optimize)
        {
            vector<u32> optimizedIndexBuffer;
            optimizedIndexBuffer.reserve(totalTriangleCount * 3);

            vector<MeshImporterVertex> optimizedVertexBuffer;
            optimizedVertexBuffer.reserve(totalTriangleCount);

            for (size_t i = 0; i < batches.size(); i++)
            {
                const auto batch = batches[i];

                for (uint j = 0; j < batch.count/3; ++j)
                {
                    for (uint k = 0; k < 3; ++k)
                    {
                        const auto index = indexBuffer[batch.offset + j * 3 + k];
                        const auto & vertex = vertexBuffer[index];

                        auto it = vertexHash.find(vertex);

                        uint vertexIndex;

                        if (vertexHash.end() == it)
                        {
                            vertexIndex = (uint)optimizedVertexBuffer.size();
                            vertexHash.insert(std::pair(vertex, vertexIndex));
                            optimizedVertexBuffer.push_back(vertex);
                        }
                        else
                        {
                            vertexIndex = it->second;
                        }

                        optimizedIndexBuffer.push_back(vertexIndex);
                    };
                }
            }

            _data.indices = std::move(optimizedIndexBuffer);
            _data.vertices = std::move(optimizedVertexBuffer);
        }      
        else
        {
            _data.indices = std::move(indexBuffer);
            _data.vertices = std::move(vertexBuffer);
        }
        
        _data.batches = std::move(batches);
        _data.materials = std::move(materials);
        _data.aabb = std::move(aabb);

        VG_SAFE_DELETE(triangleIndexTmp);

        VG_DEBUGPRINT(" %.2f ms\n", Timer::getEnlapsedTime(start, Timer::getTick()));
        return true;
    }
}