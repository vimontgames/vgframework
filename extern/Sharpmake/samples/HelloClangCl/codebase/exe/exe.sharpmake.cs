﻿// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.

using Sharpmake;

namespace HelloClangCl
{
    [Sharpmake.Generate]
    public class ExeProject : CommonProject
    {
        public ExeProject()
        {
            AddTargets(CommonTarget.GetDefaultTargets());
            Name = "exe";
        }

        public override void ConfigureAll(Configuration conf, CommonTarget target)
        {
            base.ConfigureAll(conf, target);

            conf.Output = Configuration.OutputType.Exe;

            conf.PrecompHeader = "stdafx.h";
            conf.PrecompSource = "stdafx.cpp";

            conf.AddPrivateDependency<Dll1Project>(target);
            conf.AddPrivateDependency<StaticLib2Project>(target);

            conf.Defines.Add("CREATION_DATE=\"March 2021\"");
        }
    }
}
