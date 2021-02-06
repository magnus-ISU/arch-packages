#!/bin/sh

decomment() {
	sed 's:^[ \t]*#.*$::g;/^[ \t]*$/d' $1
}

decomment rc.keys > ~/.config/ranger/rc.conf
decomment rc.options >> ~/.config/ranger/rc.conf

decomment rifle.txt > ~/.config/ranger/rifle.conf

echo '#!/usr/bin/env bash' > ~/.config/ranger/scope.sh
decomment scope.sh.orig >> ~/.config/ranger/scope.sh
