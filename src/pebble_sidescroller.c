#include <pebble.h>

#include "pge/pge.h"
#include "pge/modules/pge_title.h"

static Window *s_game_window;

static void game_logic() {
	// TODO
}

static void game_draw(GContext *ctx) {
	// TODO
}

static void game_click(int btn_id, bool long_click) {
	// TODO
}

static void about_logic() {
	// TODO
}

static void about_draw(GContext *ctx) {
	// TODO
}

static void about_click(int btn_id, bool long_click) {
	// TODO
}

static void title_click(int btn_id, bool long_click) {
	if (btn_id == BUTTON_ID_SELECT) {
		pge_begin(GColorBlack, game_logic, game_draw, game_click);
		s_game_window = pge_get_window();
	} else if (btn_id == BUTTON_ID_DOWN) {
		pge_begin(GColorBlack, about_logic, about_draw, about_click);
	}
}

void pge_init() {
	pge_title_push("Pebble Sidescroller", "PLAY >", "ABOUT >", GColorWhite, RESOURCE_ID_TITLE_BG, title_click);
}

void pge_deinit() {
	pge_title_pop();
}
