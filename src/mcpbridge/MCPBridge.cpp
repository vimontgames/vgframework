#include "mcpbridge/Precomp.h"
#include "mcpbridge/MCPBridge.h"
#include "mcpbridge/SceneBridge.h"
#include "mcpbridge/Json.h"

#include "core/Kernel.h"
#include "core/ILogger.h"
#include "core/File/File.h"
#include "engine/IEngine.h"

#include <cstdlib>

using namespace vg::core;

//--------------------------------------------------------------------------------------
vg::mcpbridge::IMCPBridge * CreateNew()
{
    return new vg::mcpbridge::MCPBridge("MCPBridge", nullptr);
}

namespace vg::mcpbridge
{
    //--------------------------------------------------------------------------------------
    MCPBridge::MCPBridge(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {
    }

    //--------------------------------------------------------------------------------------
    MCPBridge::~MCPBridge()
    {
    }

    //--------------------------------------------------------------------------------------
    bool MCPBridge::RegisterClasses()
    {
        IFactory * factory = Kernel::getFactory();

        bool result = AutoRegisterClassInfo::registerClasses(*factory);

        if (IClassDesc * desc = factory->registerPlugin(MCPBridge, "MCPBridge"))
            result |= registerProperties(*desc);

        return result;
    }

    //--------------------------------------------------------------------------------------
    bool MCPBridge::UnregisterClasses()
    {
        return AutoRegisterClassInfo::unregisterClasses();
    }

    //--------------------------------------------------------------------------------------
    bool MCPBridge::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    void MCPBridge::Init(engine::IEngine * _engine, Singletons & _singletons)
    {
        Kernel::setSingletons(_singletons);
        RegisterClasses();

        m_engine = _engine;
        m_enabled = nullptr != getenv("VG_MCP_BRIDGE");

        if (!m_enabled)
        {
            VG_INFO("[MCPBridge] disabled (set the VG_MCP_BRIDGE environment variable to enable)");
            return;
        }

        const string cwd = io::getCurrentWorkingDirectory();
        m_dir          = cwd + "/mcp";
        m_commandsPath = m_dir + "/commands.jsonl";
        m_statePath    = m_dir + "/state.json";

        io::createFolder(m_dir);

        m_scene = new SceneBridge(m_engine);
        m_commandsFileTime = io::invalidFileTime;
        m_lastProcessedId = 0;

        writeState();
        VG_INFO("[MCPBridge] enabled - watching \"%s\"", m_commandsPath.c_str());
    }

    //--------------------------------------------------------------------------------------
    void MCPBridge::Deinit()
    {
        UnregisterClasses();
        VG_SAFE_DELETE(m_scene);
        m_engine = nullptr;
    }

    //--------------------------------------------------------------------------------------
    void MCPBridge::Tick()
    {
        if (!m_enabled || !m_scene)
            return;

        VG_PROFILE_CPU("MCPBridge");

        io::FileAccessTime t = io::invalidFileTime;
        const bool haveFile = io::getLastWriteTime(m_commandsPath, &t);

        if (haveFile && t != m_commandsFileTime)
        {
            m_commandsFileTime = t;
            processCommandFile();
            writeState();
        }
        else if ((++m_tickCounter % 120) == 0)
        {
            // Periodic heartbeat so the MCP server can tell the engine is alive.
            writeState();
        }
    }

    //--------------------------------------------------------------------------------------
    void MCPBridge::processCommandFile()
    {
        string content;
        if (!io::readFile(m_commandsPath, content, false))
            return;

        core::u64 highestId = m_lastProcessedId;
        size_t start = 0;

        while (start < content.size())
        {
            size_t nl = content.find('\n', start);
            const size_t lineEnd = (nl == string::npos) ? content.size() : nl;
            std::string line = content.substr(start, lineEnd - start);
            start = (nl == string::npos) ? content.size() : nl + 1;

            // trim CR / spaces
            while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t'))
                line.pop_back();
            if (line.empty())
                continue;

            Json cmd;
            std::string err;
            if (!Json::parse(line, cmd, err))
            {
                VG_WARNING("[MCPBridge] skipping malformed command line: %s", err.c_str());
                continue;
            }

            const core::u64 id = (core::u64)cmd["id"].asNumber(0.0);
            if (id != 0 && id <= m_lastProcessedId)
                continue; // already handled

            const std::string tool = cmd["tool"].asString();
            const Json & args = cmd["args"];

            Json result = Json::makeObject();
            result.set("id", Json((double)id));
            result.set("tool", Json(tool));

            Json payload = m_scene->execute(tool, args);
            if (payload.has("error"))
            {
                result.set("ok", Json(false));
                result.set("error", payload["error"]);
            }
            else
            {
                result.set("ok", Json(true));
                result.set("data", payload);
            }

            m_recentResults.push_back(result);
            if (m_recentResults.size() > 50)
                m_recentResults.erase(m_recentResults.begin(), m_recentResults.begin() + (m_recentResults.size() - 50));

            if (id > highestId)
                highestId = id;

            VG_INFO("[MCPBridge] #%llu %s -> %s", (unsigned long long)id, tool.c_str(),
                    payload.has("error") ? "error" : "ok");
        }

        m_lastProcessedId = highestId;
    }

    //--------------------------------------------------------------------------------------
    void MCPBridge::writeState()
    {
        Json engineInfo = Json::makeObject();
        engineInfo.set("running", Json(true));
        engineInfo.set("playing", Json(m_engine ? m_engine->IsPlaying() : false));

        // Always republish the results: a heartbeat that blanked them would destroy an
        // answer the client has not read yet.
        Json results = Json::makeArray();
        for (const Json & r : m_recentResults)
            results.push(r);

        Json state = Json::makeObject();
        state.set("engine", engineInfo);
        state.set("lastProcessedId", Json((double)m_lastProcessedId));
        state.set("results", results);

        const std::string text = state.dump(2);

        // Atomic replace: write a temp file then MoveFileEx over the target.
        const std::string tmp = std::string(m_statePath.c_str()) + ".tmp";
        if (io::writeFile(tmp, text, false))
        {
            #if VG_WINDOWS
            if (!MoveFileExA(tmp.c_str(), m_statePath.c_str(), MOVEFILE_REPLACE_EXISTING))
                io::writeFile(m_statePath, text, false); // fallback: non-atomic
            #else
            io::writeFile(m_statePath, text, false);
            io::deleteFile(tmp, false);
            #endif
        }
    }
}
