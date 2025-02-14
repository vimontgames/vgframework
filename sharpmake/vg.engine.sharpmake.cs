using Sharpmake;
using System;
using System.Collections.Generic;

namespace vg
{
    [Sharpmake.Generate]
    public class Engine : Project
    {
        public Engine() : base("engine", Type.DynamicLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target);
            conf.LibraryFiles.Add("dinput8.lib", "dxguid.lib");
        }
    }
}