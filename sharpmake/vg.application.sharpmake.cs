using Sharpmake;

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
            conf.AddPrivateDependency<Engine>(target, DependencySetting.OnlyBuildOrder);
            conf.AddPrivateDependency<Renderer>(target, DependencySetting.OnlyBuildOrder);
            conf.AddPrivateDependency<Audio>(target, DependencySetting.OnlyBuildOrder);
            conf.AddPrivateDependency<Physics>(target, DependencySetting.OnlyBuildOrder);
            conf.AddPrivateDependency<Game>(target, DependencySetting.OnlyBuildOrder);
            conf.AddPrivateDependency<Editor>(target, DependencySetting.OnlyBuildOrder);

            conf.EventPreBuild.Add("$(SolutionDir)script\\generate_version_header_vs2022.bat");
            conf.EventPreBuildDescription = "Generate 'commit.h' file using current git revision";
        }
    }
}