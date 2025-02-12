using Sharpmake;
using System.Collections.Generic;

namespace vg
{
    [Sharpmake.Generate]
    public class Application : Project
    {
        public Application() : 
            base("application", Type.Executable, Flags.GfxAPIDefines)
        {
  
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            // old format: "VGFramework_x64_Debug_DX12"
            // New format: "VGFramework_win64_MSVC_DX12_Debug" (to match Visual studio combo order)
            conf.TargetFileName = $"VGFramework_{target.Platform}_{target.Compiler}_{target.GfxAPI}_{target.Optimization}";

            conf.AddPublicDependency<Core>(target);
        }
    }
}