using Sharpmake;

[module: Sharpmake.Include("vg.*.sharpmake.cs")]

namespace vg
{
    public static class Main
    {
        [Sharpmake.Main]
        public static void SharpmakeMain(Sharpmake.Arguments arguments)
        {
            KitsRootPaths.SetUseKitsRootForDevEnv(DevEnv.vs2022, KitsRootEnum.KitsRoot10, Options.Vc.General.WindowsTargetPlatformVersion.v10_0_26100_0);
            arguments.Generate<Solution>();
        }
    }
}