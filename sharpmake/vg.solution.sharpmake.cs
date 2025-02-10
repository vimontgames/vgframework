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
            conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\projects";

            string platformName = target.Platform.ToString();

            switch(target.Platform)
            {
                case Platform.win64:
                    platformName = "Win64";
                    break;

                default:
                    platformName = target.Platform.ToString();
                    break;
            };

            if (GraphicsAPI.None != target.GfxAPI)
                platformName += $" {target.GfxAPI.ToString()}";

            platformName += $" {target.Compiler.ToString()}";

            conf.PlatformName = platformName;
            conf.Name = $"{target.Optimization}";

            conf.AddProject<Application>(target);
        }
    }
}