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

            // Explicitly link with the full path since the project depends on a non-Gfx API lib.
            // This ensures the linker searches in the correct directory.
            //conf.AddPrivateDependency<Core>(target);
            conf.LibraryFiles.Add($"{SolutionDir}\\build\\lib\\{target.Platform}\\{target.Optimization} {target.Compiler}\\core.lib");

            conf.AddPrivateDependency<Gfx>(target);
        }
    }
}