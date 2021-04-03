#!/usr/bin/python3

import subprocess
import time
import threading

PLAYER = "mpv"
DOWNLOADTARGET = "/tmp/alex_program_downloaded_file"
TIMER_AUDIO = "timer.webm"

# TODO locks to make it thread-safe
play_video = None
timers = []

def play(video):
	global play_video
	kill_video()
	print("playing todo video '%s'" % (video))
	find_video = subprocess.run(["./ysearch.sh", video, DOWNLOADTARGET], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
	if find_video.returncode != 0:
		print("Could not download todo video '%s'" % (video))
		return
	play_video = subprocess.Popen([PLAYER, DOWNLOADTARGET], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL)

def kill_video():
	global play_video
	print("checking process")
	if play_video == None:
		return
	print("killing process")
	if play_video.poll() == None:
		play_video.kill()
		print("killed process")
	play_video = None

def set_timer(duration):
	# Calculate the number of seconds this timer is for
	seconds = int(duration)
	# Create a thread to wait for the timer's duration
	new_timer = thread_timer(seconds)
	new_timer.start()
	timers.append(new_timer)

def main():
	my_input = input()
	while len(my_input) > 0:
		if my_input.startswith("play "):
			play(my_input[5:])
		elif my_input.startswith("stop"):
			kill_video()
		elif my_input.startswith("set a timer "):
			set_timer(my_input[11:])
		else:
			print("Unknown command in '%s'" % (my_input))
		my_input = input()

class thread_timer(threading.Thread):
	def __init__(self, duration):
		super(thread_timer, self).__init__()
		self.duration = duration
		self.endtime = int(time.time()) + duration
	def run(self):
		global play_video
		print("sleeping")
		time.sleep(self.duration)
		print("woke")
		kill_video()
		play_video = subprocess.Popen([PLAYER, TIMER_AUDIO, "--loop"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, stdin=subprocess.DEVNULL)
		print("broke")

main()
