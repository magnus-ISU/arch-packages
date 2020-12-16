#
# ~/.bashrc
#

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

PS1='[\u@\h \W]\$ '

alias l='ls -alhS --group-directories-first --color=auto'
alias ls='ls --color=auto --group-directories-first'

mydu() {
	du -hd 1 $1 2>/dev/null | sort -rh
}
