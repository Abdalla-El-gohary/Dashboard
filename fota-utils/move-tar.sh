#!/bin/bash

# Get the directory where the script is being run
target_dir=~/updates
find_path=~/Desktop/merging_Dashboard/Dashboard/fota-utils

# Find all .tar and .bin files recursively and move them to the target directory
find "$find_path" -type f \( -name "*.tar" -o -name "*.bin" \) | while read -r file; do
    echo "Moving: $file"
    mv -v "$file" "$target_dir"/
done
