#include "gui_port.h"

//++++++++++++++++++++++++页面定义+++++++++++++++++++++
static PageStruct_t page_logo = {
	.page_id						=	PAGE_LOGO,
	.init_handler				=	PAGE_INIT_DEF(logo),
	.focus_handler			= PAGE_FOCUS_DEF(logo)
};

static PageStruct_t page_main = {
	.page_id						=	PAGE_MAIN,
	.init_handler				=	PAGE_INIT_DEF(main),
	.focus_handler			= PAGE_FOCUS_DEF(main)
};

static PageStruct_t page_apps = {
	.page_id						=	PAGE_APPS,
	.init_handler				=	PAGE_INIT_DEF(apps),
	.focus_handler			= PAGE_FOCUS_DEF(apps)
};

static PageStruct_t page_about = {
	.page_id						=	PAGE_APP_ABOUT,
	.init_handler				=	PAGE_INIT_DEF(about),
	.focus_handler			= PAGE_FOCUS_DEF(about)
};

//static PageStruct_t page_music = {
//	.page_id						=	PAGE_APP_MUSIC,
//	.init_handler				=	PAGE_INIT_DEF(music),
//	.focus_handler			= PAGE_FOCUS_DEF(music)
//};

static PageStruct_t page_book = {
	.page_id						=	PAGE_APP_BOOK,
	.init_handler				=	PAGE_INIT_DEF(book),
	.focus_handler			= PAGE_FOCUS_DEF(book)
};

//static PageStruct_t page_fm = {
//	.page_id						=	PAGE_APP_FM,
//	.init_handler				=	PAGE_INIT_DEF(fm),
//	.focus_handler			= PAGE_FOCUS_DEF(fm)
//};

static PageStruct_t page_arduboy = {
	.page_id						=	PAGE_APP_ARDUBOY,
	.init_handler				=	PAGE_INIT_DEF(arduboy),
	.focus_handler			= PAGE_FOCUS_DEF(arduboy)
};

static PageStruct_t page_nes = {
	.page_id						=	PAGE_APP_NES,
	.init_handler				=	PAGE_INIT_DEF(nes),
	.focus_handler			= PAGE_FOCUS_DEF(nes)
};

static PageStruct_t page_gps = {
	.page_id						=	PAGE_APP_GPS,
	.init_handler				=	PAGE_INIT_DEF(gps),
	.focus_handler			= PAGE_FOCUS_DEF(gps)
};

static PageStruct_t page_setting = {
	.page_id						=	PAGE_APP_SETTING,
	.init_handler				=	PAGE_INIT_DEF(setting),
	.focus_handler			= PAGE_FOCUS_DEF(setting)
};

static PageStruct_t page_calc = {
	.page_id						=	PAGE_APP_CALC,
	.init_handler				=	PAGE_INIT_DEF(calc),
	.focus_handler			= PAGE_FOCUS_DEF(calc)
};

static PageStruct_t page_calendar = {
	.page_id						=	PAGE_APP_CALENDAR,
	.init_handler				=	PAGE_INIT_DEF(calendar),
	.focus_handler			= PAGE_FOCUS_DEF(calendar)
};
//++++++++++++++++++++++++页面定义+++++++++++++++++++++





