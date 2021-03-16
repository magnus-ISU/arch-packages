#!/bin/bash

VID=$(curl "https://www.youtube.com/results?search_query=$(echo "$1" | sed 's/ /%20/g')" 2>/dev/null | ./snipsnip)
rm "$2"
youtube-dl -o "$2" -f bestaudio "https://www.youtube.com/watch?v=$VID" ||
( youtube-dl -F "https://www.youtube.com/watch?v=$VID" && exit 1 )

exit 0
