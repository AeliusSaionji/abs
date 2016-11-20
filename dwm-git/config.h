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
static const char *colors[SchemeLast][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel] =  { col_gray4, col_cyan,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "cmd", "www", "irc", "dvr", "gog", "etc" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class,           instance,                                title,      tags mask,  isfloating,  monitor */
//cmd
	{ "st-256color",    "st-256color",                           NULL,       1,          False,       -1 },
//www
	{ "Chromium",       "chromium",                              NULL,       1 << 1,     False,       -1 },
	{ "Chromium",       "crx_pfpeapihoiogbcmdmnibeplnikfnhoge",  NULL,       1 << 1,     False,       -1 },
	{ "Firefox",        NULL,                                    NULL,       1 << 1,     False,       -1 },
	{ "qutebrowser",    "qutebrowser",                           NULL,       1 << 1,     False,       -1 },
//irc
	{ "ircterm",        NULL,                                    NULL,       1 << 2,     False,       -1 },
	{ "skypeforlinux",  "skypeforlinux",                         NULL,       1 << 2,     False,       -1 },
//dvr
	{ "Chromium",       "crx_deceagebecbceejblnlcjooeohmmeldh",  NULL,       1 << 3,     False,       -1 },
	{ "Chromium",       "crx_fpniocchabmgenibceglhnfeimmdhdfm",  NULL,       1 << 3,     False,       -1 },
//gog
	{ "steam",          "steam",                                 NULL,       1 << 4,     False,       -1 },       // Big Picture Mode
	{ "Steam",          "Steam",                                 NULL,       1 << 4,     False,       -1 },
	{ "Steam",          "Steam",                                 "Friends",  1 << 4,     False,       -1 },
//etc
	{ "Client.pyw",     "client.pyw",                            NULL,       1 << 5,     False,       -1 },
	{ "xfreerdp",       "xfreerdp",                              NULL,       1 << 5,     False,       -1 },
	{ "Okular",         "okular",                                NULL,       1 << 5,     False,       -1 },
	{ "Deluge",         "deluge",                                NULL,       1 << 5,     False,       -1 },
//all
	{ "mpv",            NULL,                                    NULL,       ~0,         True,        -1 },
	{ "Sxiv",           NULL,                                    NULL,       ~0,         True,        -1 },
	{ "popterm",        NULL,                                    NULL,       ~0,         True,        -1 },
	{ "qutebrowser",    "popwww",                                NULL,       ~0,         True,        -1 },
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
	{ "[M]",      monocle }, /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[]=",      tile },    
	{ "TTT",      bstack },
	{ "HHH",      horizgrid },
	{ "###",      gaplessgrid },
	{ "[]_",      deck },
	{ "||=",      col },
	{ "===",      bstackhoriz },
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
static const char *termcmd[]     = { "st", NULL };
static const char *clipmenu[]    = { "clipmenu",   "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, "-i", NULL };
static const char *j4dmenu[]     = { "fondler.sh", "j4", dmenumon, NULL };
static const char *brightdown[]  = { "fondler.sh", "brightdown", NULL };
static const char *brightup[]    = { "fondler.sh", "brightup", NULL };
static const char *dunsttoggle[] = { "fondler.sh", "dunsttoggle", NULL };
static const char *lock[]        = { "fondler.sh", "lock", NULL };
static const char *popterm[]     = { "popterm", "abduco", "-A", "popterm", NULL };
static const char *mute[]        = { "amixer", "-q", "sset", "Master", "toggle", NULL };
static const char *newbg[]       = { "systemctl", "--user", "start", "feh-wallpaper.service", "feh-wallpaper.timer", NULL };
static const char *stopbg[]      = { "systemctl", "--user", "stop", "feh-wallpaper.timer", NULL };
static const char *openurl[]     = { "fondler.sh", "browser", NULL };
static const char *rotate[]      = { "fondler.sh", "rotate", NULL };
static const char *maim[]        = { "maim", NULL };
static const char *trans[]       = { "trans-exempt.sh", NULL };
static const char *dwgrep[]      = { "fondler.sh", "dwgrep", NULL };
static const char *dwtaggrep[]   = { "fondler.sh", "dwtaggrep", NULL };
static const char *voldown[]     = { "fondler.sh", "voldown", NULL };
static const char *volup[]       = { "fondler.sh", "volup", NULL };

static Key keys[] = {
	/* modifier,           key,                       function,        argument */
	{ 0,                   XF86XK_AudioMute,          spawn,           {.v = mute } },
	{ 0,                   XF86XK_AudioLowerVolume,   spawn,           {.v = voldown } },
	{ 0,                   XF86XK_AudioRaiseVolume,   spawn,           {.v = volup } },
	{ 0,                   XF86XK_Battery,            spawn,           {.v = dunsttoggle } },
	{ 0,                   XF86XK_Display,            spawn,           {.v = rotate } },
	{ 0,                   XF86XK_MonBrightnessDown,  spawn,           {.v = brightdown } },
	{ 0,                   XF86XK_MonBrightnessUp,    spawn,           {.v = brightup } },
	{ 0,                   XF86XK_Sleep,              spawn,           {.v = lock } },
	{ ControlMask,         XK_grave,                  spawn,           {.v = clipmenu } },
	{ MODKEY,              XK_backslash,              spawn,           {.v = popterm } },
	{ MODKEY,              XK_Insert,                 spawn,           {.v = maim } },
	{ MODKEY,              XK_e,                      spawn,           {.v = trans } },
	{ MODKEY,              XK_p,                      spawn,           {.v = j4dmenu } },
	{ MODKEY|ShiftMask,    XK_p,                      spawn,           {.v = dmenucmd } },
	{ MODKEY,              XK_u,                      spawn,           {.v = openurl } },
	{ MODKEY|ControlMask,  XK_r,                      spawn,           {.v = newbg } },
	{ MODKEY|ShiftMask,    XK_r,                      spawn,           {.v = stopbg } },
	{ MODKEY,              XK_w,                      spawn,           {.v = dwtaggrep } },
	{ MODKEY|ShiftMask,    XK_w,                      spawn,           {.v = dwgrep } },
	{ MODKEY|ShiftMask,    XK_Return,                 spawn,           {.v = termcmd } },
	{ MODKEY,              XK_space,                  setlayout,       {0} },
	{ MODKEY,              XK_m,                      setlayout,       {.v = &layouts[0]} },
	{ MODKEY,              XK_f,                      setlayout,       {.v = &layouts[1]} },
	{ MODKEY,              XK_t,                      setlayout,       {.v = &layouts[2]} },
        { MODKEY,              XK_b,                      setlayout,       {.v = &layouts[3]} },
        { MODKEY,              XK_g,                      setlayout,       {.v = &layouts[4]} },
        { MODKEY|ShiftMask,    XK_g,                      setlayout,       {.v = &layouts[5]} },
        { MODKEY|ShiftMask,    XK_d,                      setlayout,       {.v = &layouts[6]} },
        { MODKEY,              XK_c,                      setlayout,       {.v = &layouts[7]} },
        { MODKEY,              XK_o,                      setlayout,       {.v = &layouts[8]} },
	{ MODKEY,              XK_Tab,                    view,            {0} },
	{ MODKEY,              XK_0,                      view,            {.ui = ~0 } },
	{ MODKEY,              XK_Return,                 zoom,            {0} },
	{ MODKEY,              XK_k,                      focusstack,      {.i = -1 } },
	{ MODKEY,              XK_j,                      focusstack,      {.i = +1 } },
	{ MODKEY,              XK_comma,                  focusmon,        {.i = -1 } },
	{ MODKEY,              XK_period,                 focusmon,        {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_comma,                  tagmon,          {.i = -1 } },
	{ MODKEY|ShiftMask,    XK_period,                 tagmon,          {.i = +1 } },
	{ MODKEY,              XK_h,                      setmfact,        {.f = -0.05} },
	{ MODKEY,              XK_l,                      setmfact,        {.f = +0.05} },
	{ MODKEY,              XK_d,                      incnmaster,      {.i = -1 } },
	{ MODKEY,              XK_i,                      incnmaster,      {.i = +1 } },
	{ MODKEY|ShiftMask,    XK_0,                      tag,             {.ui = ~0 } },
	{ MODKEY,              XK_slash,                  togglebar,       {0} },
	{ MODKEY|ShiftMask,    XK_space,                  togglefloating,  {0} },
	{ MODKEY|ShiftMask,    XK_c,                      killclient,      {0} },
	{ MODKEY|ShiftMask,    XK_q,                      quit,            {0} },
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
};

