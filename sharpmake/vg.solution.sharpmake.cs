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

            string platformName = target.Platform.ToString();

            switch (target.Platform)
            {
                case Platform.win64:
                    platformName = "Win64";
                    break;

                default:
                    platformName = target.Platform.ToString();
                    break;
            };

            platformName += $" {target.Compiler.ToString()}";

            if (GraphicsAPI.None != target.GfxAPI)
                platformName += $" {target.GfxAPI.ToString()}";

            conf.PlatformName = platformName;
            conf.Name = $"{target.Optimization}";

            conf.AddProject<GitHub>(target, false, "data");
            conf.AddProject<ReadMe>(target, false, "data");
            conf.AddProject<Shaders>(target, false, "data");

            conf.AddProject<Application>(target);
            conf.AddProject<CoreTests>(target);
        }
    }
}