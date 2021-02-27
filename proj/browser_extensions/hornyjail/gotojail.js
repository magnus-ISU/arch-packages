const horny = {urls: [
	"*://*.reddit.com/r/memes*",
	"*://*.reddit.com/r/gonewild*",
	"*://*.reddit.com/r/asiansgonewild*",
	"*://*.reddit.com/r/hardcorensfw*",
	"*://*.reddit.com/r/guro*",
	"*://*.reddit.com/r/yaoiguro*",
	"*://*.reddit.com/r/quiver*",
	"*://*.reddit.com/r/bdsm*",
	"*://*.reddit.com/r/worldpolitics*"
]};

const no_pass_go = {
	url: "https://reddit.com/r/nofap"
}

function directly_to_jail(tab_id, change_info, tab_info) {
	if (change_info.url) {
		console.log("Tab: " + tab_id + " URL changed to " + change_info.url);
		browser.tabs.update(tab_id, no_pass_go);
	}
}

browser.tabs.onUpdated.addListener(directly_to_jail, horny);
console.log("Fear no horny...");
