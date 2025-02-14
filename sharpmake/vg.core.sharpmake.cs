using Sharpmake;
using System.Collections.Generic;

namespace vg
{
    [Sharpmake.Generate]
    public class Core : Project
    {
        public Core() : base("core", Type.StaticLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }

    [Sharpmake.Generate]
    public class CoreTests : Project
    {
        public CoreTests() : base("core.tests", Type.UnitTests)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target);
        }
    }
}