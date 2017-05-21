#!/bin/bash

# This script turns a set of images into one image with them tiled and
# a border around them. Expects a folder which contains all the files needed
# to make the image.
# Second argument is the name of the output file
montage -mode concatenate -tile 4x2 \
 -border 8 \
 -geometry +24+24 \
 "$1"/*.jpeg "$2".jpeg
