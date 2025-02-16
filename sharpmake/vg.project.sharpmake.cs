using Sharpmake;
using System;
using System.Collections.Generic;

namespace vg
{
    [Sharpmake.Generate]
    public abstract class Project : Sharpmake.Project
    {
        protected Type _Type;
        protected Flags _Flags;

        [Flags]
        public enum Flags
        {
            GfxAPIDefines = 0x00000001,
            GfxAPIInclude = 0x00000002,
            GfxAPILink    = 0x00000004
        };

        public enum Type
        {
            Executable,
            UnitTests,
            StaticLibrary,
            DynamicLibrary,
            Game,
            Data
        };

        public Project(string name, Type projectType, Flags flags = 0)
        {
            Name = name;
            _Type = projectType;
            _Flags = flags;

            bool useGfxAPIs = 0 != (flags & Flags.GfxAPIDefines);
            AddTargets(GetDefaultTargets(useGfxAPIs));

            switch(projectType)
            {
                default:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\src\{Name}";
                    break;

                case Type.UnitTests:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\tests\{Name}";
                    break;

                case Type.Game:
                    SourceRootPath = $"{SolutionDir}\\Projects\\{Name}";
                    break;

                case Type.Data:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\data\{Name}";
                    break;
            }

            if (projectType == Type.DynamicLibrary || projectType == Type.Game)
                SourceFilesExtensions.Add(".def");
        }

        public static Target[] GetDefaultTargets(bool useGfxAPIs = false)
        {
            var targets = GetWindowsTargets(useGfxAPIs); 
            return targets.ToArray();
        }

        public static List<Target> GetWindowsTargets(bool useGfxAPIs = false)
        {
            var targets = new List<Target>();

            GraphicsAPI gfxAPIs;

            if (useGfxAPIs)
                gfxAPIs = GraphicsAPI.Vulkan | GraphicsAPI.DX12;
            else
                gfxAPIs = GraphicsAPI.None;
            
            targets.Add(new Target(Platform.win64 , DevEnv.vs2022, Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final)
            {
                Compiler = Compiler.MSVC| Compiler.ClangCL,
                GfxAPI = gfxAPIs
            });

            targets.Add(new Target(Platform.arm64ec, DevEnv.vs2022, Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final)
            {
                Compiler = Compiler.MSVC ,
                GfxAPI = gfxAPIs
            });

            return targets;
        }

        [Configure]
        public virtual void ConfigureVSAdvanced(Configuration conf, Target target)
        {
            conf.Options.Add(Options.Vc.General.CharacterSet.Unicode);
        }

        private void ConfigureVSEditAndContinue(Configuration conf, Target target, bool enable)
        {
            if (enable)
            {
                conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabaseEnC);
                conf.Options.Add(Options.Vc.General.WholeProgramOptimization.Disable);
                conf.Options.Add(Options.Vc.Linker.LinkTimeCodeGeneration.Default);
                conf.Options.Add(Options.Vc.Linker.Incremental.Enable);
            }
            else
            {
                conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabase);
                conf.Options.Add(Options.Vc.General.WholeProgramOptimization.LinkTime);
                conf.Options.Add(Options.Vc.Linker.LinkTimeCodeGeneration.UseLinkTimeCodeGeneration);
                conf.Options.Add(Options.Vc.Linker.Incremental.Disable);
            }
        }
        public string SolutionDir
        {
            get
            {
                return GetSolutionDirFromSharpmakeCsPath(SharpmakeCsPath);
            }
        }

        public static string GetSolutionDirFromSharpmakeCsPath(string sharpmakeCsPath)
        {
            return $"{sharpmakeCsPath}\\..";
        }
    
        [Configure]
        public virtual void ConfigureVSCompilerGeneral(Configuration conf, Target target)
        {
            if (target.Compiler == Compiler.ClangCL)
            {
                conf.Options.Add(Options.Vc.General.PlatformToolset.ClangCL);
                conf.AdditionalCompilerOptions.Add("-mlzcnt -mfma -mf16c -mavx ");
            }

            conf.Options.Add(Options.Vc.General.WarningLevel.Level3);
            conf.Options.Add(Options.Vc.General.TreatWarningsAsErrors.Disable);
            conf.Options.Add(Options.Vc.General.DiagnosticsFormat.ColumnInfo);

            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP17);
            conf.Options.Add(Options.Vc.Compiler.MultiProcessorCompilation.Enable);
            conf.Options.Add(Options.Vc.Compiler.FunctionLevelLinking.Enable);
            conf.Options.Add(Options.Vc.Compiler.ConformanceMode.Enable);
            conf.Options.Add(Options.Vc.Compiler.Intrinsic.Enable);
            conf.Options.Add(Options.Vc.Compiler.Exceptions.Enable);
            conf.Options.Add(Options.Vc.Compiler.BufferSecurityCheck.Enable);
            conf.Options.Add(Options.Vc.Compiler.FloatingPointModel.Precise);
            conf.Options.Add(Options.Vc.Compiler.RTTI.Enable);

            switch (target.Platform)
            {
                default:
                    if (target.Compiler == Compiler.MSVC)
                        conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions);
                    break;

                case Platform.arm64ec:
                    //conf.Options.Add(Options.Vc.Compiler.EnhancedInstructionSet.AdvancedVectorExtensions); // ignoring unknown option '/arch:AVX'
                    break;
            }

            switch (target.Optimization)
            {
                case Optimization.Debug:
                    conf.Defines.Add("VG_DEBUG");
                    conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
                    conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
                    conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
                    ConfigureVSEditAndContinue(conf, target, true);
                    break;

                case Optimization.Development:
                    conf.Defines.Add("VG_DEVELOPMENT");
                    conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
                    conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.Neither);
                    conf.Options.Add(Options.Vc.Compiler.Optimization.Disable);
                    ConfigureVSEditAndContinue(conf, target, true);
                    break;

                case Optimization.Release:
                    conf.Defines.Add("VG_RELEASE");
                    conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
                    conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
                    conf.Options.Add(Options.Vc.Compiler.Optimization.MaximizeSpeed);
                    ConfigureVSEditAndContinue(conf, target, false);
                    break;

                case Optimization.Final:
                    conf.Defines.Add("VG_FINAL");
                    conf.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
                    conf.Options.Add(Options.Vc.Compiler.FavorSizeOrSpeed.FastCode);
                    conf.Options.Add(Options.Vc.Compiler.Optimization.MaximizeSpeed);
                    ConfigureVSEditAndContinue(conf, target, false);
                    break;
            }

            switch (_Type)
            {
                case Type.Data:
                    break;

                case Type.StaticLibrary:
                    conf.Output = Configuration.OutputType.Utility;
                    conf.Output = Configuration.OutputType.Lib;
                    conf.TargetPath = $"{SolutionDir}\\build\\lib\\{target.Platform}\\{target.Name}";
                    break;

                case Type.DynamicLibrary:
                    conf.Output = Configuration.OutputType.Dll;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    conf.TargetPath = $"{SolutionDir}\\build\\bin\\{target.Platform}\\{target.Name}";
                    conf.ModuleDefinitionFile = $"{Name}.def";
                    break;

                case Type.Game:
                    conf.Output = Configuration.OutputType.Dll;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    conf.TargetPath = $"{SolutionDir}\\build\\bin\\{target.Platform}\\{target.Name}";
                    conf.ModuleDefinitionFile = $"src\\{Name}.def";
                    break;

                case Type.Executable:
                    conf.Output = Configuration.OutputType.Exe;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    conf.TargetPath = $"{SolutionDir}";
                    break;

                case Type.UnitTests:
                    conf.Output = Configuration.OutputType.Exe;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
                    conf.TargetPath = $"{SolutionDir}\\build\\tests\\{target.Platform}\\{target.Name}";
                    break;
            }

            conf.IntermediatePath = $"{SolutionDir}\\tmp\\{Name}\\{target.Platform}\\{target.Name}";

            switch (_Type)
            {
                default:
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\src");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\fmt\include");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data");
                    break;

                case Type.Game:
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\src");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern");
                    conf.IncludePaths.Add($"{SolutionDir}\\Projects\\{Name}\\src");
                    break;

                case Type.Data:
                    break;

                case Type.UnitTests:
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\tests");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\googletest\googletest");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\googletest\googletest\include");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data");
                    break;
            }
        }
    
        [Configure]
        public virtual void ConfigureAll(Configuration conf, Target target)
        {
            conf.ProjectPath = $@"[project.SharpmakeCsPath]\projects\{Name}";
            conf.ProjectFileName = $@"[project.Name]_[target.DevEnv]";

            switch (_Type)
            {
                default:
                    conf.PrecompHeader = $@"{Name}/Precomp.h";
                    conf.PrecompSource = $@"{Name}/Precomp.cpp";
                    break;

                case Type.Game:
                    conf.PrecompHeader = "Precomp.h";
                    conf.PrecompSource = "Precomp.cpp";
                    break;

                case Type.Data:
                    conf.PrecompHeader = string.Empty;
                    conf.PrecompSource = string.Empty;
                    break;
            }

            switch (target.GfxAPI)
            {
                case GraphicsAPI.DX12:
                    conf.Defines.Add("VG_DX12");

                    if (0 != (_Flags & Flags.GfxAPILink))
                    {
                        conf.LibraryFiles.Add($"{SolutionDir}\\extern\\winpixeventruntime\\bin\\x64\\WinPixEventRuntime.lib");
                        conf.LibraryFiles.Add($"{SolutionDir}\\extern\\dxc\\lib\\x64\\dxcompiler.lib");
                        conf.LibraryFiles.Add("d3d12.lib");
                        conf.LibraryFiles.Add("dxgi.lib");
                        conf.LibraryFiles.Add("dxguid.lib");
                    }

                    break;

                case GraphicsAPI.Vulkan:
                    conf.Defines.Add("VG_VULKAN");

                    if (0 != (_Flags & Flags.GfxAPIInclude))
                    {
                        conf.IncludePaths.Add("$(VULKAN_SDK)\\include");
                    }

                    if (0 != (_Flags & Flags.GfxAPILink))
                    {
                        conf.LibraryFiles.Add($"{SolutionDir}\\extern\\dxc\\lib\\x64\\dxcompiler.lib");
                        conf.LibraryFiles.Add("dxgi.lib");
                        conf.LibraryFiles.Add("$(VULKAN_SDK)\\Lib\\vulkan-1.lib");
                    }

                    break;
            }

            // This is a temp define used during the transition to sharpmake, to search for DLL in correct path
            // e.g.: Load engine DLL from "build\bin\win64\debug msvc" instead of "build\bin\x64\debug"
            conf.Defines.Add("VG_SHARPMAKE");
        }
    }
    
}