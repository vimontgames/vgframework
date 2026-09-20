using Sharpmake;

namespace vg
{
    // Additive module — MCP bridge for level-design tooling (see docs/architecture.md).
    // Auto-included by main.sharpmake.cs ("vg.*.sharpmake.cs"); must also be added
    // explicitly in vg.solution.sharpmake.cs (ConfigureAll).
    [Sharpmake.Generate]
    public class MCPBridge : Project
    {
        public MCPBridge() : base("mcpbridge", Type.DynamicLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target);
        }
    }
}
