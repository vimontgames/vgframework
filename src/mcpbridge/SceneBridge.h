#pragma once

#include "mcpbridge/Json.h"
#include <string>
#include <vector>

namespace vg::engine  { class IEngine; }
namespace vg::core    { class IGameObject; class IBaseScene; class IResource; }

namespace vg::mcpbridge
{
    // Decomposed transform used on the wire (see docs/data-contract.md).
    // Rotation is a unit quaternion; identity = (0,0,0,1). Position in engine units.
    struct Transform
    {
        double px = 0.0, py = 0.0, pz = 0.0;
        double qx = 0.0, qy = 0.0, qz = 0.0, qw = 1.0;
        double sx = 1.0, sy = 1.0, sz = 1.0;
    };

    //--------------------------------------------------------------------------------------
    // Stateless helpers around the active scene. All lookups go through the global
    // UID registry (core::IFactory::FindByUID) so ids are the same the editor shows.
    //--------------------------------------------------------------------------------------
    class SceneBridge
    {
    public:
        explicit SceneBridge(engine::IEngine * _engine) : m_engine(_engine) {}

        // Executes one command object { "tool": "...", "args": { ... } } and returns
        // its result payload (never throws; errors are returned as { "error": {...} }).
        Json execute(const std::string & _tool, const Json & _args);

    private:
        Json listObjects   (const Json & _args);
        Json getTransform  (const Json & _args);
        Json setTransform  (const Json & _args);
        Json spawnObject   (const Json & _args);
        Json spawnObjects  (const Json & _args);
        Json deleteObject  (const Json & _args);
        Json createGroup   (const Json & _args);
        Json createScene   (const Json & _args);
        Json createWorld   (const Json & _args);
        Json saveWorld     (const Json & _args);
        Json createPrefabs (const Json & _args);
        Json instantiatePrefab(const Json & _args);
        Json setMaterial   (const Json & _args);
        Json setEnabled    (const Json & _args);
        Json selectObject  (const Json & _args);

        // One FBX waiting for its model. Importing is asynchronous, so the first call registers
        // the mesh and later calls finish the prefab once the model actually resolves.
        struct StagedMesh
        {
            std::string         fbx;        // data/Meshes/Giraphon/Giraphon.fbx
            std::string         name;       // Giraphon
            core::IGameObject * object = nullptr;
        };
        std::vector<StagedMesh> m_staged;

        // Writes a Default-shader .mat. A DefaultMaterialData already defaults to
        // Opaque / Back / UV0 / tiling 1 / offset 0, so only the maps are assigned here;
        // an empty path leaves that slot unassigned.
        static bool writeMaterial(const std::string & _matPath, const std::string & _name,
                                  const std::string & _albedo, const std::string & _normal,
                                  const std::string & _pbr);

        // Textures sit next to the FBX and are named <MaterialName>_<Suffix>.<ext>.
        static std::string findTexture(const std::string & _folder, const std::string & _stem,
                                       const char * const * _suffixes);

        // Reads the batch names off the loaded model, writes the .mat files and the .prefab.
        // Returns an empty Json while the model is still loading.
        Json finishPrefab(StagedMesh & _mesh, const std::string & _prefabFolder,
                          const std::string & _materialFolder, bool _overwrite, Json & _warnings);

        // All Scene-type scenes of the main world.
        //
        // Deliberately NOT IWorld::GetActiveScene(): World::AddScene tests `nullptr == m_activeScene`
        // on an array (World.cpp:171, member declared World.h:95), which decays to a never-null
        // pointer, so the active scene is never set when a world is loaded. Enumerating is both a
        // workaround for that upstream bug and more useful (multi-scene worlds).
        std::vector<const core::IBaseScene *> scenes() const;

        core::IGameObject * resolve(const std::string & _id, Json & _errorOut) const;

        // How much of an object is put on the wire. Compact is id/name/enabled only:
        // a full listing of a few hundred objects blows past the MCP token budget.
        enum class Detail { Compact, Full };

        static Json         error(const char * _code, const std::string & _msg, const std::string & _id = std::string());
        static Json         transformToJson(const Transform & _t);
        static Json         objectToJson(core::IGameObject * _go, Detail _detail);

        // Clones _source under _parent and applies _placement ({ name?, position?, rotation?, scale? }).
        // Returns nullptr and fills _errorOut on failure.
        core::IGameObject * spawnOne(core::IGameObject * _source, core::IGameObject * _parent, const Json & _placement, Json & _errorOut) const;

        // Applies the position / rotation / scale members of _src onto _t (each optional).
        static void         applyPlacement(const Json & _src, Transform & _t);

        // A real scene holds thousands of objects, so listing is always bounded:
        // either browse one level at a time, or search the whole tree by name.
        struct GatherContext
        {
            std::string nameFilter;         // lowercase substring; empty = browse mode
            int         maxDepth = 1;       // browse mode only; 0 = unlimited
            int         limit = 200;        // max objects reported
            int         matched = 0;        // total matches, even beyond limit
            Detail      detail = Detail::Compact;
            Json *      out = nullptr;
        };
        void                gather(core::IGameObject * _go, GatherContext & _ctx, const std::string & _path, int _depth) const;

        static Transform    readTransform(core::IGameObject * _go);
        static void         writeTransform(core::IGameObject * _go, const Transform & _t);

        engine::IEngine * m_engine = nullptr;
    };
}
