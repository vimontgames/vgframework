#include "mcpbridge/Precomp.h"
#include "mcpbridge/SceneBridge.h"

#include "core/Kernel.h"
#include "core/IFactory.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/IGameObject.h"
#include "core/IClassDesc.h"
#include "core/IProperty.h"
#include "core/IResource.h"
#include "core/ISelection.h"
#include "core/Misc/AABB/AABB.h"
#include "core/File/File.h"
#include "core/IResourceList.h"
#include "core/IComponent.h"
#include "engine/IEngine.h"
#include "engine/IWorldResource.h"
#include "renderer/IGraphicModel.h"   // GetBatchNames(): the FBX material names, header-only (pure virtual)

#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstdlib>

using namespace vg::core;

namespace vg::mcpbridge
{
    //--------------------------------------------------------------------------------------
    // Raw 4x4 helpers. Layout matches the engine / scene XML: rows I, J, K, T.
    //   m[0..3] = I (local X axis * scale.x), m[4..7] = J, m[8..11] = K, m[12..15] = T.
    // Rotation is taken as the matrix whose rows are the normalized basis vectors.
    //--------------------------------------------------------------------------------------
    namespace
    {
        double len3(const float * v)
        {
            return std::sqrt((double)v[0] * v[0] + (double)v[1] * v[1] + (double)v[2] * v[2]);
        }

        Transform decompose(const float m[16])
        {
            Transform t;
            t.px = m[12]; t.py = m[13]; t.pz = m[14];

            const double lx = len3(&m[0]);
            const double ly = len3(&m[4]);
            const double lz = len3(&m[8]);
            t.sx = lx; t.sy = ly; t.sz = lz;

            // Normalized rotation basis (rows). Guard against zero scale.
            const double r00 = lx > 1e-8 ? m[0] / lx : 1.0, r01 = lx > 1e-8 ? m[1] / lx : 0.0, r02 = lx > 1e-8 ? m[2] / lx : 0.0;
            const double r10 = ly > 1e-8 ? m[4] / ly : 0.0, r11 = ly > 1e-8 ? m[5] / ly : 1.0, r12 = ly > 1e-8 ? m[6] / ly : 0.0;
            const double r20 = lz > 1e-8 ? m[8] / lz : 0.0, r21 = lz > 1e-8 ? m[9] / lz : 0.0, r22 = lz > 1e-8 ? m[10] / lz : 1.0;

            // Rotation matrix -> quaternion (Shepperd's method).
            //
            // ROWS ARE THE LOCAL AXES here (row0 = rotated X, ...), which is the transpose of the
            // usual column-vector form the textbook formula assumes. The antisymmetric terms are
            // therefore taken as (r_ij - r_ji) instead of (r_ji - r_ij); the symmetric ones are
            // unaffected. Must stay the exact inverse of recompose() below — getting this backwards
            // silently negates the rotation axis on a get/set round trip.
            const double trace = r00 + r11 + r22;
            if (trace > 0.0)
            {
                double s = std::sqrt(trace + 1.0) * 2.0;
                t.qw = 0.25 * s;
                t.qx = (r12 - r21) / s;
                t.qy = (r20 - r02) / s;
                t.qz = (r01 - r10) / s;
            }
            else if (r00 > r11 && r00 > r22)
            {
                double s = std::sqrt(1.0 + r00 - r11 - r22) * 2.0;
                t.qw = (r12 - r21) / s;
                t.qx = 0.25 * s;
                t.qy = (r01 + r10) / s;
                t.qz = (r02 + r20) / s;
            }
            else if (r11 > r22)
            {
                double s = std::sqrt(1.0 + r11 - r00 - r22) * 2.0;
                t.qw = (r20 - r02) / s;
                t.qx = (r01 + r10) / s;
                t.qy = 0.25 * s;
                t.qz = (r12 + r21) / s;
            }
            else
            {
                double s = std::sqrt(1.0 + r22 - r00 - r11) * 2.0;
                t.qw = (r01 - r10) / s;
                t.qx = (r02 + r20) / s;
                t.qy = (r12 + r21) / s;
                t.qz = 0.25 * s;
            }

            // Normalize quaternion.
            double n = std::sqrt(t.qx * t.qx + t.qy * t.qy + t.qz * t.qz + t.qw * t.qw);
            if (n > 1e-12) { t.qx /= n; t.qy /= n; t.qz /= n; t.qw /= n; }
            else           { t.qx = t.qy = t.qz = 0.0; t.qw = 1.0; }
            return t;
        }

        void recompose(const Transform & t, float m[16])
        {
            double x = t.qx, y = t.qy, z = t.qz, w = t.qw;
            const double n = std::sqrt(x * x + y * y + z * z + w * w);
            if (n > 1e-12) { x /= n; y /= n; z /= n; w /= n; } else { x = y = z = 0.0; w = 1.0; }

            // Rotation matrix (rows-as-axes) from unit quaternion.
            const double r00 = 1.0 - 2.0 * (y * y + z * z);
            const double r01 = 2.0 * (x * y + z * w);
            const double r02 = 2.0 * (x * z - y * w);
            const double r10 = 2.0 * (x * y - z * w);
            const double r11 = 1.0 - 2.0 * (x * x + z * z);
            const double r12 = 2.0 * (y * z + x * w);
            const double r20 = 2.0 * (x * z + y * w);
            const double r21 = 2.0 * (y * z - x * w);
            const double r22 = 1.0 - 2.0 * (x * x + y * y);

            m[0]  = (float)(r00 * t.sx); m[1]  = (float)(r01 * t.sx); m[2]  = (float)(r02 * t.sx); m[3]  = 0.0f;
            m[4]  = (float)(r10 * t.sy); m[5]  = (float)(r11 * t.sy); m[6]  = (float)(r12 * t.sy); m[7]  = 0.0f;
            m[8]  = (float)(r20 * t.sz); m[9]  = (float)(r21 * t.sz); m[10] = (float)(r22 * t.sz); m[11] = 0.0f;
            m[12] = (float)t.px;         m[13] = (float)t.py;         m[14] = (float)t.pz;         m[15] = 1.0f;
        }

        bool finite3(double a, double b, double c)
        {
            return std::isfinite(a) && std::isfinite(b) && std::isfinite(c);
        }
    }

    //--------------------------------------------------------------------------------------
    Transform SceneBridge::readTransform(IGameObject * _go)
    {
        float m[16];
        hlslpp::store(m, _go->GetLocalMatrix());
        return decompose(m);
    }

    //--------------------------------------------------------------------------------------
    void SceneBridge::writeTransform(IGameObject * _go, const Transform & _t)
    {
        float m[16];
        recompose(_t, m);
        float4x4 mat;
        hlslpp::load(mat, m);
        _go->SetLocalMatrix(mat);
        _go->OnLocalMatrixChanged(false, true);

        // Mirror what the editor gizmo does (ImGuiView.hpp): notify so components
        // refresh their render instance and the object is flagged dirty-for-save.
        if (const IClassDesc * desc = _go->GetClassDesc())
        {
            if (const IProperty * prop = desc->GetPropertyByName("m_local", false))
                _go->OnPropertyChanged(_go, *prop, false);
        }
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::error(const char * _code, const std::string & _msg, const std::string & _id)
    {
        Json err = Json::makeObject();
        err.set("code", Json(_code));
        err.set("message", Json(_msg));
        if (!_id.empty())
            err.set("object_id", Json(_id));
        Json out = Json::makeObject();
        out.set("error", err);
        return out;
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::transformToJson(const Transform & _t)
    {
        Json pos = Json::makeObject();
        pos.set("x", Json(_t.px)); pos.set("y", Json(_t.py)); pos.set("z", Json(_t.pz));
        Json rot = Json::makeObject();
        rot.set("x", Json(_t.qx)); rot.set("y", Json(_t.qy)); rot.set("z", Json(_t.qz)); rot.set("w", Json(_t.qw));
        Json scl = Json::makeObject();
        scl.set("x", Json(_t.sx)); scl.set("y", Json(_t.sy)); scl.set("z", Json(_t.sz));
        Json tr = Json::makeObject();
        tr.set("position", pos);
        tr.set("rotation", rot);
        tr.set("scale", scl);
        return tr;
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::objectToJson(IGameObject * _go, Detail _detail)
    {
        Json o = Json::makeObject();
        o.set("object_id", Json(std::to_string(_go->GetUID(false))));
        o.set("name", Json(_go->GetName()));
        if (Detail::Full == _detail)
        {
            o.set("enabled", Json(_go->IsEnabledInHierarchy()));
            const Transform t = readTransform(_go);
            o.set("transform", transformToJson(t));

            // Un-scaled bounds in the object's own local space: reveals where the pivot sits
            // inside the mesh and the reference size, so no dimension has to be guessed.
            AABB aabb;
            if (_go->TryGetAABB(aabb))
            {
                float mn[3], mx[3];
                hlslpp::store(mn, aabb.m_min);
                hlslpp::store(mx, aabb.m_max);

                auto vec3 = [](double x, double y, double z)
                {
                    Json v = Json::makeObject();
                    v.set("x", Json(x)); v.set("y", Json(y)); v.set("z", Json(z));
                    return v;
                };

                Json box = Json::makeObject();
                box.set("min", vec3(mn[0], mn[1], mn[2]));
                box.set("max", vec3(mx[0], mx[1], mx[2]));
                box.set("size", vec3(mx[0] - mn[0], mx[1] - mn[1], mx[2] - mn[2]));
                o.set("bounds_local", box);

                // Footprint actually occupied in the parent space, i.e. bounds * scale.
                o.set("size_world", vec3((mx[0] - mn[0]) * t.sx, (mx[1] - mn[1]) * t.sy, (mx[2] - mn[2]) * t.sz));
            }
        }
        return o;
    }

    //--------------------------------------------------------------------------------------
    std::vector<const IBaseScene *> SceneBridge::scenes() const
    {
        std::vector<const IBaseScene *> out;

        if (!m_engine)
            return out;

        IWorld * world = m_engine->GetMainWorld();
        if (!world)
            return out;

        const uint count = world->GetSceneCount(BaseSceneType::Scene);
        for (uint i = 0; i < count; ++i)
        {
            if (const IBaseScene * scene = world->GetScene(i, BaseSceneType::Scene))
                out.push_back(scene);
        }
        return out;
    }

    //--------------------------------------------------------------------------------------
    IGameObject * SceneBridge::resolve(const std::string & _id, Json & _errorOut) const
    {
        if (_id.empty())
        {
            _errorOut = error("INVALID_VALUE", "missing object_id");
            return nullptr;
        }

        char * endp = nullptr;
        const unsigned long uid = strtoul(_id.c_str(), &endp, 10);
        if (endp == _id.c_str() || (endp && *endp != '\0'))
        {
            _errorOut = error("INVALID_VALUE", "object_id is not a numeric UID", _id);
            return nullptr;
        }

        IFactory * factory = Kernel::getFactory(false);
        if (!factory)
        {
            _errorOut = error("ENGINE_BUSY", "factory not available");
            return nullptr;
        }

        IObject * obj = factory->FindByUID((UID)uid);
        if (!obj)
        {
            _errorOut = error("OBJECT_NOT_FOUND", "no object with UID " + _id, _id);
            return nullptr;
        }

        IGameObject * go = dynamic_cast<IGameObject *>(obj);
        if (!go)
        {
            _errorOut = error("NOT_A_GAMEOBJECT", "UID " + _id + " is a " + obj->GetClassName(), _id);
            return nullptr;
        }
        return go;
    }

    //--------------------------------------------------------------------------------------
    void SceneBridge::gather(IGameObject * _go, GatherContext & _ctx, const std::string & _path, int _depth) const
    {
        const std::string name = _go->GetName();
        const std::string path = _path.empty() ? name : _path + "/" + name;

        const bool searching = !_ctx.nameFilter.empty();
        bool report = true;

        if (searching)
        {
            std::string lower = name;
            for (char & c : lower)
                c = (char)tolower((unsigned char)c);
            report = lower.find(_ctx.nameFilter) != std::string::npos;
        }

        if (report)
        {
            ++_ctx.matched;
            if ((int)_ctx.out->items().size() < _ctx.limit)
            {
                Json o = objectToJson(_go, _ctx.detail);
                o.set("path", Json(path));
                o.set("child_count", Json((int)_go->GetChildren().size()));
                _ctx.out->push(o);
            }
        }

        // Browsing stops at maxDepth; searching always walks the whole subtree.
        if (!searching && _ctx.maxDepth > 0 && _depth + 1 >= _ctx.maxDepth)
            return;

        for (IGameObject * child : _go->GetChildren())
            gather(child, _ctx, path, _depth + 1);
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::execute(const std::string & _tool, const Json & _args)
    {
        if (_tool == "list_objects")   return listObjects(_args);
        if (_tool == "get_transform")  return getTransform(_args);
        if (_tool == "set_transform")  return setTransform(_args);
        if (_tool == "spawn_object")   return spawnObject(_args);
        if (_tool == "spawn_objects")  return spawnObjects(_args);
        if (_tool == "delete_object")  return deleteObject(_args);
        if (_tool == "create_group")   return createGroup(_args);
        if (_tool == "create_scene")   return createScene(_args);
        if (_tool == "create_world")   return createWorld(_args);
        if (_tool == "save_world")     return saveWorld(_args);
        if (_tool == "create_prefabs") return createPrefabs(_args);
        if (_tool == "instantiate_prefab") return instantiatePrefab(_args);
        if (_tool == "set_material")   return setMaterial(_args);
        if (_tool == "set_enabled")    return setEnabled(_args);
        if (_tool == "select_object")  return selectObject(_args);
        return error("NOT_SUPPORTED", "unknown tool '" + _tool + "'");
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::listObjects(const Json & _args)
    {
        const auto sceneList = scenes();
        if (sceneList.empty())
            return error("NO_ACTIVE_SCENE", "the main world has no scene loaded");

        Json objects = Json::makeArray();

        GatherContext ctx;
        ctx.out = &objects;
        ctx.nameFilter = _args["name_contains"].asString();
        for (char & c : ctx.nameFilter)
            c = (char)tolower((unsigned char)c);
        ctx.maxDepth = (int)_args["max_depth"].asNumber(1.0);
        ctx.limit = (int)_args["limit"].asNumber(200.0);
        if (ctx.limit <= 0)
            ctx.limit = 200;
        ctx.detail = (_args["detail"].asString() == "full") ? Detail::Full : Detail::Compact;

        Json sceneNames = Json::makeArray();

        // Optional starting point: list below one object instead of the scene roots.
        if (_args.has("parent_id"))
        {
            Json err;
            IGameObject * parent = resolve(_args["parent_id"].asString(), err);
            if (!parent)
                return err;

            for (IGameObject * child : parent->GetChildren())
                gather(child, ctx, std::string(), 0);
        }
        else
        {
            for (const IBaseScene * scene : sceneList)
            {
                IGameObject * root = scene->GetRoot();

                // The root itself is never listed as an object (it is not something to move),
                // so report its id here: it is the only way to target an empty scene, whose
                // subtree shows nothing at all.
                Json s = Json::makeObject();
                s.set("name", Json(scene->GetName()));
                if (root)
                {
                    s.set("root_id", Json(std::to_string(root->GetUID(false))));
                    s.set("child_count", Json((int)root->GetChildren().size()));
                }
                sceneNames.push(s);

                // Paths are prefixed with the scene name so they stay unambiguous across scenes.
                if (root)
                {
                    for (IGameObject * child : root->GetChildren())
                        gather(child, ctx, scene->GetName(), 0);
                }
            }
        }

        Json out = Json::makeObject();
        out.set("scenes", sceneNames);
        out.set("objects", objects);
        out.set("returned", Json((int)objects.items().size()));
        out.set("matched", Json(ctx.matched));
        out.set("truncated", Json(ctx.matched > (int)objects.items().size()));
        out.set("mode", Json(ctx.nameFilter.empty() ? "browse" : "search"));
        out.set("detail", Json(Detail::Full == ctx.detail ? "full" : "compact"));
        return out;
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::getTransform(const Json & _args)
    {
        Json err;
        IGameObject * go = resolve(_args["object_id"].asString(), err);
        if (!go)
            return err;
        return objectToJson(go, Detail::Full);
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::setTransform(const Json & _args)
    {
        Json err;
        IGameObject * go = resolve(_args["object_id"].asString(), err);
        if (!go)
            return err;

        Transform t = readTransform(go);
        applyPlacement(_args, t);

        if (!finite3(t.px, t.py, t.pz) || !finite3(t.sx, t.sy, t.sz) ||
            !std::isfinite(t.qx) || !std::isfinite(t.qy) || !std::isfinite(t.qz) || !std::isfinite(t.qw))
            return error("INVALID_VALUE", "non-finite transform value", _args["object_id"].asString());

        if (std::fabs(t.sx) < 1e-6 || std::fabs(t.sy) < 1e-6 || std::fabs(t.sz) < 1e-6)
            return error("INVALID_VALUE", "scale component too close to zero", _args["object_id"].asString());

        writeTransform(go, t);

        Json out = Json::makeObject();
        out.set("object_id", Json(std::to_string(go->GetUID(false))));
        out.set("transform", transformToJson(readTransform(go)));
        return out;
    }

    //--------------------------------------------------------------------------------------
    void SceneBridge::applyPlacement(const Json & _src, Transform & _t)
    {
        auto applyVec3 = [](const Json & _v, double & _x, double & _y, double & _z)
        {
            if (_v.has("x")) _x = _v["x"].asNumber(_x);
            if (_v.has("y")) _y = _v["y"].asNumber(_y);
            if (_v.has("z")) _z = _v["z"].asNumber(_z);
        };

        if (_src.has("position")) applyVec3(_src["position"], _t.px, _t.py, _t.pz);
        if (_src.has("scale"))    applyVec3(_src["scale"],    _t.sx, _t.sy, _t.sz);
        if (_src.has("rotation"))
        {
            const Json & r = _src["rotation"];
            _t.qx = r["x"].asNumber(_t.qx);
            _t.qy = r["y"].asNumber(_t.qy);
            _t.qz = r["z"].asNumber(_t.qz);
            _t.qw = r["w"].asNumber(_t.qw);
        }
    }

    //--------------------------------------------------------------------------------------
    IGameObject * SceneBridge::spawnOne(IGameObject * _source, IGameObject * _parent, const Json & _placement, Json & _errorOut) const
    {
        IObject * clone = _source->Instanciate((InstanciateFlags)0);
        IGameObject * newGO = dynamic_cast<IGameObject *>(clone);
        if (!newGO)
        {
            VG_SAFE_RELEASE(clone);
            _errorOut = error("NOT_SUPPORTED", "engine failed to instanciate the source object");
            return nullptr;
        }

        if (_placement.has("name") && _placement["name"].isString())
            newGO->SetName(_placement["name"].asString());

        _parent->AddChild(newGO);

        if (!newGO->HasValidUID())
            newGO->RegisterUID();

        Transform t = readTransform(newGO);
        applyPlacement(_placement, t);
        writeTransform(newGO, t);

        // AddChild took its own reference; drop the one Instanciate returned. The object
        // itself stays alive as a child of _parent.
        VG_SAFE_RELEASE(clone);
        return newGO;
    }

    //--------------------------------------------------------------------------------------
    Json SceneBridge::spawnObject(const Json & _args)
    {
        Json err;
        IGameObject * source = resolve(_args["source_id"].asString(), err);
        if (!source)
            return err;

        IGameObject * parent = nullptr;
        if (_args.has("parent_id"))
        {
            parent = resolve(_args["parent_id"].asString(), err);
            if (!parent)
                return err;
        }
        else
        {
            parent = dynamic_cast<IGameObject *>(source->GetParent());
        }

        if (!parent)
            return error("INVALID_VALUE", "source has no parent and no parent_id was given");

        // Accept both the nested { transform: { position, ... } } form and a flat one.
        Json placement = _args.has("transform") ? _args["transform"] : _args;
        if (_args.has("name"))
            placement.set("name", _args["name"]);

        IGameObject * newGO = spawnOne(source, parent, placement, err);
        if (!newGO)
            return err;

        return objectToJson(newGO, Detail::Full);
    }

    //--------------------------------------------------------------------------------------
    // Batch clone: one command, N copies. Saves ~N round trips (a 16x17 voxel sprite used
    // to need ~170 spawn_object calls, which timed out under parallel load).
    //--------------------------------------------------------------------------------------
    Json SceneBridge::spawnObjects(const Json & _args)
    {
        Json err;
        IGameObject * source = resolve(_args["source_id"].asString(), err);
        if (!source)
            return err;

        IGameObject * parent = nullptr;
        if (_args.has("parent_id"))
        {
            parent = resolve(_args["parent_id"].asString(), err);
            if (!parent)
                return err;
        }
        else
        {
            parent = dynamic_cast<IGameObject *>(source->GetParent());
        }

        if (!parent)
            return error("INVALID_VALUE", "source has no parent and no parent_id was given");

        const Json & items = _args["items"];
        if (!items.isArray() || items.items().empty())
            return error("INVALID_VALUE", "'items' must be a non-empty array of placements");

        Json spawned = Json::makeArray();
        Json failed  = Json::makeArray();

        for (const Json & item : items.items())
        {
            Json itemErr;
            IGameObject * newGO = spawnOne(source, parent, item, itemErr);
            if (!newGO)
            {
                Json f = Json::makeObject();
                f.set("name", Json(item["name"].asString()));
                f.set("error", itemErr["error"]);
                failed.push(f);
                continue;
            }
            spawned.push(objectToJson(newGO, Detail::Compact));
        }

        Json out = Json::makeObject();
        out.set("parent_id", Json(std::to_string(parent->GetUID(false))));
        out.set("spawned", spawned);
        out.set("count", Json((int)spawned.items().size()));
        if (!failed.items().empty())
            out.set("failed", failed);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Detaches objects from their parent, which releases the last reference and destroys
    // them (and their whole subtree). No undo entry is created, unlike the editor's own
    // delete: a bridge delete cannot be undone with Ctrl-Z.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::deleteObject(const Json & _args)
    {
        std::vector<std::string> ids;
        if (_args.has("object_ids") && _args["object_ids"].isArray())
        {
            for (const Json & id : _args["object_ids"].items())
                ids.push_back(id.asString());
        }
        else if (_args.has("object_id"))
        {
            ids.push_back(_args["object_id"].asString());
        }

        if (ids.empty())
            return error("INVALID_VALUE", "give either object_id or a non-empty object_ids array");

        ISelection * selection = Kernel::getSelection(false);

        Json deleted = Json::makeArray();
        Json failed  = Json::makeArray();

        for (const std::string & id : ids)
        {
            Json itemErr;
            IGameObject * go = resolve(id, itemErr);
            if (!go)
            {
                failed.push(itemErr["error"]);
                continue;
            }

            IGameObject * parent = dynamic_cast<IGameObject *>(go->GetParent());
            if (!parent || go->IsRoot())
            {
                failed.push(error("INVALID_VALUE", "cannot delete a scene root object", id)["error"]);
                continue;
            }

            // The editor keeps deleted objects alive in its undo stack; we do not, so make
            // sure nothing still points at it through the selection.
            if (selection && selection->IsSelectedObject(go))
                selection->Remove(go);

            Json info = Json::makeObject();
            info.set("object_id", Json(id));
            info.set("name", Json(go->GetName()));

            if (!parent->RemoveChild(go))
            {
                failed.push(error("ENGINE_BUSY", "engine refused to remove the object from its parent", id)["error"]);
                continue;
            }
            deleted.push(info);
        }

        Json out = Json::makeObject();
        out.set("deleted", deleted);
        out.set("count", Json((int)deleted.items().size()));
        if (!failed.items().empty())
            out.set("failed", failed);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Creates an empty GameObject meant to be used as a folder for spawned content, so a
    // generated district lands under one node instead of hundreds of siblings at scene root.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::createGroup(const Json & _args)
    {
        const std::string name = _args["name"].asString();
        if (name.empty())
            return error("INVALID_VALUE", "missing group name");

        IGameObject * parent = nullptr;
        if (_args.has("parent_id"))
        {
            Json err;
            parent = resolve(_args["parent_id"].asString(), err);
            if (!parent)
                return err;
        }
        else
        {
            const auto sceneList = scenes();
            if (sceneList.empty())
                return error("NO_ACTIVE_SCENE", "the main world has no scene loaded");
            parent = sceneList[0]->GetRoot();
        }

        if (!parent)
            return error("NO_ACTIVE_SCENE", "could not find a parent for the new group");

        IFactory * factory = Kernel::getFactory(false);
        if (!factory)
            return error("ENGINE_BUSY", "factory not available");

        IObject * obj = factory->CreateObject("GameObject", name, parent);
        IGameObject * group = dynamic_cast<IGameObject *>(obj);
        if (!group)
        {
            VG_SAFE_RELEASE(obj);
            return error("NOT_SUPPORTED", "engine failed to create a GameObject");
        }

        group->RegisterUID();
        parent->AddChild(group);

        // Placed at the parent's origin unless asked otherwise; children spawned under it
        // then use coordinates relative to the group.
        if (_args.has("position") || _args.has("rotation") || _args.has("scale"))
        {
            Transform t = readTransform(group);
            applyPlacement(_args, t);
            writeTransform(group, t);
        }

        Json out = objectToJson(group, Detail::Full);
        out.set("parent_id", Json(std::to_string(parent->GetUID(false))));

        VG_SAFE_RELEASE(obj); // AddChild holds the reference now
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Adds an empty scene to the world, exactly like the editor's SceneList > New Scene
    // (ImGuiSceneList.hpp:675): CreateSceneResource writes a .scene file holding a single
    // "Root" GameObject, registers the resource and lets the resource manager load it.
    //
    // That load is asynchronous, so the new scene is NOT in the world yet when this returns:
    // call list_objects a moment later and read its root_id from the "scenes" array.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::createScene(const Json & _args)
    {
        if (!m_engine)
            return error("ENGINE_BUSY", "engine not available");

        const std::string name = _args["name"].asString();
        if (name.empty())
            return error("INVALID_VALUE", "missing scene name");

        // The name becomes a file name and the scene's display name: keep it tame rather
        // than letting a path escape the data folder.
        if (name.find_first_of("/\\:*?\"<>|") != std::string::npos || name.find("..") != std::string::npos)
            return error("INVALID_VALUE", "scene name must not contain a path or wildcard character");

        std::string folder = _args.has("folder") ? _args["folder"].asString() : std::string("data/Scenes");
        while (!folder.empty() && (folder.back() == '/' || folder.back() == '\\'))
            folder.pop_back();
        if (folder.empty())
            return error("INVALID_VALUE", "empty folder");

        const std::string file = folder + "/" + name + ".scene";

        engine::IWorldResource * worldRes = m_engine->GetWorldResource();
        if (!worldRes)
            return error("ENGINE_BUSY", "no world resource loaded");

        // CreateSceneResource is a no-op when the world data is not there yet (WorldResource.hpp:127),
        // so without this a scene created right after create_world would report success and do nothing.
        if (!worldRes->GetObject())
            return error("WORLD_NOT_READY", "the world is still loading — retry in a moment");

        // CreateSceneResource overwrites without asking, so guard both the file and the
        // world: re-creating an existing scene would silently wipe its content.
        if (io::exists(file))
            return error("ALREADY_EXISTS", "\"" + file + "\" already exists — pick another name");

        for (const IBaseScene * scene : scenes())
        {
            if (scene->GetName() == name)
                return error("ALREADY_EXISTS", "a scene named \"" + name + "\" is already loaded in the world");
        }

        io::createFolder(folder);
        worldRes->CreateSceneResource(file, BaseSceneType::Scene);

        Json out = Json::makeObject();
        out.set("name", Json(name));
        out.set("file", Json(file));
        out.set("loading", Json(true));
        out.set("note", Json("scene loads asynchronously: call list_objects to get its root_id, "
                             "then save_world to record it in the world file"));
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Creates a new empty .world and makes it the one the editor has open — the editor's
    // File > World > New (Editor.cpp:680), which is IEngine::CreateWorld (Engine.cpp:564).
    //
    // The engine holds a SINGLE world resource, so this REPLACES the world currently open:
    // unsaved changes to it are lost, and getting back to it means File > World > Open.
    // The .world file on disk is untouched, and so is EngineOptions::m_startWorld
    // (Engine.xml), which is what the standalone game loads at startup.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::createWorld(const Json & _args)
    {
        if (!m_engine)
            return error("ENGINE_BUSY", "engine not available");

        const std::string name = _args["name"].asString();
        if (name.empty())
            return error("INVALID_VALUE", "missing world name");

        if (name.find_first_of("/\\:*?\"<>|") != std::string::npos || name.find("..") != std::string::npos)
            return error("INVALID_VALUE", "world name must not contain a path or wildcard character");

        std::string folder = _args.has("folder") ? _args["folder"].asString() : std::string("data/Worlds");
        while (!folder.empty() && (folder.back() == '/' || folder.back() == '\\'))
            folder.pop_back();
        if (folder.empty())
            return error("INVALID_VALUE", "empty folder");

        const std::string file = folder + "/" + name + ".world";

        // WorldResource::CreateFile writes over an existing file without asking (WorldResource.hpp:87).
        if (io::exists(file))
            return error("ALREADY_EXISTS", "\"" + file + "\" already exists — pick another name");

        std::string previous;
        if (engine::IWorldResource * worldRes = m_engine->GetWorldResource())
            previous = worldRes->GetResourcePath();

        io::createPath(folder);

        if (!m_engine->CreateWorld(file))
            return error("IO_ERROR", "CreateWorld failed for \"" + file + "\"");

        // CreateFile reports success without checking that the XML was actually written.
        if (!io::exists(file))
            return error("IO_ERROR", "\"" + file + "\" was not written");

        Json out = Json::makeObject();
        out.set("name", Json(name));
        out.set("file", Json(file));
        out.set("previous_world", Json(previous));
        out.set("loading", Json(true));
        out.set("note", Json("this world is now the one the editor has open, replacing \"" + previous +
                             "\"; it loads asynchronously, so wait for create_scene to stop answering "
                             "WORLD_NOT_READY before adding scenes"));
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Saves every Scene of the world and then the world file itself — the same order as the
    // editor's "Save All".
    //
    // IEngine::SaveWorld() alone only rewrites the .world file, which holds scene references
    // and no GameObject: saving just that reports success while silently dropping every
    // object the bridge created (verified 2026-09-12).
    //--------------------------------------------------------------------------------------
    Json SceneBridge::saveWorld(const Json &)
    {
        if (!m_engine)
            return error("ENGINE_BUSY", "engine not available");

        engine::IWorldResource * worldRes = m_engine->GetWorldResource();
        if (!worldRes)
            return error("ENGINE_BUSY", "no world resource loaded");

        Json sceneResults = Json::makeArray();
        int savedScenes = 0;

        const uint sceneCount = worldRes->GetSceneResourceCount(BaseSceneType::Scene);
        for (uint i = 0; i < sceneCount; ++i)
        {
            IResource * sceneRes = worldRes->GetSceneResource(i, BaseSceneType::Scene);
            if (!sceneRes)
                continue;

            const bool ok = m_engine->SaveScene(sceneRes);
            if (ok)
                ++savedScenes;

            Json s = Json::makeObject();
            s.set("file", Json(sceneRes->GetResourcePath()));
            s.set("saved", Json(ok));
            sceneResults.push(s);
        }

        const bool worldOk = m_engine->SaveWorld();

        Json out = Json::makeObject();
        out.set("saved", Json(worldOk && savedScenes == (int)sceneResults.items().size()));
        out.set("world_saved", Json(worldOk));
        out.set("scenes_saved", Json(savedScenes));
        out.set("scenes", sceneResults);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Textures sit next to the FBX and are named <MaterialName><Suffix>.<ext>. Several
    // spellings are accepted per slot because the convention drifted over time.
    //--------------------------------------------------------------------------------------
    std::string SceneBridge::findTexture(const std::string & _folder, const std::string & _stem, const char * const * _suffixes)
    {
        static const char * const exts[] = { ".png", ".jpg", ".jpeg", ".tga", ".dds", ".bmp", nullptr };

        if (!io::exists(_folder))
            return std::string();

        const auto lower = [](std::string _s)
        {
            for (char & c : _s)
                c = (char)tolower((unsigned char)c);
            return _s;
        };

        const auto entries = io::getFilesInFolder(_folder);

        // Matched case-insensitively because Windows is, but returned with the on-disk
        // spelling: this path is written into the .mat and travels to case-sensitive tools.
        for (const char * const * suffix = _suffixes; *suffix; ++suffix)
        {
            for (const char * const * ext = exts; *ext; ++ext)
            {
                const std::string wanted = lower(_stem + *suffix + *ext);

                for (const auto & entry : entries)
                {
                    if (entry.isFolder)
                        continue;

                    const std::string file = entry.name.c_str();
                    if (lower(file) == wanted)
                        return _folder + "/" + file;
                }
            }
        }
        return std::string();
    }

    //--------------------------------------------------------------------------------------
    // Writes a resource path WITHOUT loading it: Resource::SetResourcePath queues
    // LoadResourceAsync(this, ...) (Resource.cpp:240), and these resources only exist long
    // enough to be serialized - the manager would be left holding freed clients. Writing the
    // "m_resourcePath" string property directly serializes the same XML with nothing queued.
    //--------------------------------------------------------------------------------------
    static void setResourcePathCold(IResource * _res, const std::string & _path)
    {
        if (!_res || _path.empty())
            return;

        const IClassDesc * desc = _res->GetClassDesc();
        const IProperty *  prop = desc ? desc->GetPropertyByName("m_resourcePath") : nullptr;

        if (string * value = prop ? prop->GetPropertyString(_res) : nullptr)
            *value = _path;
    }

    //--------------------------------------------------------------------------------------
    // Nothing is overwritten unless asked: an existing file makes the writer fall back to
    // "<name>-01", "-02"... so a generated asset can sit next to a hand-made one and be diffed.
    //--------------------------------------------------------------------------------------
    static std::string uniquePath(const std::string & _folder, const std::string & _stem, const char * _ext, bool _overwrite)
    {
        const std::string base = _folder + "/" + _stem + _ext;
        if (_overwrite || !io::exists(base))
            return base;

        for (int i = 1; i < 100; ++i)
        {
            char suffix[8];
            snprintf(suffix, sizeof(suffix), "-%02d", i);

            const std::string candidate = _folder + "/" + _stem + suffix + _ext;
            if (!io::exists(candidate))
                return candidate;
        }

        return base;
    }

    //--------------------------------------------------------------------------------------
    // A .mat is an XML serialization of a MaterialResourceData. Assigning "m_shader" is what
    // allocates the matching data block (MaterialResourceData.hpp:51) and DefaultMaterialData's
    // constructor already yields Opaque / Back / UV0 / tiling 1 / offset 0, so only the maps
    // are written here.
    //--------------------------------------------------------------------------------------
    bool SceneBridge::writeMaterial(const std::string & _matPath, const std::string & _name,
                                    const std::string & _albedo, const std::string & _normal,
                                    const std::string & _pbr)
    {
        IFactory * factory = Kernel::getFactory(false);
        if (!factory)
            return false;

        IObject * matResData = factory->CreateObject("MaterialResourceData", _name);
        if (!matResData)
            return false;

        bool ok = false;

        const IClassDesc * matDesc       = matResData->GetClassDesc();
        const IProperty *  modelTypeProp = matDesc ? matDesc->GetPropertyByName("m_materialModelType") : nullptr;
        IObject *          modelType     = modelTypeProp ? modelTypeProp->GetPropertyObject(matResData) : nullptr;
        const IClassDesc * typeDesc      = modelType ? modelType->GetClassDesc() : nullptr;
        const IProperty *  shaderProp    = typeDesc ? typeDesc->GetPropertyByName("m_shader") : nullptr;
        string *           shader        = shaderProp ? shaderProp->GetPropertyString(modelType) : nullptr;

        if (shader)
        {
            *shader = "Default";
            matResData->OnPropertyChanged(modelType, *shaderProp, false);

            const IProperty * dataProp = matDesc->GetPropertyByName("m_data");
            IObject **        dataPtr  = dataProp ? dataProp->GetPropertyObjectPtr(matResData) : nullptr;
            IObject *         data     = dataPtr ? *dataPtr : nullptr;

            if (data)
            {
                const IClassDesc * dataDesc = data->GetClassDesc();

                const struct { const char * prop; const std::string * path; } maps[] =
                {
                    { "m_albedoMap", &_albedo },
                    { "m_normalMap", &_normal },
                    { "m_pbrMap",    &_pbr    },
                };

                for (const auto & map : maps)
                {
                    if (map.path->empty())
                        continue;

                    const IProperty * prop = dataDesc ? dataDesc->GetPropertyByName(map.prop) : nullptr;
                    setResourcePathCold(prop ? prop->GetPropertyResource(data) : nullptr, *map.path);
                }

                ok = factory->SaveToXML(matResData, _matPath);
            }
        }

        VG_SAFE_RELEASE(matResData);
        return ok;
    }

    //--------------------------------------------------------------------------------------
    // The model exposes one batch per FBX material ID, so GetBatchNames() both sizes and names
    // the material list. Returns a null Json while the import is still in flight.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::finishPrefab(StagedMesh & _mesh, const std::string & _prefabFolder,
                                   const std::string & _materialFolder, bool _overwrite, Json & _warnings)
    {
        IFactory * factory = Kernel::getFactory(false);
        if (!factory || !_mesh.object)
            return Json();

        IComponent * comp = _mesh.object->GetComponentByType("MeshComponent");
        if (!comp)
            return Json();

        const IClassDesc * compDesc = comp->GetClassDesc();
        const IProperty *  meshProp = compDesc ? compDesc->GetPropertyByName("m_meshResource") : nullptr;
        IResource *        meshRes  = meshProp ? meshProp->GetPropertyResource(comp) : nullptr;

        renderer::IGraphicModel * model = meshRes ? dynamic_cast<renderer::IGraphicModel *>(meshRes->GetObject()) : nullptr;
        if (!model)
            return Json(); // still importing

        const std::string meshFolder = io::getFileDir(_mesh.fbx);
        const std::string matDir     = _materialFolder + "/" + _mesh.name;

        static const char * const albedoSuffixes[] = { "_BaseColor", "_Color", "_Albedo", nullptr };
        static const char * const normalSuffixes[] = { "_Normal", "_NormalMap", nullptr };
        static const char * const pbrSuffixes[]    = { "_OcclusionRoughnessMetallic", "_ORM", "_PBR", nullptr };

        io::createPath(matDir); // createFolder only makes one level, so an unseen parent would fail

        Json materials = Json::makeArray();
        std::vector<std::pair<std::string, std::string>> slots;

        for (const auto & batchName : model->GetBatchNames())
        {
            const std::string name    = batchName.c_str();
            const std::string matPath = uniquePath(matDir, name, ".mat", _overwrite);
            const std::string albedo  = findTexture(meshFolder, name, albedoSuffixes);
            const std::string normal  = findTexture(meshFolder, name, normalSuffixes);
            const std::string pbr     = findTexture(meshFolder, name, pbrSuffixes);

            if (albedo.empty() && normal.empty() && pbr.empty())
                _warnings.push(Json("no texture named \"" + name + "_BaseColor|_Normal|_OcclusionRoughnessMetallic\" in " +
                                    meshFolder + " - \"" + name + "\" written without maps"));

            if (!writeMaterial(matPath, name, albedo, normal, pbr))
                _warnings.push(Json("failed to write \"" + matPath + "\""));

            Json m = Json::makeObject();
            m.set("name",   Json(name));
            m.set("file",   Json(matPath));
            m.set("albedo", Json(albedo));
            m.set("normal", Json(normal));
            m.set("pbr",    Json(pbr));
            materials.push(m);

            slots.emplace_back(name, matPath);
        }

        // One slot per batch, in batch order: the engine matches them by index.
        const IProperty * matsProp = compDesc ? compDesc->GetPropertyByName("m_meshMaterials") : nullptr;
        IObject *         matsObj  = matsProp ? matsProp->GetPropertyObject(comp) : nullptr;

        if (IResourceList * list = dynamic_cast<IResourceList *>(matsObj))
        {
            while (list->Size() > 0)
                list->Pop();

            // Add() only loads when given a non-empty path (ResourceList.h:36), so the slot is
            // created cold and the path written afterwards - this component is serialized and
            // destroyed right away, it must not leave material loads queued behind it.
            for (const auto & slot : slots)
                list->Add(slot.first.c_str(), "");

            const IProperty * resourcesProp = matsObj->GetClassDesc() ? matsObj->GetClassDesc()->GetPropertyByName("m_resources") : nullptr;
            if (resourcesProp)
            {
                const uint count = resourcesProp->GetPropertyResourceVectorCount(matsObj);
                for (uint i = 0; i < count && i < slots.size(); ++i)
                    setResourcePathCold(resourcesProp->GetPropertyResourceVectorElement(matsObj, i), slots[i].second);
            }
        }
        else
        {
            _warnings.push(Json("could not reach the material list of \"" + _mesh.name + "\" - prefab written without materials"));
        }

        const std::string prefabDir = _prefabFolder + "/" + _mesh.name;
        io::createPath(prefabDir);

        const std::string prefabPath = uniquePath(prefabDir, _mesh.name, ".prefab", _overwrite);

        Json out = Json::makeObject();
        out.set("mesh",      Json(_mesh.fbx));
        out.set("prefab",    Json(prefabPath));
        out.set("materials", materials);

        // The staged object lives under the staging group, so it is disposed of the same way
        // delete_object does - never by releasing a detached GameObject by hand.
        IGameObject * parent = dynamic_cast<IGameObject *>(_mesh.object->GetParent());

        IObject *     prefabObj = factory->CreateObject("PrefabGameObject", _mesh.name);
        IGameObject * prefabGO  = dynamic_cast<IGameObject *>(prefabObj);
        IResource *   prefabRes = prefabGO ? prefabGO->GetPrefabResource() : nullptr;

        bool written = false;
        if (prefabRes)
        {
            // Same hand-off as the editor's Create Prefab (ImGuiGameObjectSceneEditorMenu.hpp:579):
            // hold a reference, unparent, then let CreateFile re-parent under its own "Root" and
            // release it (PrefabResource.hpp:93). The staged pointer is dead afterwards.
            VG_SAFE_INCREASE_REFCOUNT(_mesh.object);
            if (parent)
                parent->RemoveChild(_mesh.object);

            // CreateFile reports success as soon as it built the PrefabScene and never checks
            // whether SaveToXML actually wrote (PrefabResource.hpp:99), so confirm on disk.
            written = prefabRes->CreateFile(prefabPath, _mesh.object) && io::exists(prefabPath);
            _mesh.object = nullptr;
        }
        else
        {
            _warnings.push(Json("engine did not provide a PrefabResource for \"" + _mesh.name + "\""));

            if (parent)
                parent->RemoveChild(_mesh.object);
            _mesh.object = nullptr;
        }

        VG_SAFE_RELEASE(prefabObj);

        out.set("written", Json(written));
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Puts objects in the editor's selection, so whatever the bridge just built can be framed
    // with the editor's own focus shortcut and shows up in the Inspector.
    //
    // This is the reachable half of "point the camera at it": the viewport camera is
    // ImGuiView::m_editorCam, private editor state re-fed to the view every frame
    // (ImGuiView.hpp:239), so moving it would mean modifying the editor.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::selectObject(const Json & _args)
    {
        ISelection * selection = Kernel::getSelection(false);
        if (!selection)
            return error("ENGINE_BUSY", "selection not available");

        std::vector<std::string> ids;
        if (_args.has("object_ids") && _args["object_ids"].isArray())
        {
            for (const Json & id : _args["object_ids"].items())
                ids.push_back(id.asString());
        }
        else if (_args.has("object_id"))
        {
            ids.push_back(_args["object_id"].asString());
        }

        const bool add = _args["add"].asBool(false);

        if (ids.empty())
        {
            if (!_args["clear"].asBool(false))
                return error("INVALID_VALUE", "give object_id, object_ids, or clear:true");

            selection->Clear();

            Json out = Json::makeObject();
            out.set("cleared", Json(true));
            out.set("count", Json(0));
            return out;
        }

        core::vector<IObject *> objects;
        Json selected = Json::makeArray();
        Json failed   = Json::makeArray();

        for (const std::string & id : ids)
        {
            Json itemErr;
            IGameObject * go = resolve(id, itemErr);
            if (!go)
            {
                failed.push(itemErr["error"]);
                continue;
            }

            objects.push_back(go);

            Json info = Json::makeObject();
            info.set("object_id", Json(id));
            info.set("name", Json(go->GetName()));
            selected.push(info);
        }

        if (objects.empty())
        {
            Json out = Json::makeObject();
            out.set("count", Json(0));
            out.set("failed", failed);
            return out;
        }

        if (add)
        {
            for (IObject * obj : objects)
                selection->Add(obj);
        }
        else
        {
            selection->SetSelectedObjects(objects);
        }

        Json out = Json::makeObject();
        out.set("selected", selected);
        out.set("count", Json((int)selected.items().size()));
        out.set("note", Json("use the editor's focus shortcut to frame the selection"));
        if (!failed.items().empty())
            out.set("failed", failed);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Shows or hides an object and its whole subtree - IInstance::Enable, the same switch as
    // the editor's checkbox. Handy to put a scene aside while working next to it: two scenes
    // of one world share the same coordinate space and are drawn together, so an untouched
    // level otherwise sits on top of whatever is being built.
    //
    // Enabling a child of a disabled parent leaves it invisible, hence enabled_in_hierarchy
    // in the result.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::setEnabled(const Json & _args)
    {
        if (!_args.has("enabled"))
            return error("INVALID_VALUE", "missing enabled (true or false)");

        const bool enabled = _args["enabled"].asBool(true);

        std::vector<std::string> ids;
        if (_args.has("object_ids") && _args["object_ids"].isArray())
        {
            for (const Json & id : _args["object_ids"].items())
                ids.push_back(id.asString());
        }
        else if (_args.has("object_id"))
        {
            ids.push_back(_args["object_id"].asString());
        }

        if (ids.empty())
            return error("INVALID_VALUE", "give either object_id or a non-empty object_ids array");

        Json updated = Json::makeArray();
        Json failed  = Json::makeArray();

        for (const std::string & id : ids)
        {
            Json itemErr;
            IGameObject * go = resolve(id, itemErr);
            if (!go)
            {
                failed.push(itemErr["error"]);
                continue;
            }

            go->Enable(enabled);

            Json info = Json::makeObject();
            info.set("object_id", Json(id));
            info.set("name", Json(go->GetName()));
            info.set("enabled", Json(enabled));
            info.set("enabled_in_hierarchy", Json(go->IsEnabledInHierarchy()));
            updated.push(info);
        }

        Json out = Json::makeObject();
        out.set("updated", updated);
        out.set("count", Json((int)updated.items().size()));
        if (!failed.items().empty())
            out.set("failed", failed);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Assigns a .mat to the material slots of an object's MeshComponent.
    //
    // Without this every clone of a source object keeps the source's material, so anything
    // built from one reference box comes out monochrome.
    //
    // The component is looked up in the children too, because that is where it sits on a
    // prefab instance (Box_Base/Root/Box_Base). The object stays in the scene, so unlike
    // create_prefabs the load queued by SetResourcePath is exactly what makes the new material
    // show up in the viewport.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::setMaterial(const Json & _args)
    {
        const std::string material = _args["material"].asString();
        if (material.empty())
            return error("INVALID_VALUE", "missing material path");
        if (material.find("..") != std::string::npos)
            return error("INVALID_VALUE", "material path must not escape the project");
        if (!io::fileHasExtension(material, ".mat"))
            return error("INVALID_VALUE", "\"" + material + "\" is not a .mat file");
        if (!io::exists(material))
            return error("NOT_FOUND", "\"" + material + "\" does not exist");

        std::vector<std::string> ids;
        if (_args.has("object_ids") && _args["object_ids"].isArray())
        {
            for (const Json & id : _args["object_ids"].items())
                ids.push_back(id.asString());
        }
        else if (_args.has("object_id"))
        {
            ids.push_back(_args["object_id"].asString());
        }

        if (ids.empty())
            return error("INVALID_VALUE", "give either object_id or a non-empty object_ids array");

        const bool hasSlot = _args.has("slot");
        const int  slot    = hasSlot ? (int)_args["slot"].asNumber(0.0) : -1;
        if (hasSlot && slot < 0)
            return error("INVALID_VALUE", "slot must be >= 0");

        Json updated = Json::makeArray();
        Json failed  = Json::makeArray();

        for (const std::string & id : ids)
        {
            Json itemErr;
            IGameObject * go = resolve(id, itemErr);
            if (!go)
            {
                failed.push(itemErr["error"]);
                continue;
            }

            IComponent * comp = go->GetComponentByType("MeshComponent", false, true);
            if (!comp)
            {
                failed.push(error("NOT_SUPPORTED", "no MeshComponent on this object or its children", id)["error"]);
                continue;
            }

            const IClassDesc * compDesc = comp->GetClassDesc();
            const IProperty *  matsProp = compDesc ? compDesc->GetPropertyByName("m_meshMaterials") : nullptr;
            IObject *          matsObj  = matsProp ? matsProp->GetPropertyObject(comp) : nullptr;
            IResourceList *    list     = dynamic_cast<IResourceList *>(matsObj);

            if (!list)
            {
                failed.push(error("NOT_SUPPORTED", "could not reach the material list", id)["error"]);
                continue;
            }

            // A slot only exists once something sized the list, which does not happen on its
            // own here: grow it to the mesh's batch count, or far enough to reach the slot.
            size_t wanted = hasSlot ? (size_t)slot + 1 : list->Size();
            if (!hasSlot && wanted == 0)
            {
                const IProperty * meshProp = compDesc->GetPropertyByName("m_meshResource");
                IResource *       meshRes  = meshProp ? meshProp->GetPropertyResource(comp) : nullptr;

                if (auto * model = meshRes ? dynamic_cast<renderer::IGraphicModel *>(meshRes->GetObject()) : nullptr)
                    wanted = model->GetBatchCount();
            }

            while (list->Size() < wanted)
                list->Add("Material", "");

            if (list->Size() == 0)
            {
                failed.push(error("NOT_SUPPORTED", "mesh has no material slot yet - is it still loading?", id)["error"]);
                continue;
            }

            const IProperty * resourcesProp = matsObj->GetClassDesc() ? matsObj->GetClassDesc()->GetPropertyByName("m_resources") : nullptr;
            if (!resourcesProp)
            {
                failed.push(error("NOT_SUPPORTED", "could not reach the material slots", id)["error"]);
                continue;
            }

            const uint count   = resourcesProp->GetPropertyResourceVectorCount(matsObj);
            const uint first   = hasSlot ? (uint)slot : 0u;
            const uint last    = hasSlot ? (uint)slot : (count ? count - 1 : 0u);
            int        applied = 0;

            for (uint i = first; i <= last && i < count; ++i)
            {
                if (IResource * res = resourcesProp->GetPropertyResourceVectorElement(matsObj, i))
                {
                    res->SetResourcePath(material);
                    ++applied;
                }
            }

            if (applied == 0)
            {
                failed.push(error("INVALID_VALUE", "slot out of range (" + std::to_string(count) + " slot(s))", id)["error"]);
                continue;
            }

            Json info = Json::makeObject();
            info.set("object_id", Json(id));
            info.set("name", Json(go->GetName()));
            info.set("slots_set", Json(applied));
            info.set("slot_count", Json((int)count));
            updated.push(info);
        }

        Json out = Json::makeObject();
        out.set("material", Json(material));
        out.set("updated", updated);
        out.set("count", Json((int)updated.items().size()));
        if (!failed.items().empty())
            out.set("failed", failed);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Places a .prefab file in the scene - the editor's Add Prefab
    // (ImGuiGameObjectSceneEditorMenu.hpp:565).
    //
    // This is what spawn_object cannot do: spawn_object only CLONES an object already present
    // in the loaded world, so a prefab with no instance anywhere was simply unreachable. Here
    // any prefab file can be laid out, including whatever create_prefabs just produced.
    //
    // The prefab CONTENT loads asynchronously: the object and its transform exist on return,
    // its children appear a moment later - call list_objects again to see them.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::instantiatePrefab(const Json & _args)
    {
        IFactory * factory = Kernel::getFactory(false);
        if (!factory)
            return error("ENGINE_BUSY", "factory not available");

        const std::string prefab = _args["prefab"].asString();
        if (prefab.empty())
            return error("INVALID_VALUE", "missing prefab path");
        if (prefab.find("..") != std::string::npos)
            return error("INVALID_VALUE", "prefab path must not escape the project");
        if (!io::fileHasExtension(prefab, ".prefab"))
            return error("INVALID_VALUE", "\"" + prefab + "\" is not a .prefab file");
        if (!io::exists(prefab))
            return error("NOT_FOUND", "\"" + prefab + "\" does not exist");

        IGameObject * parent = nullptr;
        if (_args.has("parent_id"))
        {
            Json err;
            parent = resolve(_args["parent_id"].asString(), err);
            if (!parent)
                return err;
        }
        else
        {
            const auto sceneList = scenes();
            if (sceneList.empty())
                return error("NO_ACTIVE_SCENE", "the main world has no scene loaded");
            parent = sceneList[0]->GetRoot();
        }

        if (!parent)
            return error("NO_ACTIVE_SCENE", "could not find a parent for the instance");

        // Either one instance placed by the top-level position/rotation/scale, or a batch:
        // one engine round trip for a whole row of props instead of one call each.
        const std::vector<Json>   single(1, _args);
        const bool                batch = _args.has("items") && _args["items"].isArray();
        const std::vector<Json> & placements = batch ? _args["items"].items() : single;

        if (placements.empty())
            return error("INVALID_VALUE", "items is empty");

        const std::string defaultName = io::getFileNameWithoutExt(prefab).c_str();

        Json created = Json::makeArray();

        for (const Json & placement : placements)
        {
            const std::string name = placement.has("name") ? placement["name"].asString() : defaultName;

            IObject *     obj = factory->CreateObject("PrefabGameObject", name, parent);
            IGameObject * go  = dynamic_cast<IGameObject *>(obj);
            if (!go)
            {
                VG_SAFE_RELEASE(obj);
                continue;
            }

            go->RegisterUID();

            // Assigning the path is what pulls the prefab content in. Unlike create_prefabs,
            // this object stays in the scene, so the load it queues is wanted.
            if (IResource * res = go->GetPrefabResource())
                res->SetResourcePath(prefab);

            parent->AddChild(go);

            if (placement.has("position") || placement.has("rotation") || placement.has("scale"))
            {
                Transform t = readTransform(go);
                applyPlacement(placement, t);
                writeTransform(go, t);
            }

            created.push(objectToJson(go, Detail::Compact));
            VG_SAFE_RELEASE(obj); // AddChild holds the reference now
        }

        Json out = Json::makeObject();
        out.set("prefab",    Json(prefab));
        out.set("parent_id", Json(std::to_string(parent->GetUID(false))));
        out.set("created",   created);
        out.set("count",     Json((int)created.items().size()));
        out.set("note", Json("prefab content loads asynchronously: call list_objects again to see the children, and save_world to persist"));
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Turns every FBX under a folder into data/Prefabs/<Mesh>/<Mesh>.prefab, with one
    // data/Materials/<Mesh>/<Material>.mat per FBX material ID.
    //
    // Importing an FBX is ASYNCHRONOUS: the first call only stages the meshes and returns
    // pending > 0. Call it again with the same arguments until pending reaches 0.
    //--------------------------------------------------------------------------------------
    Json SceneBridge::createPrefabs(const Json & _args)
    {
        IFactory * factory = Kernel::getFactory(false);
        if (!factory)
            return error("ENGINE_BUSY", "factory not available");

        const std::string prefabFolder   = _args.has("prefab_folder")   ? _args["prefab_folder"].asString()   : std::string("data/Prefabs");
        const std::string materialFolder = _args.has("material_folder") ? _args["material_folder"].asString() : std::string("data/Materials");
        const bool        overwrite      = _args["overwrite"].asBool(false);

        if (m_staged.empty())
        {
            std::string folder = _args["folder"].asString();
            while (!folder.empty() && (folder.back() == '/' || folder.back() == '\\'))
                folder.pop_back();

            if (folder.empty())
                return error("INVALID_VALUE", "missing folder");
            if (folder.find("..") != std::string::npos)
                return error("INVALID_VALUE", "folder must not escape the project");
            if (!io::exists(folder))
                return error("NOT_FOUND", "\"" + folder + "\" does not exist");

            // Accepts both a single mesh folder and a folder holding one folder per mesh.
            std::vector<std::string> dirs;
            dirs.push_back(folder);
            for (const auto & entry : io::getFilesInFolder(folder))
            {
                const std::string name = entry.name.c_str();
                if (entry.isFolder && name != "." && name != "..")
                    dirs.push_back(folder + "/" + name);
            }

            // Staged objects are parked in the scene: a GameObject that owns resources must be
            // created and destroyed through the scene graph, not held detached.
            const auto sceneList = scenes();
            if (sceneList.empty())
                return error("NO_ACTIVE_SCENE", "the main world has no scene loaded");

            IGameObject * sceneRoot = sceneList[0]->GetRoot();
            if (!sceneRoot)
                return error("NO_ACTIVE_SCENE", "the first scene has no root");

            IObject *     stagingObj   = factory->CreateObject("GameObject", "__mcp_prefab_staging", sceneRoot);
            IGameObject * stagingGroup = dynamic_cast<IGameObject *>(stagingObj);
            if (!stagingGroup)
            {
                VG_SAFE_RELEASE(stagingObj);
                return error("NOT_SUPPORTED", "engine failed to create the staging group");
            }

            stagingGroup->RegisterUID();
            sceneRoot->AddChild(stagingGroup);
            VG_SAFE_RELEASE(stagingObj); // the scene root owns it now

            for (const std::string & dir : dirs)
            {
                for (const auto & entry : io::getFilesInFolder(dir))
                {
                    const std::string file = entry.name.c_str();
                    if (entry.isFolder || !io::fileHasExtension(file, ".fbx"))
                        continue;

                    const std::string fbx  = dir + "/" + file;
                    const std::string name = io::getFileNameWithoutExt(file).c_str();

                    IObject *     obj = factory->CreateObject("GameObject", name, stagingGroup);
                    IGameObject * go  = dynamic_cast<IGameObject *>(obj);
                    if (!go)
                    {
                        VG_SAFE_RELEASE(obj);
                        continue;
                    }

                    go->RegisterUID();
                    stagingGroup->AddChild(go);
                    VG_SAFE_RELEASE(obj); // the staging group owns it now

                    // Assigning the path is what starts the import.
                    IComponent * comp = go->AddComponent("MeshComponent", "New MeshComponent");
                    const IClassDesc * compDesc = comp ? comp->GetClassDesc() : nullptr;
                    const IProperty *  meshProp = compDesc ? compDesc->GetPropertyByName("m_meshResource") : nullptr;
                    if (IResource * meshRes = meshProp ? meshProp->GetPropertyResource(comp) : nullptr)
                        meshRes->SetResourcePath(fbx);

                    StagedMesh staged;
                    staged.fbx    = fbx;
                    staged.name   = name;
                    staged.object = go;
                    m_staged.push_back(staged);
                }
            }

            if (m_staged.empty())
                return error("NOT_FOUND", "no .fbx found under \"" + folder + "\"");

            Json out = Json::makeObject();
            out.set("staged",  Json((double)m_staged.size()));
            out.set("pending", Json((double)m_staged.size()));
            out.set("note", Json("FBX import is asynchronous: call create_prefabs again with the same arguments to write the prefabs once the models are ready"));
            return out;
        }

        Json created  = Json::makeArray();
        Json warnings = Json::makeArray();

        for (size_t i = 0; i < m_staged.size(); )
        {
            Json result = finishPrefab(m_staged[i], prefabFolder, materialFolder, overwrite, warnings);
            if (result.isNull())
            {
                ++i; // still importing, try again on the next call
                continue;
            }

            created.push(result);
            m_staged.erase(m_staged.begin() + i); // finishPrefab always disposes of the object
        }

        // Nothing left to import: take the staging group back out of the scene.
        if (m_staged.empty())
        {
            const auto sceneList = scenes();
            IGameObject * sceneRoot = sceneList.empty() ? nullptr : sceneList[0]->GetRoot();

            if (sceneRoot)
            {
                for (uint i = 0; i < sceneRoot->GetChildren().size(); ++i)
                {
                    IGameObject * child = sceneRoot->GetChildren()[i];
                    if (child && child->GetName() == "__mcp_prefab_staging")
                    {
                        sceneRoot->RemoveChild(child);
                        break;
                    }
                }
            }
        }

        Json out = Json::makeObject();
        out.set("created",  created);
        out.set("pending",  Json((double)m_staged.size()));
        out.set("warnings", warnings);
        return out;
    }
}
