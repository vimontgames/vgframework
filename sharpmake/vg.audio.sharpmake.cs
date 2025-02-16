using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Audio : Project
    {
        public Audio() : base("audio", Type.DynamicLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add($"{SolutionDir}\\extern\\soloud\\include");

            conf.AddPrivateDependency<Core>(target);
        }
    }
}