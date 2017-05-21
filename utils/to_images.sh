#!/bin/bash
# Turns a video into a set of images with 1 second intervals
ffmpeg -i "$1" \
       -r 1 \
       -f image2 "$2-%03d.jpeg"

