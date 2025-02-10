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

            SourceRootPath = $@"[project.SharpmakeCsPath]\..\src\{Name}";
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

            Compiler compiler;
            compiler = Compiler.MSVC;

            targets.Add(new Target(Platform.win64, DevEnv.vs2022, Optimization.Debug | Optimization.Development | Optimization.Release | Optimization.Final)
            {
                Compiler = compiler,
                GfxAPI = gfxAPIs
            });

            return targets;
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
            conf.IncludePaths.Add(@"[project.SharpmakeCsPath]\..\extern\fmt\include");

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
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.Exceptions.Enable);  
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.BufferSecurityCheck.Enable);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.FloatingPointModel.Precise);
            conf.Options.Add(Sharpmake.Options.Vc.Compiler.RTTI.Enable);

            // Runtime
            switch(target.Optimization)
            {
                case Optimization.Debug:
                    conf.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);
                    break;

                default:
                case Optimization.Development:
                case Optimization.Release:
                case Optimization.Final:
                    conf.Options.Add(Sharpmake.Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDLL);
                    break;
            }

            // Optimization level
            switch(target.Optimization)
            {
                case Optimization.Debug:
                    conf.Defines.Add("VG_DEBUG");
                    break;

                case Optimization.Development:
                    conf.Defines.Add("VG_DEVELOPMENT");
                    break;

                case Optimization.Release:
                    conf.Defines.Add("VG_RELEASE");
                    break;

                case Optimization.Final:
                    conf.Defines.Add("VG_FINAL");
                    break;
            }
    
            conf.ProjectFileName = "[project.Name]_[target.DevEnv]_[target.Platform]";
            conf.ProjectPath = $@"[project.SharpmakeCsPath]\projects\{Name}";
            // conf.IntermediatePath = $@"[project.SharpmakeCsPath]\projects\obj\{Name}\[target.Platform]\[target.Configuration]";
    
            conf.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP17);

            // Default path for precomp
            conf.PrecompHeader = $@"{Name}/Precomp.h";
            conf.PrecompSource = $@"{Name}/Precomp.cpp";

            switch (_Type)
            {
                case Type.Data:
                    Debug.Assert(false);
                    break;

                case Type.StaticLibrary:
                    conf.Output = Configuration.OutputType.Lib;
                    conf.PrecompHeader = $@"{Name}/Precomp.h";
                    conf.PrecompSource = $@"{Name}/Precomp.cpp";
                    break;

                case Type.DynamicLibrary:
                    conf.Output = Configuration.OutputType.Dll;
                    conf.PrecompHeader = $@"{Name}/Precomp.h";
                    conf.PrecompSource = $@"{Name}/Precomp.cpp";
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    break;

                case Type.Executable:
                    conf.Output = Configuration.OutputType.Exe;
                    conf.PrecompHeader = $@"Precomp.h";
                    conf.PrecompSource = $@"Precomp.cpp";
                    conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                    break;

                    //case 2:
                    //    conf.Output = Configuration.OutputType.Dll;
                    //    conf.PrecompHeader = $@"Precomp.h";
                    //    conf.PrecompSource = $@"Precomp.cpp";
                    //    conf.IncludePaths.Add($@"[project.SharpmakeCsPath]\..\projects\{Name}\src");
                    //    break;         
            }

            switch(target.GfxAPI)
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