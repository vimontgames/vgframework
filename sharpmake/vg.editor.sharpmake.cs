using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Editor : Project
    {
        public Editor() : base("editor", Type.DynamicLibrary, Flags.EditorOnly)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            if (target.Optimization == Optimization.Final)
            {
                conf.IsExcludedFromBuild = true;
            }

            base.ConfigureAll(conf, target);
            
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");

            conf.AddPrivateDependency<Core>(target);
        }
    }
}