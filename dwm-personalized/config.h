/* mee LICENSE file for copyright and license details. */

#include "gaplessgrid.c"
#include "bstack.c"
#include "push.c"
#include "moveresize.c"
#include "bstackhoriz.c"
#include <X11/XF86keysym.h>
/* appearance */
static const char *fonts[] = {
	"fixed:size=7"
};
static const char dmenufont[]       = "-misc-fixed-medium-r-normal-*-9-*-*-*-*-*-iso10646-*";
static const char normbordercolor[] = "#444444";
static const char normbgcolor[]     = "#222222";
static const char normfgcolor[]     = "#bbbbbb";
//static const char selbordercolor[]  = "#005577";
static const char selbordercolor[]  = "#ff4500";
static const char selbgcolor[]      = "#005577";
static const char selfgcolor[]      = "#eeeeee";
static const char floatnormbordercolor[] = "#005577";
static const char floatselbordercolor[]  = "#ff1493";
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */
/* Run or raise */
//static const char *termcmd[] = { "st", "-c", "st-256color", "-t", "st-256color", "-e", "dvtm", "-M", NULL, NULL, NULL, "st-256color" };
//static const char *browser[] = { "firefox", NULL, NULL, NULL, "Firefox" };
static const char *browser[] = { "qutebrowser", NULL, NULL, NULL, "qutebrowser" };
//static const char *irc[] = { "st", "-c", "irssi", "-t", "irssi", "-e", "abduco", "-A", "irc", "irssi", NULL, NULL, NULL, "irssi" };
static const char *irc[] = { "st", "-c", "ircterm", "-t", "ircterm", "-e", "abduco", "-A", "irc", "dvtm", NULL, NULL, NULL, "ircterm" };

/* tagging */
#define MAX_TAGLEN 16
static char tags[][MAX_TAGLEN] = { "cmd", "www", "irc", "etc" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class         instance       title       tags mask     isfloating   monitor */
	//cmd
	{ "st-256color", "st-256color", NULL,       1,            False,       -1 },
	//www
	{ "Firefox",     NULL,          NULL,       1 << 1,       False,       -1 },
	{ "Dwb",         NULL,          NULL,       1 << 1,       False,       -1 },
	{ "qutebrowser", "qutebrowser", NULL,       1 << 1,       False,       -1 },
	{ "Xombrero",    NULL,          NULL,       1 << 1,       False,       -1 },
	{ "Chromium",    NULL,          NULL,       1 << 1,       False,       -1 },
	//irc
	{ "irssi",       NULL,          NULL,       1 << 2,       False,       -1 },
	{ "ircterm",     NULL,          NULL,       1 << 2,       False,       -1 },
	//etc
	{ "Client.pyw",  NULL,          NULL,       1 << 3,       False,       -1 },
	{ "Steam",       "Steam",       NULL,       1 << 3,       False,       -1 },
	{ "Steam",       "Steam",       "Friends",  1 << 3,       False,       -1 },
	//all
	{ "mpv",         NULL,          NULL,      ~0,            True,        -1 },
	{ NULL,          NULL,          "qiv",     ~0,            True,        -1 },
	{ "Sxiv",        NULL,          NULL,      ~0,            True,        -1 },
	{ "Gifview",     NULL,          NULL,      ~0,            True,        -1 },
};

/* layout(s) */
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = False; /* True means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[M]",      monocle }, /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[]=",      tile },    
        { "###",      gaplessgrid },
	{ "TTT",      bstack },
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
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]   = { "run-recent", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]    = { "st", "-c", "st-256color", "-e", "dvtm", "-M", NULL }; //Mutually exclusive with run or raise termcmd
static const char *openurl[]    = { "browser.sh", NULL };
static const char *clipmenu[]   = { "clipmenu", NULL };
static const char *clipsync[]   = { "clipsync.sh", NULL };
static const char *newbg[]      = { "systemctl", "--user", "start", "feh-wallpaper.service", NULL };
static const char *lightdown[]  = { "bright_adjust.sh", "dec", NULL };
static const char *lightup[]    = { "bright_adjust.sh", "inc", NULL };
static const char *rotate[]     = { "rotate.sh", NULL };
static const char *voldown[]    = { "vol_adjust.sh", "-", NULL };
static const char *volup[]      = { "vol_adjust.sh", "+", NULL };
static const char *mute[]       = { "amixer", "-q", "sset", "Master", "toggle", NULL };

static Key keys[] = {
	/* modifier           key                       function        argument */
	{ 0,                  XF86XK_MonBrightnessDown, spawn,          {.v = lightdown } },
	{ 0,                  XF86XK_MonBrightnessUp,   spawn,          {.v = lightup } },
	{ 0,                  XF86XK_Display,           spawn,          {.v = rotate } },
	{ 0,                  XF86XK_AudioLowerVolume,  spawn,          {.v = voldown } },
	{ 0,                  XF86XK_AudioRaiseVolume,  spawn,          {.v = volup } },
	{ 0,                  XF86XK_AudioMute,         spawn,          {.v = mute } },
        { MODKEY,             XK_backslash,             runorraise,     {.v = irc } },
	{ MODKEY,             XK_comma,                 focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,   XK_comma,                 tagmon,         {.i = -1 } },
	{ ControlMask,        XK_grave,                 spawn,          {.v = clipmenu } },
	{ ControlMask,        XK_Insert,                spawn,          {.v = clipsync } },
	{ MODKEY,             XK_period,                focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,   XK_period,                tagmon,         {.i = +1 } },
	{ MODKEY,             XK_Return,                zoom,           {0} },
//	{ MODKEY|ShiftMask,   XK_Return,                runorraise,     {.v = termcmd } },
	{ MODKEY|ShiftMask,   XK_Return,                spawn,          {.v = termcmd } },
	{ MODKEY,             XK_slash,                 togglebar,      {0} },
	{ MODKEY,             XK_space,                 setlayout,      {0} },
	{ MODKEY|ShiftMask,   XK_space,                 togglefloating, {0} },
	{ MODKEY,             XK_Tab,                   view,           {0} },
	{ MODKEY,             XK_Down,                  moveresize,     {.v = (int []){ 0, 25, 0, 0 }}},
	{ MODKEY|ShiftMask,   XK_Down,                  moveresize,     {.v = (int []){ 0, 0, 0, 25 }}},
	{ MODKEY,             XK_Left,                  moveresize,     {.v = (int []){ -25, 0, 0, 0 }}},
	{ MODKEY|ShiftMask,   XK_Left,                  moveresize,     {.v = (int []){ 0, 0, -25, 0 }}},
	{ MODKEY,             XK_Right,                 moveresize,     {.v = (int []){ 25, 0, 0, 0 }}},
	{ MODKEY|ShiftMask,   XK_Right,                 moveresize,     {.v = (int []){ 0, 0, 25, 0 }}},
	{ MODKEY,             XK_Up,                    moveresize,     {.v = (int []){ 0, -25, 0, 0 }}},
	{ MODKEY|ShiftMask,   XK_Up,                    moveresize,     {.v = (int []){ 0, 0, 0, -25 }}},
        { MODKEY,             XK_b,                     setlayout,      {.v = &layouts[4]} },
        { MODKEY|ShiftMask,   XK_b,                     setlayout,      {.v = &layouts[5]} },
	{ MODKEY|ShiftMask,   XK_c,                     killclient,     {0} },
	{ MODKEY,             XK_d,                     incnmaster,     {.i = -1 } },
	{ MODKEY,             XK_f,                     setlayout,      {.v = &layouts[1]} },
        { MODKEY,             XK_g,                     setlayout,      {.v = &layouts[3]} },
	{ MODKEY,             XK_h,                     setmfact,       {.f = -0.05} },
	{ MODKEY|ControlMask, XK_h,                     setcfact,       {.f = +0.25} },
	{ MODKEY,             XK_i,                     incnmaster,     {.i = +1 } },
	{ MODKEY,             XK_j,                     focusstack,     {.i = +1 } },
	{ MODKEY|ControlMask, XK_j,                     pushdown,       {0} },
	{ MODKEY,             XK_k,                     focusstack,     {.i = -1 } },
	{ MODKEY|ControlMask, XK_k,                     pushup,         {0} },
	{ MODKEY,             XK_l,                     setmfact,       {.f = +0.05} },
	{ MODKEY|ControlMask, XK_l,                     setcfact,       {.f = -0.25} },
	{ MODKEY,             XK_m,                     setlayout,      {.v = &layouts[0]} },
        { MODKEY,             XK_o,                     runorraise,     {.v = browser } },
	{ MODKEY|ControlMask, XK_o,                     setcfact,       {.f =  0.00} },
	{ MODKEY,             XK_p,                     spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,   XK_q,                     quit,           {0} },
	{ MODKEY|ControlMask, XK_r,                     spawn,          {.v = newbg } },
	{ MODKEY|ShiftMask,   XK_r,                     resizemouse,    {0} },
	{ MODKEY,             XK_t,                     setlayout,      {.v = &layouts[2]} },
	{ MODKEY,             XK_u,                     spawn,          {.v = openurl } },
	{ MODKEY,             XK_0,                     view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,   XK_0,                     tag,            {.ui = ~0 } },
	TAGKEYS(              XK_1,                      0)
	TAGKEYS(              XK_2,                      1)
	TAGKEYS(              XK_3,                      2)
	TAGKEYS(              XK_4,                      3)
	TAGKEYS(              XK_5,                      4)
	TAGKEYS(              XK_6,                      5)
	TAGKEYS(              XK_7,                      6)
	TAGKEYS(              XK_8,                      7)
	TAGKEYS(              XK_9,                      8)
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
//	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
