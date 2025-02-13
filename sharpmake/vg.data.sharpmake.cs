using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Shaders : Project
    {
        public Shaders() : 
            base("shaders", Type.Data)
        {
            SourceRootPath = $@"[project.SharpmakeCsPath]\..\data\Shaders";
            SourceFilesExtensions.Add(".hlsl"); 
            SourceFilesExtensions.Add(".hlsli");
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            
        }
    }

    [Sharpmake.Generate]
    public class GitHub : Project
    {
        public GitHub() :
            base("github", Type.Data)
        {
            SourceRootPath = $@"[project.SharpmakeCsPath]\..\.github";
            SourceFilesExtensions.Add(".yml");
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }

    [Sharpmake.Generate]
    public class ReadMe : Project
    {
        public ReadMe() :
            base("readme", Type.Data)
        {
            SourceRootPath = $@"[project.SharpmakeCsPath]\..\.";
            
            SourceFilesExtensions.Add(".md");
            SourceFilesExcludeRegex.Add(@".*\\data(\.*)?");
            SourceFilesExcludeRegex.Add(@".*\\extern(\.*)?");
            //SourceFilesExcludeRegex.Add(@".*\\projects(\.*)?");
            
            SourceFilesExcludeRegex.Add(@".*\.asm\.*");
            SourceFilesExcludeRegex.Add(@".*\.nasm\.*");
            SourceFilesExcludeRegex.Add(@".*\.h\.*");
            SourceFilesExcludeRegex.Add(@".*\.hpp\.*");
            SourceFilesExcludeRegex.Add(@".*\.inl\.*");
            SourceFilesExcludeRegex.Add(@".*\.cpp\.*");
            SourceFilesExcludeRegex.Add(@".*\.hlsl\.*");
            SourceFilesExcludeRegex.Add(@".*\.hlsli\.*");
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
        }
    }
}