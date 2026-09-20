#pragma once

#include "mcpbridge/IMCPBridge.h"
#include "mcpbridge/Json.h"
#include "core/Singleton/Singleton.h"
#include "core/File/File.h"
#include <string>
#include <vector>

namespace vg::mcpbridge
{
    class SceneBridge;

    class MCPBridge final : public IMCPBridge, public core::Singleton<MCPBridge>
    {
    public:
        using super = IMCPBridge;

                        MCPBridge           (const core::string & _name, core::IObject * _parent);
                        ~MCPBridge          ();

        const char *    GetClassName        () const final override { return "MCPBridge"; }

        bool            RegisterClasses     () final override;
        bool            UnregisterClasses   () final override;
        static bool     registerProperties  (core::IClassDesc & _desc);

        void            Init                (engine::IEngine * _engine, core::Singletons & _singletons) final override;
        void            Deinit              () final override;
        void            Tick                () final override;
        bool            IsEnabled           () const final override { return m_enabled; }

    private:
        void            processCommandFile  ();
        void            writeState          ();

        engine::IEngine *           m_engine = nullptr;
        SceneBridge *               m_scene = nullptr;
        bool                        m_enabled = false;

        core::string                m_dir;              // "<cwd>/mcp"
        core::string                m_commandsPath;     // "<cwd>/mcp/commands.jsonl"
        core::string                m_statePath;        // "<cwd>/mcp/state.json"

        core::io::FileAccessTime    m_commandsFileTime = core::io::invalidFileTime;
        core::u64                   m_lastProcessedId = 0;
        core::u32                   m_tickCounter = 0;

        // Rolling window of recent command results. Always serialized into state.json,
        // including on a heartbeat write, so a slow client never loses an answer.
        std::vector<Json>           m_recentResults;
    };
}
