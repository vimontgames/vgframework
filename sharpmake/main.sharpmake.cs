using Sharpmake;

namespace vg
{
    public abstract class BaseProject : Project
    {
        int Type { get; }

        public BaseProject(string name, int type)
        {
            Name = name;
            Type = type;

            AddTargets(new Target(
                    Platform.win64,
                    DevEnv.vs2022,
                    Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final
            ));

            switch (Type)
            {
                case 0:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\src\{Name}";
                    break;
                case 1:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\src\{Name}";
                    break;
                case 2:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\projects\{Name}\src";
                    break;
                case 3:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\src\{Name}";
                    break;
            }
        }

        [Configure]
        public virtual void ConfigureVSAdvanced(Configuration conf, Target target)
        {
            conf.Options.Add(Sharpmake.Options.Vc.General.CharacterSet.Unicode);
            conf.Options.Add(Sharpmake.Options.Vc.General.WholeProgramOptimization.LinkTime);
        }

        [Configure]
        public virtual void ConfigureVSCompilerGeneral(Configuration conf, Target target)
        {
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\src");
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern");

            conf.Options.Add(Sharpmake.Options.Vc.General.WarningLevel.Level3);
            conf.Options.Add(Sharpmake.Options.Vc.General.TreatWarningsAsErrors.Disable);
            conf.Options.Add(Sharpmake.Options.Vc.General.DiagnosticsFormat.ColumnInfo);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.MultiProcessorCompilation.Enable);
        }

        [Configure]
        public virtual void ConfigureAll(Configuration conf, Target target)
        {
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Optimization.MaximizeSpeed);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.FunctionLevelLinking.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Intrinsic.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.ConformanceMode.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.CppLanguageStandard.CPP17);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Inline.Disable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Intrinsic.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.FunctionLevelLinking.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions);

            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);    
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.FloatingPointModel.Precise);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.ConformanceMode.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.RTTI.Enable);

            conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
            conf.ProjectPath = $@"[project.SharpmakeCsPath]\projects\{Name}";
            // conf.IntermediatePath = $@"[project.SharpmakeCsPath]\projects\obj\{Name}\[target.Platform]\[target.Configuration]";

            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP17);

            switch(Type)
            {
                case 0:
                    conf.Output = Configuration.OutputType.Dll;
                    conf.PrecompHeader = $@"{Name}/Precomp.h";
                    conf.PrecompSource = $@"{Name}/Precomp.cpp";
                    break;
                case 1:
                    conf.Output = Configuration.OutputType.Lib;
                    conf.PrecompHeader = $@"{Name}/Precomp.h";
                    conf.PrecompSource = $@"{Name}/Precomp.cpp";
                    break;
                case 2:
                    conf.Output = Configuration.OutputType.Dll;
                    conf.PrecompHeader = $@"Precomp.h";
                    conf.PrecompSource = $@"Precomp.cpp";
                    conf.IncludePaths.Add($@"[project.SharpmakeCsPath]\..\projects\{Name}\src");
                    break;
                case 3:
                    conf.Output = Configuration.OutputType.Exe;
                    conf.PrecompHeader = $@"Precomp.h";
                    conf.PrecompSource = $@"Precomp.cpp";
                    break;
            }
        }
    }

    [Sharpmake.Generate]
    public abstract class BaseLibProject : BaseProject
    {
        public BaseLibProject(string name, bool isStatic)
            : base(name, isStatic ? 1 : 1)
        {
        }
    }

    [Sharpmake.Generate]
    public abstract class BaseGameProject : BaseProject
    {
        public BaseGameProject(string name)
            : base(name, 2)
        {
        }
    }

    [Sharpmake.Generate]
    public abstract class BaseApplicationProject : BaseProject
    {
        public BaseApplicationProject(string name)
            : base(name, 3)
        {
        }
    }

    [Sharpmake.Generate]
    public class CoreLibProject : BaseLibProject
    {
        public CoreLibProject()
            : base("core", true)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\fmt\include");

            conf.Defines.Add("_CRTDBG_MAP_ALLOC");
        }
    }

    [Sharpmake.Generate]
    public class AudioLibProject : BaseLibProject
    {
        public AudioLibProject()
            : base("audio", false)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\soloud\include");

            // conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class PhysicsLibProject : BaseLibProject
    {
        public PhysicsLibProject()
            : base("physics", false)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\JoltPhysics");

            conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class GfxLibProject : BaseLibProject
    {
        public GfxLibProject()
            : base("gfx", true)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data");
            conf.IncludePaths.Add(@"C:\VulkanSDK\1.4.304.0\Include");

            conf.Defines.Add("VG_VULKAN");

            conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class RendererLibProject : BaseLibProject
    {
        public RendererLibProject()
            : base("renderer", true)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data\Shaders");
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");
            //conf.IncludePaths.Add(@"C:\VulkanSDK\1.4.304.0\Include");

            conf.Defines.Add("VG_VULKAN");

            conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<GfxLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class EngineLibProject : BaseLibProject
    {
        public EngineLibProject()
            : base("engine", true)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data\Shaders");
            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");
            //conf.IncludePaths.Add(@"C:\VulkanSDK\1.4.304.0\Include");

            //conf.Defines.Add("VG_VULKAN");

            conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<RendererLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class EditorLibProject : BaseLibProject
    {
        public EditorLibProject()
            : base("editor", true)
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data\Shaders");
            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");
            //conf.IncludePaths.Add(@"C:\VulkanSDK\1.4.304.0\Include");

            //conf.Defines.Add("VG_VULKAN");

            conf.AddPublicDependency<GameProjectProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class GameProjectProject : BaseGameProject
    {
        public GameProjectProject()
            : base("game")
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data\Shaders");
            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");
            //conf.IncludePaths.Add(@"C:\VulkanSDK\1.4.304.0\Include");

            //conf.Defines.Add("VG_VULKAN");

            conf.AddPublicDependency<CoreLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<EngineLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<GfxLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<RendererLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    [Sharpmake.Generate]
    public class ApplicationProject : BaseApplicationProject
    {
        public ApplicationProject()
            : base("application")
        {
        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);

            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data\Shaders");
            //conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\imgui");
            //conf.IncludePaths.Add(@"C:\VulkanSDK\1.4.304.0\Include");

            conf.Defines.Add("VG_VULKAN");

            conf.AddPublicDependency<AudioLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
            conf.AddPublicDependency<EditorLibProject>(target, DependencySetting.LibraryPaths | DependencySetting.IncludePaths | DependencySetting.LibraryFiles);
        }
    }

    public static class Main
    {
        [Sharpmake.Main]
        public static void SharpmakeMain(Sharpmake.Arguments arguments)
        {
            KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.Latest);
            arguments.Generate<VGFrameworkSolution>();
        }
    }
}