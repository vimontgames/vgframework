#include "graphics/renderer/Precomp.h"
#include "FBXImporter.h"
#include "graphics/renderer/importer/SceneImporterData.h"
#include "core/Timer/Timer.h"
#include "core/Math/Math.h"

#define FBXSDK_SHARED
#include <fbxsdk.h>

using namespace fbxsdk;
using namespace vg::core;

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    FBXImporter::FBXImporter()
    {
        m_fbxManager = FbxManager::Create();
        VG_ASSERT(nullptr != m_fbxManager);
        //VG_DEBUGPRINT("Autodesk FBX SDK version %s\n", m_fbxManager->GetVersion());

        FbxIOSettings* ios = FbxIOSettings::Create(m_fbxManager, IOSROOT);
        m_fbxManager->SetIOSettings(ios);

        //FbxString lPath = FbxGetApplicationDirectory();
        //m_fbxManager->LoadPluginsDirectory(lPath.Buffer());
    }

    //--------------------------------------------------------------------------------------
    FBXImporter::~FBXImporter()
    {
        //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
        if (m_fbxManager) 
            m_fbxManager->Destroy();
    }

    //--------------------------------------------------------------------------------------
    bool FBXImporter::importFBX(const core::string & _path, SceneImporterData & _data)
    {
        VG_ASSERT(m_fbxManager);
        if (!m_fbxManager)
            return false;

        FbxScene * fbxScene = FbxScene::Create(m_fbxManager, "FBXScene");
        VG_ASSERT(fbxScene);
        if (!fbxScene)
            return false;

        int fbxSDKMajor, fbxSDKMinor, fbxSDKRev;
        FbxManager::GetFileFormatVersion(fbxSDKMajor, fbxSDKMinor, fbxSDKRev);

        FbxImporter * fbxImporter = FbxImporter::Create(m_fbxManager, "FBXImporter");
        VG_ASSERT(fbxImporter);
        if (!fbxImporter)
            return false;

        FbxIOSettings * ios = m_fbxManager->GetIOSettings();

        const auto start = Timer::getTick();
        VG_DEBUGPRINT("[FBXImporter] Opening FBX file \"%s\" ...", _path.c_str());

        int fbxFileMajor, fbxFileMinor, fbxFileRev;
        const bool fbxImporterStatus = fbxImporter->Initialize(_path.c_str(), -1, ios);
        fbxImporter->GetFileVersion(fbxFileMajor, fbxFileMinor, fbxFileRev);

        if (!fbxImporterStatus)
        {
            FbxString error = fbxImporter->GetStatus().GetErrorString();
            VG_DEBUGPRINT("Call to FbxImporter::Initialize() failed.\n");
            VG_DEBUGPRINT("Error returned: %s\n\n", error.Buffer());

            if (fbxImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
            {
                VG_DEBUGPRINT("FBX file format version for this FBX SDK is %d.%d.%d\n", fbxSDKMajor, fbxSDKMinor, fbxSDKRev);
                VG_DEBUGPRINT("FBX file format version for file '%s' is %d.%d.%d\n\n", _path.c_str(), fbxFileMajor, fbxFileMinor, fbxFileRev);
            }

            return false;
        }

        if (!fbxImporter->IsFBX())
            return false;

        ios->SetBoolProp(IMP_FBX_MATERIAL, true);
        ios->SetBoolProp(IMP_FBX_TEXTURE, true);
        ios->SetBoolProp(IMP_FBX_LINK, true);
        ios->SetBoolProp(IMP_FBX_SHAPE, true);
        ios->SetBoolProp(IMP_FBX_GOBO, true);
        ios->SetBoolProp(IMP_FBX_ANIMATION, true);
        ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

        bool success = false;

        if (fbxImporter->Import(fbxScene))
        {
            VG_DEBUGPRINT(" %.2f ms\n", Timer::getEnlapsedTime(start, Timer::getTick()));

            success = true;

            FbxStatus status;
            FbxArray<FbxString*> details;
            FbxSceneCheckUtility sceneCheck(fbxScene, &status, &details);

            const bool warningOrError = (!sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData) && details.GetCount() > 0) || (FbxStatus::eSuccess != fbxImporter->GetStatus().GetCode());

            if (warningOrError)
            {
                VG_DEBUGPRINT("FBX importer error(s):\n");

                if (details.GetCount() > 0)
                {
                    for (int i = 0; i < details.GetCount(); i++)
                        VG_DEBUGPRINT("%s\n", details[i]->Buffer());

                    FbxArrayDelete<FbxString*>(details);
                }

                if (FbxStatus::eSuccess != fbxImporter->GetStatus().GetCode())
                {
                    VG_DEBUGPRINT("FBX importer warning(s):\n");
                    VG_DEBUGPRINT(fbxImporter->GetStatus().GetErrorString());
                }
            }

            // Convert Coordinate systems and units if needed
            const FbxAxisSystem fileAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
            const FbxAxisSystem axisSystem(FbxAxisSystem::eZAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded);
            if (fileAxisSystem != axisSystem)
                axisSystem.ConvertScene(fbxScene);

            const FbxSystemUnit SceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
            if (1.0f != SceneSystemUnit.GetScaleFactor())
                FbxSystemUnit::m.ConvertScene(fbxScene);

            // Convert to triangle mesh
            FbxGeometryConverter geoConverter(m_fbxManager);
            const bool converted = geoConverter.Triangulate(fbxScene, true);
            VG_ASSERT(converted);

            const int nodeCount = fbxScene->GetSrcObjectCount<FbxNode>();
            for (int iNode = 0; iNode < nodeCount; iNode++)
            {
                FbxNode * node = fbxScene->GetSrcObject<FbxNode>(iNode);
                if (node->GetGeometry())
                {
                    FbxNodeAttribute * fbxNodeAttribute = node->GetNodeAttribute();

                    switch (fbxNodeAttribute->GetAttributeType())
                    {
                        default:
                            VG_ASSERT_NOT_IMPLEMENTED();
                            break;

                        case FbxNodeAttribute::eMesh:
                        {
                            MeshImporterData meshImporterData;

                            if (loadFBXMesh(node, static_cast<FbxMesh*>(fbxNodeAttribute), meshImporterData))
                                _data.meshes.push_back(meshImporterData);                         
                        }
                        break;
                    }
                }
            }
        }

        fbxScene->Destroy();
        fbxScene = nullptr;

        return success;
    }

    //--------------------------------------------------------------------------------------
    template <typename V> V store(const double * _pData);

    template <> float1 store<float1>(const double * _pData) { return float1(_pData[0]); }
    template <> float2 store<float2>(const double * _pData) { return float2(_pData[0], _pData[1]); }
    template <> float3 store<float3>(const double * _pData) { return float3(_pData[0], _pData[1], _pData[2]); }
    template <> float4 store<float4>(const double * _pData) { return float4(_pData[0], _pData[1], _pData[2], _pData[3]); }

    template <typename V> uint countOf();
    template <> uint countOf<float1>() { return 1; }
    template <> uint countOf<float2>() { return 2; }
    template <> uint countOf<float3>() { return 3; }
    template <> uint countOf<float4>() { return 4; }

    //--------------------------------------------------------------------------------------
    template <class T, class V> bool FBXImporter::importFBXLayerElement(vector<V> & _result, const FbxMesh * _fbxMesh, T * _layerElement, uint _triangleCount, V const & _default)
    {
        _result.resize(_triangleCount * 3);

        if (nullptr == _layerElement)
        {
            for (uint i = 0; i < _result.size(); ++i)
                _result[i] = _default;
            return false;
        }

        const double * data = (double*)(_layerElement->mDirectArray->GetLocked(FbxLayerElementArray::eReadLock));
        const int * indices = (int*)(_layerElement->mIndexArray->GetLocked(FbxLayerElementArray::eReadLock));

        FbxLayerElement::EMappingMode mappingMode = _layerElement->GetMappingMode();
        FbxLayerElement::EReferenceMode referenceMode = _layerElement->GetReferenceMode();

        const uint count = countOf<V>();

        switch (mappingMode)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(mappingMode);
                break;

            case FbxLayerElement::eByControlPoint:
                if (FbxLayerElement::eDirect == referenceMode)
                {
                    for (uint p = 0; p < _triangleCount; p++)
                    {
                        const uint index[3] =
                        {
                            (uint)_fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[p].mIndex + 0],
                            (uint)_fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[p].mIndex + 1],
                            (uint)_fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[p].mIndex + 2]
                        };

                        for (uint i = 0; i < countof(index); ++i)
                        {
                            const double * pData = (double*)&data[(index[i])*count];
                            _result[p * 3 + i] = store<V>(pData);
                        }
                    }
                }
                else
                {
                    VG_ASSERT(FbxLayerElement::eIndexToDirect == referenceMode);
                    for (u32 p = 0; p < _triangleCount; p++)
                    {
                        const uint index[3] =
                        {
                            (uint)indices[_fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[p].mIndex + 0]],
                            (uint)indices[_fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[p].mIndex + 1]],
                            (uint)indices[_fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[p].mIndex + 2]]
                        };

                        for (uint i = 0; i < countof(index); ++i)
                        {
                            const double * pData = (double*)&data[(index[i])*count];
                            _result[p*3 + i] = store<V>(pData);
                        }
                    }
                }
                break;

            case FbxLayerElement::eByPolygonVertex:
                if (FbxLayerElement::eDirect == referenceMode)
                {
                    for (u32 p = 0; p < _triangleCount; p++)
                    {
                        const uint index[3] =
                        {
                            (uint)p * 3 + 0,
                            (uint)p * 3 + 1,
                            (uint)p * 3 + 2
                        };

                        for (uint i = 0; i < countof(index); ++i)
                        {
                            const double * pData = (double*)&data[(index[i])*count];
                            _result[p*3 + i] = store<V>(pData);
                        }
                    }
                }
                else
                {
                    VG_ASSERT(FbxLayerElement::eIndexToDirect == referenceMode);
                    for (u32 p = 0; p < _triangleCount; p++)
                    {
                        const uint index[3] =
                        {
                            (uint)indices[p * 3 + 0],
                            (uint)indices[p * 3 + 1],
                            (uint)indices[p * 3 + 2]
                        };

                        for (uint i = 0; i < countof(index); ++i)
                        {
                            const double * pData = (double*)&data[(index[i])*count];
                            _result[p*3 + i] = store<V>(pData);
                        }
                    }
                }
                break;
        }

        _layerElement->mDirectArray->Release((void**)(&data));
        _layerElement->mIndexArray->Release((void**)(&indices));

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool FBXImporter::loadFBXMesh(FbxNode * _fbxNode, FbxMesh * _fbxMesh, MeshImporterData & _data)
    {
        const auto start = Timer::getTick();
        VG_DEBUGPRINT("[FBXImporter] Importing FBX Mesh \"%s\" ...", _fbxNode->GetName());

        VG_ASSERT(3 == _fbxMesh->GetPolygonSize(0));

        const uint controlPointCount = _fbxMesh->GetControlPointsCount();
        const uint triangleCount = (uint)_fbxMesh->GetPolygonCount();

        vector<float4> positions;
                       positions.resize(controlPointCount);

        // import positions
        const FbxVector4 * points = _fbxMesh->GetControlPoints();

        for (u32 p = 0; p < controlPointCount; p++)
        {
            const double * pPosD = (double*)&points[p];
            positions[p] = float4((float)pPosD[0], (float)pPosD[1], (float)pPosD[2], (float)pPosD[3]);
        }

        // get layers
        vector<FbxLayer*> fbxLayers;
        uint i = 0;
        while (nullptr != _fbxMesh->GetLayer(i))
            fbxLayers.push_back(_fbxMesh->GetLayer(i++));

        // import normals
        vector<float4> normals;
        const bool hasNormals = importFBXLayerElement<FbxLayerElementNormal, float4>(normals, _fbxMesh, fbxLayers[0] ? fbxLayers[0]->GetNormals() : nullptr, triangleCount, float4(0, 0, 0, 0));

        // import UVs
        vector<float2> uv[2];
        const bool hasUV0 = importFBXLayerElement<FbxLayerElementUV, float2>(uv[0], _fbxMesh, fbxLayers.size() > 0 ? fbxLayers[0]->GetUVs(FbxLayerElement::eTextureDiffuse) : nullptr, triangleCount, float2(0, 0));
        const bool hasUV1 = importFBXLayerElement<FbxLayerElementUV, float2>(uv[1], _fbxMesh, fbxLayers.size() > 1 ? fbxLayers[1]->GetUVs(FbxLayerElement::eTextureDiffuse) : nullptr, triangleCount, float2(0, 0));

        FbxLayerElementMaterial * matLayer = fbxLayers.size() > 0 ? fbxLayers[0]->GetMaterials() : nullptr;

        uint maxMatID = 0;
        // first pass to count how many matID
        if (nullptr != matLayer)
        {
            for (uint t = 0; t < triangleCount; ++t)
            {
                // get material ID index
                const uint matID = matLayer->GetIndexArray().GetAt(t);
                maxMatID = max(matID, maxMatID);
            }
        }
        const uint maxMaterialCount = maxMatID + 1;

        vector<vector<u32>> indexBuffers;
                            indexBuffers.resize(maxMaterialCount);

        for (uint b = 0; b < maxMaterialCount; ++b)
            indexBuffers[b].reserve(triangleCount * 3);

        // load triangles
        vector<MeshImporterVertex> vertexBuffer;
                                   vertexBuffer.reserve(triangleCount * 3); // worst case

        for (uint t = 0; t < triangleCount; ++t)
        {
            // get material ID index
            const uint matID = matLayer ? matLayer->GetIndexArray().GetAt(t) : 0;
            auto & indexBuffer = indexBuffers[matID];

            for (uint v = 0; v < 3; ++v)
            {
                MeshImporterVertex vertex;

                int test = _fbxMesh->GetPolygonVertex(t, v);

                const i32 index = _fbxMesh->mPolygonVertices[_fbxMesh->mPolygons[t].mIndex + v];

                vertex.pos = positions[index];

                const uint vertexID = t * 3 + v;

                vertex.nrm = normals[vertexID];
                vertex.uv[0] = uv[0][vertexID];
                vertex.uv[1] = uv[1][vertexID];

                indexBuffer.push_back(vertexID);
                vertexBuffer.push_back(vertex);
            }
        }

        // flatten index buffer
        vector<u32> flatIndexBuffer;
                    flatIndexBuffer.reserve(triangleCount * 3);

        vector<Batch> batches;
                      batches.reserve(maxMaterialCount);

        u32 offset = 0;

        for (uint b = 0; b < maxMaterialCount; ++b)
        {
            const auto & indexBuffer = indexBuffers[b];
            const uint indexCount = (uint)indexBuffer.size();
            if (indexCount > 0)
            {
                for (uint i = 0; i < indexCount; ++i)
                {
                    u32 index = indexBuffer[i];
                    flatIndexBuffer.push_back(index);
                }

                Batch batch;
                      batch.count = indexCount;
                      batch.offset = offset;

                batches.push_back(batch);

                offset += indexCount;
            }
        }

        _data.batches = std::move(batches);
        _data.indices = std::move(flatIndexBuffer);
        _data.vertices = std::move(vertexBuffer);
 
        VG_DEBUGPRINT(" %.2f ms\n", _fbxNode->GetName(), Timer::getEnlapsedTime(start, Timer::getTick()));

        return true;
    }
}