using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Editor : Project
    {
        public Editor() : base("editor", Type.DynamicLibrary)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");

            conf.AddPrivateDependency<Core>(target);
        }
    }
}