#include "renderer/Precomp.h"
#include "UFBXImporter.h"
#include "core/Timer/Timer.h"
#include "renderer/Importer/SceneImporterData.h"
#include "core/File/File.h"
#include "ufbx/ufbx.c"
#include "core/string/string.h"

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
            for (uint i = 0; i < meshCount; ++i)
            {
                MeshImporterData meshImporterData;
                if (loadFBXMesh(scene, scene->meshes[i], meshImporterData, scene->settings.unit_meters))
                    _data.meshes.push_back(meshImporterData);
            }

            const auto animCount = (uint)scene->anim_stacks.count;
            for (uint i = 0; i < animCount; ++i)
            {
                AnimImporterData animImporterData;
                if (loadFBXAnim(scene, scene->anim_stacks[i], animImporterData))
                    _data.anims.push_back(animImporterData);
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
    float4x4 UFBXMatrixToFloat4x3(const ufbx_matrix & _m)
    {
        return float4x4
        (
            (float)_m.m00, (float)_m.m01, (float)_m.m02, (float)_m.m03,
            (float)_m.m10, (float)_m.m11, (float)_m.m12, (float)_m.m13,
            (float)_m.m20, (float)_m.m21, (float)_m.m22, (float)_m.m23,
                     0.0f,          0.0f,          0.0f,          1.0f
        );
    }

    //--------------------------------------------------------------------------------------
    float3 UFBXVec3ToFloat3(ufbx_vec3 v)
    { 
        return float3((float)v.x, (float)v.y, (float)v.z);
    }

    //--------------------------------------------------------------------------------------
    core::quaternion UFBXQuatToQuat(ufbx_quat q)
    { 
        return core::quaternion((float)q.x, (float)q.y, (float)q.z, (float)q.w);
    }

    //--------------------------------------------------------------------------------------
    bool UFBXImporter::loadFBXMesh(const ufbx_scene * _UFBXScene, const ufbx_mesh * _UFbxMesh, MeshImporterData & _data, double _scale)
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

        ufbx_skin_deformer * UFbxSkin = nullptr;
        bool skinned = false;

        vector<SkinVertex<4/*MAXBONESPERVERTEX*/>> meshSkinVertices;

        if (_UFbxMesh->skin_deformers.count > 0)
        {
            _data.nodes.reserve(_UFBXScene->nodes.count);
            for (uint i = 0; i < _UFBXScene->nodes.count; i++)
            {
                const ufbx_node * UFBXNode = _UFBXScene->nodes.data[i];
                MeshImporterNode & node = _data.nodes.push_empty();

                //VG_DEBUGPRINT("Skin Node %u : %s\n", i, UFBXNode->name.data);

                node.name = UFBXNode->name.data;
                node.parent_index = UFBXNode->parent ? UFBXNode->parent->typed_id : -1;

                node.node_to_parent = UFBXMatrixToFloat4x3(UFBXNode->node_to_parent);
                node.node_to_world = UFBXMatrixToFloat4x3(UFBXNode->node_to_world);
                node.geometry_to_node = UFBXMatrixToFloat4x3(UFBXNode->geometry_to_node);
                node.geometry_to_world = UFBXMatrixToFloat4x3(UFBXNode->geometry_to_world);
                node.normal_to_world = UFBXMatrixToFloat4x3(ufbx_matrix_for_normals(&UFBXNode->geometry_to_world));

                // Store original TRS
                ufbx_transform transform = ufbx_evaluate_transform(&_UFBXScene->anim_stacks[0]->anim, UFBXNode, 0.0f);

                node.rot = UFBXQuatToQuat(transform.rotation);
                node.pos = UFBXVec3ToFloat3(transform.translation);
                node.scale = UFBXVec3ToFloat3(transform.scale);
            }

            UFbxSkin = _UFbxMesh->skin_deformers[0];
            skinned = (nullptr != UFbxSkin);
            _data.maxBonesCountPerVertex = 4;

            uint boneCount = 0;
            for (uint i = 0; i < UFbxSkin->clusters.count; i++)
            {
                ufbx_skin_cluster * cluster = UFbxSkin->clusters.data[i];

                _data.bonesIndices.push_back(cluster->bone_node->typed_id);
                _data.bonesMatrices.push_back(UFBXMatrixToFloat4x3(cluster->geometry_to_bone));

                boneCount++;
            }

            // TODO : template by max bone count
            VG_ASSERT(_data.maxBonesCountPerVertex == 4);
            constexpr uint MAXBONESPERVERTEX = 4; // _data.maxBonesCountPerVertex;

            meshSkinVertices.reserve(totalTriangleCount * 3);

            // Precalculate skinned vertex bones/weights for each vertex
            for (uint i = 0; i < _UFbxMesh->num_vertices; i++)
            {
                size_t weightIndex = 0;
                float weightTotal = 0.0f;

                float weights[MAXBONESPERVERTEX] = { 0.0f };
                u8 clusters[MAXBONESPERVERTEX] = { 0 };

                // `ufbx_skin_vertex` contains the offset and number of weights that deform the vertex in a descending weight order
                ufbx_skin_vertex vertex_weights = UFbxSkin->vertices.data[i];
                const uint weightCount = min(vertex_weights.num_weights, MAXBONESPERVERTEX);
                for (uint j = 0; j < weightCount; j++)
                {
                    ufbx_skin_weight weight = UFbxSkin->weights.data[vertex_weights.weight_begin + j];

                    weightTotal += (float)weight.weight;
                    clusters[weightIndex] = (uint8_t)weight.cluster_index;
                    weights[weightIndex] = (float)weight.weight;
                    weightIndex++;
                }

                SkinVertex<MAXBONESPERVERTEX> & skinVertex = meshSkinVertices.push_empty();

                // Normalize weights
                if (weightTotal > 0.0f)
                {
                    uint32_t quantized_sum = 0;
                    for (uint j = 0; j < MAXBONESPERVERTEX; j++)
                    {
                        u8 quantized_weight = (u8)((float)weights[j] / weightTotal * 255.0f);
                        quantized_sum += quantized_weight;

                        skinVertex.bone_index[j] = clusters[j];
                        skinVertex.bone_weight[j] = quantized_weight;
                    }
                    skinVertex.bone_weight[0] += 255 - quantized_sum;
                }
                else
                {
                    for (uint j = 0; j < MAXBONESPERVERTEX; j++)
                    {
                        skinVertex.bone_index[j] = -1;
                        skinVertex.bone_weight[j] = 0;
                    }
                }
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

                for (uint t = 0; t < triangleCount; t++)
                {
                    for (uint v = 0; v < 3; ++v)
                    {
                        uint k = t * 3 + 2 - v;

                        uint index = triangleIndexTmp[k];

                        const auto pos = getUFBXAttribute(index, _UFbxMesh->vertex_position, float3(0, 0, 0));
                        const auto nrm = getUFBXAttribute(index, _UFbxMesh->vertex_normal, float3(1, 0, 0));
                        const auto bin = getUFBXAttribute(index, _UFbxMesh->vertex_bitangent, float3(0, 1, 0));
                        const auto tan = getUFBXAttribute(index, _UFbxMesh->vertex_tangent, float3(0, 0, 1));
                        const auto col = getUFBXAttribute(index, _UFbxMesh->vertex_color, float4(1, 1, 1, 1));
                        const auto uv0 = getUFBXAttribute(index, _UFbxMesh->vertex_uv, float2(0, 0));

                        MeshImporterVertex vertex;

                        vertex.pos = float4(pos.x * _scale, pos.y * _scale, pos.z * _scale, 1.0f);
                        vertex.nrm = float4(nrm.x, nrm.y, nrm.z, 1.0f);
                        vertex.bin = float4(bin.x, bin.y, bin.z, 1.0f);
                        vertex.tan = float4(tan.x, tan.y, tan.z, 1.0f);
                        vertex.color = float4(col.x, col.y, col.z, col.w);
                        vertex.uv[0] = float2(uv0.x, 1.0f - uv0.y);
                        vertex.uv[1] = float2(0, 1.0f - 0); // TODO: Support UV1+

                        // The skinning vertex stream is pre-calculated above so we just need to copy the right one by the vertex index.
                        if (skinned)
                        {
                            auto & skinVertex = meshSkinVertices[_UFbxMesh->vertex_indices.data[index]];

                            for (uint l = 0; l < 4; ++l)
                            {
                                vertex.indices[l] = skinVertex.bone_index[l];
                                vertex.weights[l] = skinVertex.bone_weight[l];
                            }
                        }

                        vertexBuffer.push_back(vertex);
                        indexBuffer.push_back(vertexIndex);

                        aabb.grow(vertex.pos.xyz);

                        vertexIndex++;
                    }
                }

                batchIndexCount += (uint)triangleCount * 3;
            }

            Batch batch;
                  batch.setName(UFbxMeshMat->material ? UFbxMeshMat->material->name.data : fmt::sprintf("Material %u", i).c_str());
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

                    matImportData.m_name = UFbxMeshMat->material->name.data;

                    const auto & albedo = UFbxMeshMat->material->pbr.maps[UFBX_MATERIAL_PBR_BASE_COLOR];
                    const auto & normal = UFbxMeshMat->material->pbr.maps[UFBX_MATERIAL_PBR_NORMAL_MAP];

                    matImportData.m_texturePath[(int)MaterialTextureType::Albedo] = io::cleanPath(albedo.texture ? albedo.texture->absolute_filename.data : "");
                    matImportData.m_texturePath[(int)MaterialTextureType::Albedo] = io::getRelativePath(matImportData.m_texturePath[(int)MaterialTextureType::Albedo]);

                    matImportData.m_texturePath[(int)MaterialTextureType::Normal] = io::cleanPath(normal.texture ? normal.texture->absolute_filename.data : "");
                    matImportData.m_texturePath[(int)MaterialTextureType::Normal] = io::getRelativePath(matImportData.m_texturePath[(int)MaterialTextureType::Normal]);

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

    //--------------------------------------------------------------------------------------
    bool UFBXImporter::loadFBXAnim(const ufbx_scene * _UFBXScene, const ufbx_anim_stack * _UFBXAnimStack, AnimImporterData & _animData)
    {
        const float target_framerate = 60.0f;
        const uint max_frames = 65535;

        // Sample the animation evenly at `target_framerate` if possible while limiting the maximum
        // number of frames to `max_frames` by potentially dropping FPS.
        float duration = (float)_UFBXAnimStack->time_end - (float)_UFBXAnimStack->time_begin;
        uint num_frames = clamp(uint(duration * target_framerate), (uint)2, (uint)max_frames);
        float framerate = (float)(num_frames - 1) / duration;

        _animData.name = _UFBXScene->metadata.filename.data;//_UFBXAnimStack->name.data;
        _animData.time_begin = (float)_UFBXAnimStack->time_begin;
        _animData.time_end = (float)_UFBXAnimStack->time_end;
        _animData.framerate = framerate;
        _animData.num_frames = num_frames;

        // Sample the animations of all nodes and blend channels in the stack
        for (size_t i = 0; i < _UFBXScene->nodes.count; i++)
        {
            ufbx_node * UFBXNode = _UFBXScene->nodes.data[i];
            //VG_DEBUGPRINT("Anim Node %u : %s (Bone:%s)\n", i, UFBXNode->name.data, UFBXNode->bone? "Yes" : "No");

            AnimNodeData animNodeData;

            if (loadFBXAnimNode(_UFBXAnimStack, UFBXNode, _animData, animNodeData))
                _animData.animNodes.push_back(animNodeData);
        }

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool UFBXImporter::loadFBXAnimNode(const ufbx_anim_stack * _UFBXAnimStack, const ufbx_node * _UFBXNode, const AnimImporterData & _animData, AnimNodeData & _animNodeData)
    {
        _animNodeData.name = _UFBXNode->name.data;

        _animNodeData.rot.reserve(_animData.num_frames);
        _animNodeData.pos.reserve(_animData.num_frames);
        _animNodeData.scale.reserve(_animData.num_frames);

        bool const_rot = true, const_pos = true, const_scale = true;

        // Sample the node's transform evenly for animation duration
        for (size_t i = 0; i < _animData.num_frames; i++)
        {
            double time = _UFBXAnimStack->time_begin + (double)i / _animData.framerate;
        
            ufbx_transform transform = ufbx_evaluate_transform(&_UFBXAnimStack->anim, _UFBXNode, time);
        
            _animNodeData.rot.push_back(UFBXQuatToQuat(transform.rotation));
            _animNodeData.pos.push_back(UFBXVec3ToFloat3(transform.translation));
            _animNodeData.scale.push_back(UFBXVec3ToFloat3(transform.scale));
        
            if (i > 0)
            {
                // Flip the quaternion if necessary
                if (dot(_animNodeData.rot[i], _animNodeData.rot[i - 1]).x < 0.0f)
                    _animNodeData.rot[i] = -_animNodeData.rot[i];

                // Keep track of constant channels
                if (any(_animNodeData.rot[i] != _animNodeData.rot[i - 1]))
                    const_rot = false;

                if (any(_animNodeData.pos[i] != _animNodeData.pos[i - 1]))
                    const_pos = false;

                if (any(_animNodeData.scale[i] != _animNodeData.scale[i - 1]))
                    const_scale = false;
            }
        }

        if (const_rot) 
        { 
            _animNodeData.const_rot = _animNodeData.rot[0];
            _animNodeData.rot.clear();
        }

        if (const_pos)
        {
            _animNodeData.const_pos = _animNodeData.pos[0];
            _animNodeData.pos.clear();
        }

        if (const_scale)
        {
            _animNodeData.const_scale = _animNodeData.scale[0];
            _animNodeData.scale.clear();
        }

        return true;
    }
}