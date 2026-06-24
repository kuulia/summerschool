#!/bin/bash

# SPDX-FileCopyrightText: 2026 CSC - IT Center for Science Ltd. <www.csc.fi>
#
# SPDX-License-Identifier: MIT

# Usage:
# - store original *.pdf in the current directory
# - prefix pdf files not to be shared as NO_*.pdf
# - run this script -> creates all/lightning-talks*.pdf

set -xeuo pipefail

mkdir -p all
cd all
rm -f *.pdf

# Width for scaled pages
TARGET_WIDTH=800

process_page() {
    local f="$1"
    local i="$2"

    # Output filename
    local OUT
    if [[ "$(basename "$f")" == NO_* ]]; then
        OUT=$(printf "page_%03d_NO.pdf" "$i")
    else
        OUT=$(printf "page_%03d.pdf" "$i")
    fi
    local TMP=$(printf "tmp_%03d.pdf" "$i")

    # Extract first page
    pdftk "$f" cat 1 output "$TMP"

    # Get width and height (points)
    local WIDTH HEIGHT
    WIDTH=$(pdfinfo "$TMP" | grep "Page size" | awk '{print $3}')
    HEIGHT=$(pdfinfo "$TMP" | grep "Page size" | awk '{print $5}')

    # Sanity check
    if [ -z "$WIDTH" ] || [ -z "$HEIGHT" ] || [ "$WIDTH" = "0" ]; then
        echo "Error: failed to read page size for $f"
        return 1
    fi

    # Compute scale + new height
    local SCALE NEW_HEIGHT
    SCALE=$(awk -v t="$TARGET_WIDTH" -v w="$WIDTH" 'BEGIN {printf "%.6f", t/w}')
    NEW_HEIGHT=$(awk -v h="$HEIGHT" -v s="$SCALE" 'BEGIN {printf "%.0f", h*s}')

    # Scale + downsample
    gs -q -dNOPAUSE -dBATCH -dSAFER \
       -sDEVICE=pdfwrite \
       -dCompatibilityLevel=1.5 \
       -dPDFFitPage \
       -dDEVICEWIDTHPOINTS="$TARGET_WIDTH" \
       -dDEVICEHEIGHTPOINTS="$NEW_HEIGHT" \
       -dDownsampleColorImages=true \
       -dColorImageResolution=100 \
       -dColorImageDownsampleType=/Bicubic \
       -dDownsampleGrayImages=true \
       -dGrayImageResolution=100 \
       -dGrayImageDownsampleType=/Bicubic \
       -dDownsampleMonoImages=true \
       -dMonoImageResolution=100 \
       -sOutputFile="$OUT" \
       "$TMP"

    rm "$TMP"
}

i=1
for f in ../*.pdf; do
    process_page "$f" "$i" &
    i=$((i+1))
done
wait

# Combine all pages
pdftk page_*.pdf cat output lightning-talks-2026.pdf
pdftk page_???.pdf cat output lightning-talks-2026-share.pdf
rm page_*.pdf
