#!/usr/bin/bash

shopt -s globstar

make
jq -s add **/compile_commands.json > merged.json
mv merged.json compile_commands.json
