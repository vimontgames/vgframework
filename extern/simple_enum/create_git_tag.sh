#!/bin/bash
# SPDX-FileCopyrightText: 2024 Artur Bać
# SPDX-License-Identifier: BSL-1.0
# SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
# Path to the header file
file_path="include/simple_enum/simple_enum.hpp"

# Check if the file exists
if [ -f "$file_path" ]; then
    # Extract the version number using grep and awk
    version=$(grep "SIMPLE_ENUM_NAME_VERSION" $file_path | awk -F '"' '{print $2}')
    
    # Check if version was found
    if [ -z "$version" ]; then
        echo "Version not found in $file_path"
        exit 1
    fi

    # Format the version for git tagging
    # Assuming the version in the file does not have a 'v' prefix and is in the form x.y.z
    git_tag="v${version}"

    # Create the Git tag
    git tag -a "$git_tag" -m "Release $git_tag"

    echo "Git tag $git_tag created."
else
    echo "File $file_path does not exist."
fi
