all: lynx.shortcfg
	#./decomment.sh lynx.cfg > lynx.shortcfg
	#./decomment.sh lynx.keybinds >> lynx.shortcfg
	#cp lynx.shortcfg ~/.config/lynx/lynx.cfg
	#./decomment.sh lynx.shortcfg > ~/.config/lynx/lynx.cfg
	sed 's:^[ \t]*#.*$::g;/^[ \t]*$/d' lynx.shortcfg > "XDG_CONFIG_HOME/lynx/lynx.cfg"
