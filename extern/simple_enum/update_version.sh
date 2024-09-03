#!/bin/bash
# SPDX-FileCopyrightText: 2024 Artur Bać
# SPDX-License-Identifier: BSL-1.0
# SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
# Define the file path
file_path="include/simple_enum/core.hpp"

# Check if the file exists
if [ ! -f "$file_path" ]; then
    echo "Error: File $file_path does not exist."
    exit 1
fi

# Extract the current version string from the file
current_version=$(grep -oP '#define SIMPLE_ENUM_NAME_VERSION "\K[0-9]+\.[0-9]+\.[0-9]+' $file_path)

if [ -z "$current_version" ]; then
    echo "Error: Could not extract the current version from $file_path."
    exit 1
fi

# If no argument provided, print the current version and exit
if [ -z "$1" ]; then
    echo "Current version: $current_version"
    exit 0
fi

# The script takes one argument: the version string x.y.z
new_version=$1

# Regular expression to validate the version format
version_regex="^[0-9]+\.[0-9]+\.[0-9]+$"

# Check if the version string matches the required format
if ! [[ $new_version =~ $version_regex ]]; then
    echo "Error: Version string must be in x.y.z format, where x, y, and z are numbers."
    exit 1
fi

# Compare the current and new version strings
if ! printf "%s\n%s\n" "$current_version" "$new_version" | sort -V | tail -1 | grep -q "^$new_version$"; then
    echo "Error: The new version must be greater than the current version ($current_version)."
    exit 1
fi

# Replace the version string in the specified file
sed -i "s/#define SIMPLE_ENUM_NAME_VERSION \".*\"/#define SIMPLE_ENUM_NAME_VERSION \"$new_version\"/" $file_path

if [ $? -ne 0 ]; then
    echo "Error: Failed to update the version in $file_path."
    exit 1
fi

# Commit the change with a version-specific message
git add $file_path
git commit -m "v$new_version"

if [ $? -ne 0 ]; then
    echo "Error: Failed to commit the changes."
    exit 1
fi

# Add a Git tag with the same version
git tag "v$new_version"

if [ $? -ne 0 ]; then
    echo "Error: Failed to add Git tag v$new_version."
    exit 1
fi

echo "Version updated to v$new_version, committed, and tagged successfully."
