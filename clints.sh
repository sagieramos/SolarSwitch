#!/bin/bash

# Define the source code directory
SRC_DIR="./src"

# Define a list of file extensions to check (e.g., .c and .h)
EXTENSIONS="c h"

# Run clang-format to fix code formatting
echo "Fixing code formatting..."
for EXT in $EXTENSIONS; do
  find "$SRC_DIR" -type f -name "*.$EXT" -exec clang-format -style=file -i {} \;
done

# Run cppcheck for static code analysis
echo "Running static code analysis..."
cppcheck --enable=all --force --quiet "$SRC_DIR"

# Check the exit codes of the tools
if [ $? -eq 0 ]; then
  echo "Linting completed successfully. No issues found."
  exit 0
else
  echo "Linting found issues. Please review the output for details."
  exit 1
fi
