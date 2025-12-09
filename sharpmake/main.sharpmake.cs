using Sharpmake;
using System;
using System.Diagnostics;

[module: Sharpmake.Include("vg.*.sharpmake.cs")]

namespace vg
{
    public static class Globals
    {
        public static bool win64 = true;
        public static bool arm64ec = true;

        [CommandLine.Option("win64", @"generate win64 platform targets")]
        public static void CommandLine_win64(bool value)
        {

            win64 = value;
        }

        [CommandLine.Option("arm64ec", @"generate ar64ec platform targets")]
        public static void CommandLine_arm64ec(bool value)
        {
            arm64ec = value;
        }

        public static bool msvc = true;
        public static bool clang = true;

        [CommandLine.Option("msvc", @"generate msvc compiler targets")]
        public static void CommandLine_msvc(bool value)
        {
            
            msvc = value;
        }

        [CommandLine.Option("clang", @"generate clang compiler targets")]
        public static void CommandLine_clang(bool value)
        {
            clang = value;
        }

        public static bool dx12 = true;
        public static bool vulkan = true;

        [CommandLine.Option("dx12", @"generate dx12 API targets")]
        public static void CommandLine_dx12(bool value)
        {
            dx12 = value;
        }

        [CommandLine.Option("vulkan", @"generate vulkan API targets")]
        public static void CommandLine_vulkan(bool value)
        {
            vulkan = value;
        }
    }
    public static class Main
    {
        [Sharpmake.Main]
        public static void SharpmakeMain(Sharpmake.Arguments arguments)
        {
            CommandLine.ExecuteOnType(typeof(Globals));

            Console.WriteLine($"win64 platform targets are {(Globals.win64 ? "enabled" : "disabled")}");
            Console.WriteLine($"arm64ec platform targets are {(Globals.arm64ec ? "enabled" : "disabled")}");

            Console.WriteLine($"msvc compiler targets are {(Globals.msvc ? "enabled" : "disabled")}");
            Console.WriteLine($"clang compiler targets are {(Globals.clang ? "enabled" : "disabled")}");

            Console.WriteLine($"dx12 API targets are {(Globals.dx12 ? "enabled" : "disabled")}");
            Console.WriteLine($"vulkan API targets are {(Globals.vulkan ? "enabled" : "disabled")}");

            KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.v10_0_26100_0);
            arguments.Generate<Solution>();
        }
    }
}