using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Engine : Project
    {
        public Engine() : base("engine", Type.DynamicLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target);
            conf.LibraryFiles.Add("dinput8.lib", "dxguid.lib");

            // Compile the additive MCP bridge hook (src/mcpbridge, loaded at runtime only
            // when the VG_MCP_BRIDGE env var is set). Remove this line to strip it entirely.
            conf.Defines.Add("VG_ENABLE_MCPBRIDGE");
        }
    }
}