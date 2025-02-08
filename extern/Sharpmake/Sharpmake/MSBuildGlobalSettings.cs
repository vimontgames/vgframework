// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.

using System;
using System.Collections.Concurrent;

namespace Sharpmake
{
    /// <summary>
    /// This class contains some global msbuild settings
    /// </summary>
    public static class MSBuildGlobalSettings
    {
        // cppPlatformFolders (pre-vs2019)
        private static readonly ConcurrentDictionary<Tuple<DevEnv, string>, string> s_cppPlatformFolders = new ConcurrentDictionary<Tuple<DevEnv, string>, string>();
        private static bool s_overrideGlobalVCTargetsPath = false;

        /// <summary>
        /// Allows overwriting the MSBuild platform folder used for a known sharpmake platform and Visual Studio version.
        /// This is typically used if you want to put VS files in source control such as Perforce or nuget.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <param name="value">The location of the MSBuild platform folder. Warning: this *must* end with a trailing separator</param>
        /// <returns></returns>
        public static void SetCppPlatformFolder(DevEnv devEnv, Platform platform, string value)
        {
            SetCppPlatformFolderInternal(devEnv, Util.GetSimplePlatformString(platform), value);
        }

        /// <summary>
        /// Allows overwriting the MSBuild platform folder used for a custom platform passed as a string and Visual Studio version.
        /// This is typically used if you want to put VS files in source control such as Perforce or nuget.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <param name="value">The location of the MSBuild platform folder. Warning: this *must* end with a trailing separator</param>
        /// <returns></returns>
        [Obsolete("Use SetCppPlatformFolder with a Platform enum field instead")]
        public static void SetCppPlatformFolder(DevEnv devEnv, string platform, string value) => SetCppPlatformFolderInternal(devEnv, platform, value);
        private static void SetCppPlatformFolderInternal(DevEnv devEnv, string platform, string value)
        {
            var key = Tuple.Create(devEnv, platform);
            if (!string.Equals(s_cppPlatformFolders.GetOrAdd(key, value), value))
                throw new Error("You can't register more than once a platform folder for a specific combinaison. Key already registered: " + key);
        }

        /// <summary>
        /// Use to reset the override of a platform
        /// </summary>
        /// <param name="devEnv">Visual studio version</param>
        /// <param name="platform">Platform</param>
        public static void ResetCppPlatformFolder(DevEnv devEnv, Platform platform)
        {
            ResetCppPlatformFolderInternal(devEnv, Util.GetSimplePlatformString(platform));
        }

        /// <summary>
        /// Use to reset the override of a platform
        /// </summary>
        /// <param name="devEnv">Visual studio version</param>
        /// <param name="platform">Platform</param>
        [Obsolete("Use ResetCppPlatformFolder with a Platform enum field instead")]
        public static void ResetCppPlatformFolder(DevEnv devEnv, string platform) => ResetCppPlatformFolderInternal(devEnv, platform);
        private static void ResetCppPlatformFolderInternal(DevEnv devEnv, string platform)
        {
            var key = Tuple.Create(devEnv, platform);
            string value;
            s_cppPlatformFolders.TryRemove(key, out value);
        }

        /// <summary>
        /// Get the overwritten MSBuild platform folder used for a known sharpmake platform and Visual studio version.
        /// This is typically used if you want to put your VS files in source control such as Perforce or nuget.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <returns>the registered msbuild foldervalue for the requested pair. null if not found</returns>
        public static string GetCppPlatformFolder(DevEnv devEnv, Platform platform)
        {
            return GetCppPlatformFolderInternal(devEnv, Util.GetSimplePlatformString(platform));
        }

        /// <summary>
        /// Get the overwritten MSBuild platform folder used for a custom platform passed as a string and Visual studio version.
        /// This is typically used if you want to put your VS files in source control such as Perforce or nuget.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <returns>the registered msbuild foldervalue for the requested pair. null if not found</returns>
        [Obsolete("Use GetCppPlatformFolder with a Platform enum field instead")]
        public static string GetCppPlatformFolder(DevEnv devEnv, string platform) => GetCppPlatformFolderInternal(devEnv, platform);
        private static string GetCppPlatformFolderInternal(DevEnv devEnv, string platform)
        {
            var key = Tuple.Create(devEnv, platform);
            string value;
            if (s_cppPlatformFolders.TryGetValue(key, out value))
                return value;
            return null; // No override found
        }


        // additionalVCTargetsPath (vs2019+)
        private static readonly ConcurrentDictionary<Tuple<DevEnv, string>, string> s_additionalVCTargetsPath = new ConcurrentDictionary<Tuple<DevEnv, string>, string>();

        /// <summary>
        /// Allows setting MSBuild vc targets path used for a known sharpmake platform and Visual Studio version.
        /// This is typically used if you want to add platform specific files since vs2019 as the older _PlatformFolder way of doing it is deprecated.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <param name="value">The location of the MSBuild additional VC target path. Warning: this *must* end with a trailing separator</param>
        /// <returns></returns>
        public static void SetAdditionalVCTargetsPath(DevEnv devEnv, Platform platform, string value)
        {
            SetAdditionalVCTargetsPathInternal(devEnv, Util.GetSimplePlatformString(platform), value);
        }

        /// <summary>
        /// Allows setting MSBuild vc target path used for a custom platform passed as a string and Visual Studio version.
        /// This is typically used if you want to add platform specific files with vs2019+ as the older way of doing it through _PlatformFolder is deprecated.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <param name="value">The location of the MSBuild additional VC target path. Warning: this *must* end with a trailing separator</param>
        /// <returns></returns>
        [Obsolete("Use SetAdditionalVCTargetsPath with a Platform enum field instead")]
        public static void SetAdditionalVCTargetsPath(DevEnv devEnv, string platform, string value) => SetAdditionalVCTargetsPathInternal(devEnv, platform, value);
        private static void SetAdditionalVCTargetsPathInternal(DevEnv devEnv, string platform, string value)
        {
            var key = Tuple.Create(devEnv, platform);
            if (!string.Equals(s_additionalVCTargetsPath.GetOrAdd(key, value), value))
                throw new Error("You can't register more than once an additional VC target path for a specific combinaison. Key already registered: " + key);
        }

        /// <summary>
        /// Use to reset the set of AdditionalVCTargetsPath
        /// </summary>
        /// <param name="devEnv">Visual studio version</param>
        /// <param name="platform">Platform</param>
        public static void ResetAdditionalVCTargetsPath(DevEnv devEnv, Platform platform)
        {
            ResetAdditionalVCTargetsPathInternal(devEnv, Util.GetSimplePlatformString(platform));
        }

        /// <summary>
        /// Use to reset the set of AdditionalVCTargetsPath
        /// </summary>
        /// <param name="devEnv">Visual studio version</param>
        /// <param name="platform">Platform</param>
        [Obsolete("Use ResetAdditionalVCTargetsPath with a Platform enum field instead")]
        public static void ResetAdditionalVCTargetsPath(DevEnv devEnv, string platform) => ResetAdditionalVCTargetsPathInternal(devEnv, platform);
        private static void ResetAdditionalVCTargetsPathInternal(DevEnv devEnv, string platform)
        {
            var key = Tuple.Create(devEnv, platform);
            string value;
            s_additionalVCTargetsPath.TryRemove(key, out value);
        }

        /// <summary>
        /// Get the MSBuild Additional VC targets path used for a known sharpmake platform and Visual studio version.
        /// This is typically used if you want to add platform specific files with vs2019+ as the older way of doing it through _PlatformFolder is deprecated.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <returns>the registered msbuild additional VC targets path for the requested pair. null if not found</returns>
        public static string GetAdditionalVCTargetsPath(DevEnv devEnv, Platform platform)
        {
            return GetAdditionalVCTargetsPathInternal(devEnv, Util.GetSimplePlatformString(platform));
        }

        /// <summary>
        /// Get the MSBuild Additional VC targets path used for a custom platform passed as a string and Visual studio version.
        /// This is typically used if you want to add platform specific files with vs2019+ as the older way of doing it through _PlatformFolder is deprecated.
        /// </summary>
        /// <param name="devEnv">Visual studio version affected</param>
        /// <param name="platform">Platform affected</param>
        /// <returns>the registered msbuild additional VC targets for the requested pair. null if not found</returns>
        [Obsolete("Use GetAdditionalVCTargetsPath with a Platform enum field instead")]
        public static string GetAdditionalVCTargetsPath(DevEnv devEnv, string platform) => GetAdditionalVCTargetsPathInternal(devEnv, platform);
        private static string GetAdditionalVCTargetsPathInternal(DevEnv devEnv, string platform)
        {
            var key = Tuple.Create(devEnv, platform);
            string value;
            if (s_additionalVCTargetsPath.TryGetValue(key, out value))
                return value;
            return null; // No override found
        }

        public static void SetOverrideGlobalVCTargetsPath(bool overrideGlobalVCTargetsPath)
        {
            s_overrideGlobalVCTargetsPath = overrideGlobalVCTargetsPath;
        }

        public static bool IsOverridingGlobalVCTargetsPath()
        {
            return s_overrideGlobalVCTargetsPath;
        }
    }
}
