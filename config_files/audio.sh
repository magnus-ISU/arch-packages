#!/bin/sh
# ~/media

if [ $# -eq 0 ]; then
	echo Actions: play save kill short
else
	case $1 in
	play)
		if [ $# -eq 2 ]; then
			ffplay -loop 0 -nodisp $2 2>/dev/null &
		else
			echo Format: 'play filename'
		fi
		;;
	save)
		if [ $# -eq 2 ]; then
			youtube-dl -F $2
		elif [ $# -eq 4 ]; then
			youtube-dl -o "$4"'.%(ext)s' -f $3 $2
		else
			echo Format: 'save url [typenum output_name]'
		fi
		;;
	kill)
		killall ffplay
		;;
	short)
		if [ $# -eq 5 ]; then
			ffmpeg -ss $3 -t $4 -i $2 -c copy $5
		else
			echo Format: 'save short orig.file 10 20 new.file' saves seconds 10-30 of orig.file to new.file
		fi
		;;
	*)
		echo Unrecognized action $1
		;;
	esac
fi
