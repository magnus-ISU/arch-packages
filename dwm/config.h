/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int gappx     = 2;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=12" };
static const char dmenufont[]       = "monospace:size=11";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char col_aqua[]        = "#11c620";
static const char col_gold[]        = "#dbb92e";
static const unsigned int baralpha = 0xd0;
static const unsigned int unfocusedbaralpha = 0x80;
static const unsigned int borderalpha = OPAQUE;

static const char *colors[][3]      = {
	/*                               fg         bg        border   */
	[SchemeNorm]	         = { col_gold,  col_gray1,  col_gray2 },
	[SchemeSel]	         = { col_gray1, col_gold,   col_aqua  },
	[SchemeUnselMonBarNorm]    = { col_aqua,  col_gray1,  col_gray2  },
};
static const unsigned int alphas[][3]      = {
	/*               fg      bg        border     */
	[SchemeNorm] = { OPAQUE, baralpha, borderalpha },
	[SchemeSel]  = { OPAQUE, baralpha, borderalpha },
	[SchemeUnselMonBarNorm]  = { OPAQUE, unfocusedbaralpha, borderalpha },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "askpass",  0,          0,          0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]H",      tile },    /* first entry is default */
	{ "FLO",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
 	{ "[@]",      spiral },
 	{ "[\\]",      dwindle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gold, "-sb", col_cyan, "-sf", col_gray4, NULL };

static const char *termcmd[]		= { "st", 0 };
static const char *browsercmd[] 	= { "waterfox", "--MOZ_LOG_FILE=/tmp/mag/trash/misc/waterfox.log", 0 };
static const char *passwordcmd[] 	= { "passmenu", 0 };
static const char *packagecmd[]		= { "/home/mag/p/util/ms_pacman", 0 };
static const char *historycmd[]		= { "/home/mag/p/util/dmenu_history", 0 };
static const char *womancmd[]		= { "/home/mag/p/util/woman", 0 };

#include "shiftview.c"
static Key keys[] = {
	/* modifier                     key        function        argument */

//navigation
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      incnmaster,     {.i = -1 } },

	{ MODKEY,                       XK_l,      shiftview,      {.i = +1 } },
	{ MODKEY,                       XK_h,      shiftview,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY|ShiftMask,             XK_l,      setmfact,       {.f = +0.05} },

	{ MODKEY|ShiftMask,             XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_d,      tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_u,      tagmon,         {.i = +1 } },

	{ MODKEY,                       XK_s,      zoom,           {0} },
	{ MODKEY,                       XK_f,      fullscreen,     {0} },

//spawning windows
	{ MODKEY,                       XK_space,  spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_t,      spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      spawn,          {.v = browsercmd } },

	{ MODKEY,                       XK_p,      spawn,          {.v = packagecmd } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = passwordcmd } },
	{ MODKEY|ShiftMask,             XK_r,      spawn,          {.v = historycmd } },
	{ MODKEY,                       XK_m,      spawn,          {.v = womancmd } },

	{ MODKEY,                       XK_x,      killclient,     {0} },

//other
	{ MODKEY|ShiftMask,             XK_f,      togglefloating, {0} },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },

//removed
//	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
//	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
//	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
//	{ MODKEY,                       NONE,      setlayout,      {.v = &layouts[3]} }, //fib, I edited dwm.c to make this default
//	{ MODKEY,                       NONE,      setlayout,      {.v = &layouts[4]} }, //dwindle
//	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
//	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
//	{ MODKEY,                       XK_Tab,    view,           {0} },
//	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
//	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
//	{ MODKEY|ShiftMask,             XK_Return, setlayout,      {0} },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_Tab   ,  quit,       {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

