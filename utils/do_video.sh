#!/bin/bash
# Takes an already existing video and adds timestamp to the top left corner.
# After this is done, it uses `to-images.sh` to turn the videos into a set
# of images with one second interval.
# Lastly, it creates a directory to keep those store these images and a folder
# to store the used folder

# needs 4 arguments
# $1 name of video
# $2 name of temp to store
# $3 name of folder to create
# $4 start name of folders

if [ $# -lt 4 ]; then
  echo "Need 4 inputs"
  exit 1
fi

ffmpeg -i "$1" \
       -r 30 \
       -vcodec libx264 \
       -cmp 22 \
       -vf "drawtext=fontfile=DroidSansMono.ttf: \
            fontsize=200: \
            text='%{pts\:gmtime\:0\:%S}': \
            r=30: \
            x=0: \
            y=0: \
            fontcolor=black: \
            box=1: \
            boxcolor=0x00000000" \
            -y "$2"

./to-images.sh "$2" "$4"
mkdir "$3-used"
mkdir "$3"
mv "$1" "$3-used/"
mv "$2" "$3-used/"
mv "$4"* "$3/"
