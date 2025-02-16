using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Renderer : Project
    {
        public Renderer() : base("renderer", Type.DynamicLibrary, Flags.GfxAPIDefines | Flags.GfxAPIInclude | Flags.GfxAPILink)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data\Shaders");
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");

            conf.AddPrivateDependency<Core>(target);
            conf.AddPrivateDependency<Gfx>(target);
        }
    }
}