using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class VGFrameworkSolution : Sharpmake.Solution
    {
        public VGFrameworkSolution()
        {
            Name = "vgframework";

            AddTargets(new Target(
                    Platform.win64,
                    DevEnv.vs2022,
                    Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final
            ));
        }

        [Configure()]
        public void ConfigureAll(Configuration conf, Target target)
        {
            conf.SolutionFileName = "[solution.Name]_[target.DevEnv]_[target.Platform]";
            conf.SolutionPath = @"[solution.SharpmakeCsPath]\projects";

            conf.AddProject<ApplicationProject>(target);
        }
    }
}