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
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\..\";

            string platformName = Util.GetSimplePlatformString(target.GetPlatform());

            platformName += $" {target.Compiler.ToString()}";

            if (GraphicsAPI.None != target.GfxAPI)
                platformName += $" {target.GfxAPI.ToString()}";

            conf.PlatformName = platformName;
            conf.Name = $"{target.Optimization}";

            // "Solution folders" here for conveniency (e.g. edit shaders or README.md in IDE)
            conf.AddProject<GitHub>(target, false, "data");
            conf.AddProject<ReadMe>(target, false, "data");
            conf.AddProject<Shaders>(target, false, "data");

            // Main application (editor & standalone)
            conf.AddProject<Application>(target);
            conf.AddProject<Core>(target);
            conf.AddProject<CoreTests>(target);
            conf.AddProject<Engine>(target);
        }
    }
}