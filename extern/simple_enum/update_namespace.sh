#!/bin/bash
# SPDX-FileCopyrightText: 2024 Artur Bać
# SPDX-License-Identifier: BSL-1.0
# SPDX-PackageHomePage: https://github.com/arturbac/simple_enum
# Define the path to the 'include' directory
INCLUDE_DIR="./include"

# Define the search and replace patterns
SEARCH_PATTERN="namespace simple_enum::inline v0_7"
REPLACE_PATTERN="namespace simple_enum::inline v0_8"

# Use find to recursively search for .h and .hpp files and sed to replace the text
find "$INCLUDE_DIR" \( -name "*.h" -o -name "*.hpp" \) -type f -exec sed -i "s/${SEARCH_PATTERN}/${REPLACE_PATTERN}/g" {} +

echo "Text replacement complete."
