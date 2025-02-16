using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Physics : Project
    {
        public Physics() : base("physics", Type.DynamicLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target);
            conf.IncludePaths.Add($"{SolutionDir}\\extern\\JoltPhysics");
        }
    }
}