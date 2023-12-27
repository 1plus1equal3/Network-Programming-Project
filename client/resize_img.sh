#!/bin/bash

# Resize images to 800x450
# Usage: ./resize_img.sh <path_to_images>
if test -f "$1"; then
    mogrify -resize 800x450 $1
else 
    echo "$1 does not exist."
fi
