﻿// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using NUnit.Framework;
using Sharpmake.Generators.FastBuild;
using Sharpmake.UnitTests.BffUnityResolverTestUtils;

namespace Sharpmake.UnitTests
{
    [TestFixture]
    public class BffUnityResolverTest : TestProjectBuilder
    {
        public BffUnityResolverTest()
            : base(InitType.Cpp, typeof(BffUnityResolverTestProjects.SimpleProject).Namespace, ConfigureOrder.New)
        { }

        [SetUp]
        public void TestSetUp()
        {
            Init();
        }

        [TearDown]
        public void TestTearDown()
        {
            Builder.Dispose();
            Util.ClearFakeTree();
        }

        [Test]
        public void FragmentUnityResolver()
        {
            GenerateAndBuildUsingResolver<Bff.FragmentUnityResolver>(DevEnv.vs2017 | DevEnv.vs2019);

            var unityLogNames = GetProjectUnityLogNames();

            var expectedLogNames = new List<string> {
                    "SimpleProject_vs2017_Debug : SimpleProject_vs2017_Debug_Release_unity",   // Debug and Release have the same unity config
                    "SimpleProject_vs2017_Release : SimpleProject_vs2017_Debug_Release_unity",
                    "SimpleProject_vs2017_Retail : SimpleProject_vs2017_Retail_unity",
                    "SimpleProject_vs2019_Debug : SimpleProject_vs2019_Debug_Release_unity",   // vs2019 values follow same pattern (debug == release),
                    "SimpleProject_vs2019_Release : SimpleProject_vs2019_Debug_Release_unity", // but different names because of different conf.FastBuildUnityUseRelativePaths from vs2017.
                    "SimpleProject_vs2019_Retail : SimpleProject_vs2019_Retail_unity",
                };
            CollectionAssert.AreEqual(expectedLogNames, unityLogNames);
        }

        [Test]
        public void HashUnityResolver()
        {
            GenerateAndBuildUsingResolver<Bff.HashUnityResolver>(DevEnv.vs2017 | DevEnv.vs2019);

            var unityLogNames = GetProjectUnityLogNames();

            var expectedLogNames = new List<string> {
                    "SimpleProject_vs2017_Debug : SimpleProject_unity_C301FCDB",   // Debug and Release have the same unity config, so hash is identical.
                    "SimpleProject_vs2017_Release : SimpleProject_unity_C301FCDB",
                    "SimpleProject_vs2017_Retail : SimpleProject_unity_0BAACBF5",
                    "SimpleProject_vs2019_Debug : SimpleProject_unity_D68FD759",   // vs2019 values follow same pattern (debug == release),
                    "SimpleProject_vs2019_Release : SimpleProject_unity_D68FD759", // but different hash values because of different conf.FastBuildUnityUseRelativePaths from vs2017.
                    "SimpleProject_vs2019_Retail : SimpleProject_unity_3DFB929D",
                };
            CollectionAssert.AreEqual(expectedLogNames, unityLogNames);
        }

        [Test]
        public void FragmentHashUnityResolver()
        {
            GenerateAndBuildUsingResolver<Bff.FragmentHashUnityResolver>(DevEnv.vs2017 | DevEnv.vs2019);

            var unityLogNames = GetProjectUnityLogNames();

            var expectedLogNames = new List<string> {
                    "SimpleProject_vs2017_Debug : SimpleProject_unity_41974D6F",   // Debug and Release have the same unity config, so hash is identical.
                    "SimpleProject_vs2017_Release : SimpleProject_unity_41974D6F",
                    "SimpleProject_vs2017_Retail : SimpleProject_unity_55FBF013",
                    "SimpleProject_vs2019_Debug : SimpleProject_unity_32D177F1",   // vs2019 values follow same pattern (debug == release),
                    "SimpleProject_vs2019_Release : SimpleProject_unity_32D177F1", // but different hash values because of different conf.FastBuildUnityUseRelativePaths from vs2017.
                    "SimpleProject_vs2019_Retail : SimpleProject_unity_CFC9B4D5",
                };
            CollectionAssert.AreEqual(expectedLogNames, unityLogNames);
        }

        protected void GenerateAndBuildUsingResolver<T>(DevEnv devEnv) where T : Bff.IUnityResolver, new()
        {
            Bff.IUnityResolver unityResolver = new T();
            Bff.UnityResolver = new UnityResolverLogger(unityResolver);

            GlobalSettings.DefaultDevEnvs = devEnv;

            GenerateAndBuildProjects();
        }

        protected List<string> GetProjectUnityLogNames()
        {
            var project = GetProject<BffUnityResolverTestProjects.SimpleProject>();
            Assert.IsNotNull(project);

            var logUnityNames = project.Configurations.OfType<CommonProjectConfiguration>().Select(x => x.Name + " : " + x.LogUnityName).ToList();
            logUnityNames.Sort();
            return logUnityNames;
        }
    }

    namespace BffUnityResolverTestUtils
    {
        public class CommonProjectConfiguration : Project.Configuration
        {
            public string LogUnityName = null;
        }

        public class UnityResolverLogger : Bff.IUnityResolver
        {
            private Bff.IUnityResolver _unityResolver;

            public UnityResolverLogger(Bff.IUnityResolver resolver)
            {
                _unityResolver = resolver;
            }

            public void ResolveUnities(Project project, string projectPath, ref Dictionary<Bff.Unity, List<Project.Configuration>> unities)
            {
                _unityResolver.ResolveUnities(project, projectPath, ref unities);

                foreach (var unitySection in unities)
                {
                    var unity = unitySection.Key;
                    var unityConfigurations = unitySection.Value;
                    foreach (var conf in unityConfigurations.OfType<CommonProjectConfiguration>())
                    {
                        Assert.IsNull(conf.LogUnityName);
                        Assert.IsNotNull(unity.UnityName);
                        conf.LogUnityName = unity.UnityName;
                    }
                }
            }
        }
        public static class GlobalSettings
        {
            public static DevEnv DefaultDevEnvs = DevEnv.vs2017 | DevEnv.vs2019;
        }
    }


    namespace BffUnityResolverTestProjects
    {
        [Generate]
        public class SimpleProject : Project
        {
            public SimpleProject() : base(typeof(Target), typeof(CommonProjectConfiguration))
            {
                RootPath = Directory.GetCurrentDirectory();
                SourceRootPath = Directory.GetCurrentDirectory() + "/[project.Name]";
                AddTargets(new Target(Platform.win64,
                                      GlobalSettings.DefaultDevEnvs,
                                      Optimization.Debug | Optimization.Release | Optimization.Retail,
                                      OutputType.Lib,
                                      Blob.FastBuildUnitys,
                                      BuildSystem.FastBuild));
            }

            [Configure]
            public void ConfigureAll(CommonProjectConfiguration conf, Target target)
            {
                conf.ProjectPath = @"[project.RootPath]\[project.Name]\[target.DevEnv]";
                conf.Name = "[project.Name]_[target.DevEnv]_[target.Optimization]";

                conf.IsFastBuild = true;

                // Make Unity configurations:
                // Make a different config content per devenv
                if (target.DevEnv == DevEnv.vs2017)
                    conf.FastBuildUnityUseRelativePaths = false;
                else
                    conf.FastBuildUnityUseRelativePaths = true;

                // For a given devenv version, unity config is identical between debug and release, retail is different
                if (target.Optimization == Optimization.Retail)
                {
                    conf.FastBuildUnityInputIsolateWritableFiles = false;
                    conf.FastBuildUnityInputIsolateWritableFilesLimit = 0;
                }
                else
                {
                    conf.FastBuildUnityInputIsolateWritableFiles = true;
                    conf.FastBuildUnityInputIsolateWritableFilesLimit = 3;
                }
            }
        }
    }
}
