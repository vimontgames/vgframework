using Sharpmake;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.Intrinsics.X86;

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

                case Type.Data:
                    SourceRootPath = $@"[project.SharpmakeCsPath]\..\data\{Name}";
                    break;
            }
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
                Compiler = Compiler.MSVC ,
                GfxAPI = gfxAPIs
            });

            targets.Add(new Target(Platform.win64, DevEnv.vs2022, Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final)
            {
                Compiler = Compiler.ClangCL,
                GfxAPI = gfxAPIs
            });

            targets.Add(new Target(Platform.arm64ec, DevEnv.vs2022, Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final)
            {
                Compiler = Compiler.MSVC ,
                GfxAPI = gfxAPIs
            });

            //targets.Add(new Target(Platform.arm64ec, DevEnv.vs2022, Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final)
            //{
            //    Compiler = Compiler.ClangCL,
            //    GfxAPI = gfxAPIs
            //});

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
                conf.Options.Add(Options.Vc.Linker.Incremental.Enable);
            }
            else
            {
                conf.Options.Add(Options.Vc.General.DebugInformation.ProgramDatabase);
                conf.Options.Add(Options.Vc.General.WholeProgramOptimization.LinkTime);
                conf.Options.Add(Options.Vc.Linker.Incremental.Disable);
            }
        }
    
        [Configure]
        public virtual void ConfigureVSCompilerGeneral(Configuration conf, Target target)
        {
            if (target.Compiler == Compiler.ClangCL)
                conf.Options.Add(Options.Vc.General.PlatformToolset.ClangCL);

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
                    break;

                case Type.DynamicLibrary:
                    conf.Output = Configuration.OutputType.Dll;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    break;

                case Type.Executable:
                    conf.Output = Configuration.OutputType.Exe;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    conf.TargetPath = @"[project.SharpmakeCsPath]\.."; // "[conf.ProjectPath]\\output\\[target.Platform]\\[conf.Name]"
                    break;

                case Type.UnitTests:
                    conf.Output = Configuration.OutputType.Exe;
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
                    break;
            }

            switch (_Type)
            {
                default:
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\src");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\fmt\include");
                    conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\data");
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
            //conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
            conf.ProjectPath = $@"[project.SharpmakeCsPath]\projects\{Name}";
            conf.ProjectFileName = $@"[project.Name]_[target.DevEnv]";
            // conf.IntermediatePath = $@"[project.SharpmakeCsPath]\projects\obj\{Name}\[target.Platform]\[target.Configuration]";

            // Precompiled header path

            switch (_Type)
            {
                default:
                    conf.PrecompHeader = $@"{Name}/Precomp.h";
                    conf.PrecompSource = $@"{Name}/Precomp.cpp";
                    break;

                //case Type.UnitTests:
                //    conf.PrecompHeader = $@"Precomp.h";
                //    conf.PrecompSource = $@"Precomp.cpp";
                //    break;

                case Type.Data:
                    conf.PrecompHeader = string.Empty;
                    conf.PrecompSource = string.Empty;
                    break;
            }

            switch (target.GfxAPI)
            {
                case GraphicsAPI.DX12:
                    conf.Defines.Add("VG_DX12");
                    break;

                case GraphicsAPI.Vulkan:
                    conf.Defines.Add("VG_VULKAN");
                    break;
            }
        }
    }
    
}