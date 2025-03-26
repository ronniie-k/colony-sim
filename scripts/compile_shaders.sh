#!/bin/bash

glslc res/shaders/shader.vert -o res/shaders/output/vert.spv
glslc res/shaders/shader.frag -o res/shaders/output/frag.spv
rm -rf /home/ronnie/dev/colony-sim/build/src/res
cp -r res /home/ronnie/dev/colony-sim/build/src/