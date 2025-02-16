using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Gfx : Project
    {
        public Gfx() : base("gfx", Type.StaticLibrary, Flags.GfxAPIDefines | Flags.GfxAPIInclude)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target, DependencySetting.OnlyBuildOrder);
        }
    }

    //[Sharpmake.Generate]
    //public class GfxTests : Project
    //{
    //    public GfxTests() : base("gfx.tests", Type.UnitTests)
    //    {
    //
    //    }
    //
    //    public override void ConfigureAll(Configuration conf, Target target)
    //    {
    //        base.ConfigureAll(conf, target);
    //        conf.AddPrivateDependency<Core>(target);
    //    }
    //}
}