/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "inconsolata:size=8" };
static const char dmenufont[]       = "inconsolata:size=8";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel] =  { col_gray4, col_cyan,  col_cyan  },
};

/*   Display modes of the tab bar: never shown, always shown, shown only in  */
/*   monocle mode in presence of several windows.                            */
/*   A mode can be disabled by moving it after the showtab_nmodes end marker */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab            = showtab_auto; /* Default tab bar show mode  */
static const Bool toptab            = False;         /* False means bottom tab bar */

/* tagging */
static const char *tags[] = { "cmd", "www", "irc", "etc", "dvr", "rdp", "gog" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class,             instance,           title,      tagsmask,  isfloating,  monitor */
//cmd
	{ "st-256color",      "st-256color",      NULL,       1,         False,       -1 },
//www
	{ "Chromium",         "chromium",         NULL,       1 << 1,    False,       -1 },
	{ "Firefox",          NULL,               NULL,       1 << 1,    False,       -1 },
	{ "qutebrowser",      "qutebrowser",      NULL,       1 << 1,    False,       -1 },
//irc
	{ "ircterm",          NULL,               NULL,       1 << 2,    False,       -1 },
	{ "quassel",          "quasselclient",    NULL,       1 << 2,    False,       -1 },
	{ "Keybase",          "keybase",          NULL,       1 << 2,    False,       -1 },
	{ "Signal",           "signal",           NULL,       1 << 2,    False,       -1 },
	{ "discord",          "discord",          NULL,       1 << 2,    False,       -1 },
	{ "Slack",            "slack",            NULL,       1 << 2,    False,       -1 },
//etc
	{ "Deluge",           "deluge",           NULL,       1 << 3,    False,       -1 },
//dvr
	{ "Spotify",          "spotify",          NULL,       1 << 4,    False,       -1 },
	{ "plexmediaplayer",  "plexmediaplayer",  NULL,       1 << 4,    False,       -1 },
	{ "Plexamp",          "plexamp",          NULL,       1 << 4,    True,        -1 },
	{ "Girens",           "girens",           NULL,       1 << 4,    False,       -1 },
//rdp
	{ "xfreerdp",         "xfreerdp",         NULL,       1 << 5,    False,       -1 },
//gog
	{ "steam",            "steam",            NULL,       1 << 6,    False,       -1 },       // Big Picture Mode
	{ "Steam",            "Steam",            NULL,       1 << 6,    False,       -1 },
	{ "Steam",            "Steam",            "Friends",  1 << 6,    False,       -1 },
	{ "retroarch",        "retroarch",        NULL,       1 << 6,    False,       -1 },
//all
	{ "popterm",          NULL,               NULL,       ~0,        True,        -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

#include "column.c"
#include "deck.c"
#include "gaplessgrid.c"
#include "horizgrid.c"
#include "bstack.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */   
	{ "[M]",      monocle },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      horizgrid },
	{ "###",      gaplessgrid },
	{ "||=",      col },
	{ "[]_",      deck },
	{ "><>",      NULL },    /* no layout function means floating behavior */
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
static char dmenumon[2]          = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]    = { "run-recent", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]     = { "st", "-e", "tmux", "-f", ".config/tmux/tmux.conf", NULL };
static const char *j4dmenu[]     = { "fondler.sh", "j4", dmenumon, NULL };
static const char *brightdown[]  = { "fondler.sh", "brightdown", NULL };
static const char *brightup[]    = { "fondler.sh", "brightup", NULL };
static const char *popterm[]     = { "popterm", "tmux", "-f", ".config/tmux/tmux.conf", NULL };
static const char *micmute[]     = { "amixer", "-q", "set", "Capture", "nocap", NULL };
static const char *micunmute[]   = { "amixer", "-q", "set", "Capture", "cap", NULL };
static const char *mute[]        = { "amixer", "-q", "set", "Master", "toggle", NULL };
static const char *newbg[]       = { "systemctl", "--user", "start", "feh-wallpaper.service", "feh-wallpaper.timer", NULL };
static const char *stopbg[]      = { "systemctl", "--user", "stop", "feh-wallpaper.timer", NULL };
static const char *openurl[]     = { "fondler.sh", "browser", NULL };
static const char *maim[]        = { "fondler.sh", "maim", NULL };
static const char *trans[]       = { "trans-exempt.sh", NULL };
static const char *voldown[]     = { "fondler.sh", "voldown", NULL };
static const char *volup[]       = { "fondler.sh", "volup", NULL };
static const char *quitWM[]      = { "touch", ".cache/quitWM", NULL };

static Key keys[] = {
	/* modifier,           key,                       function,        argument */
// Core Commands
	{ MODKEY|ShiftMask,    XK_Return,                 spawn,           {.v = termcmd } },
	{ MODKEY,              XK_space,                  spawn,           {.v = j4dmenu } },
	{ MODKEY|Mod1Mask,     XK_space,                  spawn,           {.v = dmenucmd } },
	{ MODKEY|ControlMask,  XK_q,                      spawn,           {.v = quitWM } },
	{ MODKEY|ShiftMask,    XK_q,                      quit,            {0} },
// Environment Commands
	{ MODKEY|ControlMask,  XK_Return,                 spawn,           {.v = popterm } },
	{ 0,                   XK_Print,                  spawn,           {.v = maim } },
	{ MODKEY,              XK_e,                      spawn,           {.v = trans } },
	{ MODKEY,              XK_u,                      spawn,           {.v = openurl } },
	{ MODKEY|ControlMask,  XK_r,                      spawn,           {.v = newbg } },
	{ MODKEY|ShiftMask,    XK_r,                      spawn,           {.v = stopbg } },
	{ MODKEY,              XK_Home,                   spawn,           {.v = micunmute } },
	{ MODKEY,              XK_End,                    spawn,           {.v = micmute } },
// Laptop
	{ 0,                   XF86XK_AudioMute,          spawn,           {.v = mute } },
	{ 0,                   XF86XK_AudioLowerVolume,   spawn,           {.v = voldown } },
	{ 0,                   XF86XK_AudioRaiseVolume,   spawn,           {.v = volup } },
	{ 0,                   XF86XK_MonBrightnessDown,  spawn,           {.v = brightdown } },
	{ 0,                   XF86XK_MonBrightnessUp,    spawn,           {.v = brightup } },
// Layouts
	{ MODKEY,              XK_F1,                     setlayout,       {.v = &layouts[0]} }, //tile
	{ MODKEY,              XK_F2,                     setlayout,       {.v = &layouts[1]} }, //monocle
	{ MODKEY,              XK_F3,                     setlayout,       {.v = &layouts[2]} }, //bstack
	{ MODKEY,              XK_F4,                     setlayout,       {.v = &layouts[3]} }, //bstackhoriz
	{ MODKEY,              XK_F5,                     setlayout,       {.v = &layouts[4]} }, //horizgrid
	{ MODKEY,              XK_F6,                     setlayout,       {.v = &layouts[5]} }, //gaplessgrid
	{ MODKEY,              XK_F7,                     setlayout,       {.v = &layouts[6]} }, //col
	{ MODKEY,              XK_F8,                     setlayout,       {.v = &layouts[7]} }, //deck
	{ MODKEY,              XK_F9,                     setlayout,       {.v = &layouts[8]} }, //float
	{ MODKEY|ShiftMask,    XK_Tab,                    setlayout,       {0} },
	{ MODKEY,              XK_Tab,                    view,            {0} },
	{ MODKEY,              XK_0,                      view,            {.ui = ~0 } },
	{ MODKEY|ControlMask,  XK_Escape,                 tabmode,         {-1} }, //tab patch
	{ MODKEY|ShiftMask,    XK_Escape,                 togglebar,       {0} },
// Navigation
	{ MODKEY,              XK_bracketleft,            focusstack,      {.i = +1 } }, //hjkl 3rd finger down
	{ MODKEY,              XK_bracketright,           focusstack,      {.i = -1 } }, //hjkl 4th finger up
	{ MODKEY,              XK_Return,                 zoom,            {0} },
	{ MODKEY|ControlMask,  XK_bracketleft,            focusmon,        {.i = -1 } },
	{ MODKEY|ControlMask,  XK_bracketright,           focusmon,        {.i = +1 } },
// Manipulation
	{ MODKEY,              XK_p,                      setmfact,        {.f = -0.05} },
	{ MODKEY,              XK_backslash,              setmfact,        {.f = +0.05} },
	{ MODKEY,              XK_d,                      incnmaster,      {.i = -1 } },
	{ MODKEY,              XK_i,                      incnmaster,      {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_0,                      tag,             {.ui = ~0 } },
	{ MODKEY|ShiftMask,    XK_bracketleft,            tagmon,          {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_bracketright,           tagmon,          {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_space,                  togglefloating,  {0} },
	{ MODKEY|ShiftMask,    XK_c,                      killclient,      {0} },

	TAGKEYS(               XK_1,                      0)
	TAGKEYS(               XK_2,                      1)
	TAGKEYS(               XK_3,                      2)
	TAGKEYS(               XK_4,                      3)
	TAGKEYS(               XK_5,                      4)
	TAGKEYS(               XK_6,                      5)
	TAGKEYS(               XK_7,                      6)
	TAGKEYS(               XK_8,                      7)
	TAGKEYS(               XK_9,                      8)
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click,         event mask,  button,   function,        argument */
	{ ClkLtSymbol,    0,           Button1,  setlayout,       {0} },
	{ ClkLtSymbol,    0,           Button3,  setlayout,       {.v = &layouts[2]} },
	{ ClkWinTitle,    0,           Button2,  zoom,            {0} },
//	{ ClkStatusText,  0,           Button2,  spawn,           {.v = termcmd } },
	{ ClkClientWin,   MODKEY,      Button1,  movemouse,       {0} },
	{ ClkClientWin,   MODKEY,      Button2,  togglefloating,  {0} },
	{ ClkClientWin,   MODKEY,      Button3,  resizemouse,     {0} },
	{ ClkTagBar,      0,           Button1,  view,            {0} },
	{ ClkTagBar,      0,           Button3,  toggleview,      {0} },
	{ ClkTagBar,      MODKEY,      Button1,  tag,             {0} },
	{ ClkTagBar,      MODKEY,      Button3,  toggletag,       {0} },
	{ ClkTabBar,      0,           Button1,  focuswin,        {0} }, //tab patch
};

