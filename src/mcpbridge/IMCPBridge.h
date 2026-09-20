#pragma once

#include "core/IPlugin.h"

namespace vg
{
    namespace engine
    {
        class IEngine;
    }

    namespace mcpbridge
    {
        //--------------------------------------------------------------------------------------
        // Additive plugin. Bridges an external MCP server (Claude Code) to the active scene
        // for level-design tooling: list / get / set transform, spawn (clone) objects.
        //
        // Transport V1 (see docs/architecture.md): the plugin polls "<cwd>/mcp/commands.jsonl"
        // and writes "<cwd>/mcp/state.json". Nothing is done unless the environment variable
        // VG_MCP_BRIDGE is set (=> zero cost / zero behaviour change on a normal run).
        //--------------------------------------------------------------------------------------
        class IMCPBridge : public core::IPlugin
        {
        public:
            IMCPBridge(const core::string & _name, core::IObject * _parent) :
                core::IPlugin(_name, _parent)
            {
            }

            // _engine : borrowed, not owned. Must outlive the bridge.
            virtual void    Init        (engine::IEngine * _engine, core::Singletons & _singletons) = 0;
            virtual void    Deinit      () = 0;

            // Poll the command file and apply pending commands. Call once per frame.
            virtual void    Tick        () = 0;

            // True when VG_MCP_BRIDGE was set at Init() time.
            virtual bool    IsEnabled   () const = 0;
        };
    }
}
