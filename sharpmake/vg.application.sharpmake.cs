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
            conf.TargetFileName = $"VGFramework_{target.Platform}_{target.Compiler}_{target.GfxAPI}_{target.Optimization}";
            conf.AddPrivateDependency<Core>(target);
            conf.AddPublicDependency<Engine>(target, DependencySetting.OnlyBuildOrder);
        }
    }
}