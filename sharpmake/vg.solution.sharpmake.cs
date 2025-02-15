using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Solution : Sharpmake.Solution
    {
        public Solution() 
        {
            Name = "vgframework";
            AddTargets(Project.GetDefaultTargets(true));
        }

        [Configure()]
        public void ConfigureAll(Configuration conf, Target target)
        {
            conf.SolutionFileName = $"{Name}_{target.DevEnv}";
            conf.SolutionPath = Project.GetSolutionDirFromSharpmakeCsPath(SharpmakeCsPath);

            string platformName = Util.GetSimplePlatformString(target.GetPlatform());

            platformName += $" {target.Compiler.ToString()}";

            if (GraphicsAPI.None != target.GfxAPI)
                platformName += $" {target.GfxAPI.ToString()}";

            conf.PlatformName = platformName;
            conf.Name = $"{target.Optimization}";

            // "Solution folders" here for convenience (e.g. edit shaders or README.md in IDE)
            conf.AddProject<GitHub>(target, false, "data");
            conf.AddProject<ReadMe>(target, false, "data");
            conf.AddProject<Shaders>(target, false, "data");

            // All projects must be explicitly added here, deducing them from dependencies is not enough and will generate incorrect platform configurations
            conf.AddProject<Core>(target);
            conf.AddProject<CoreTests>(target);

            conf.AddProject<Gfx>(target);
            //conf.AddProject<GfxTests>(target);

            conf.AddProject<Renderer>(target);
            //conf.AddProject<RendererTests>(target);

            conf.AddProject<Engine>(target);
            //conf.AddProject<EngineTests>(target);

            conf.AddProject<Audio>(target);
            //conf.AddProject<AudioTests>(target);

            conf.AddProject<Physics>(target);
            //conf.AddProject<PhysicsTests>(target);

            conf.AddProject<Editor>(target);
            //conf.AddProject<EditorTests>(target);

            conf.AddProject<Game>(target);
            //conf.AddProject<GameTests>(target);

            conf.AddProject<Application>(target);
            //conf.AddProject<ApplicationTests>(target);
        }
    }
}