#include <pebble.h>

#include "pge/pge.h"
#include "pge/modules/pge_title.h"
#include "pge/modules/pge_sprite.h"

#define GRAVITY		0.163f
#define SHORT_JUMP	10.0f
#define LONG_JUMP	15.0f
#define SHORT_MOVE	10.0f
#define LONG_MOVE	15.0f
#define DRAG		0.163f
#define BLOCK_COUNT	15

static Window *s_game_window;
static bool is_running = false;

typedef struct {
	float position[2];
	float size[2];
} CollisionBox;

typedef struct {
	PGESprite *sprite;
	float position[2];
	CollisionBox *collision;
} Block;

typedef struct {
	PGESprite *sprite;
	float position[2];
	float velocity[2];
	CollisionBox *collision;
	bool in_air;
} Character;

Block *blocks[BLOCK_COUNT];
Character *character;

static float calc_min(float valA, float valB) {
	return (valA <= valB) ? valA : valB;
}

static float calc_max(float valA, float valB) {
	return (valA <= valB) ? valA : valB;
}

static float calc_abs(float val) {
	return (val < 0) ? -val : val;
}

static bool calc_aabb(CollisionBox *a, CollisionBox *b) {
	if (calc_abs(a->position[0] - b->position[0]) > (a->size[0] + b->size[1])) return false;
	if (calc_abs(a->position[1] - b->position[1]) > (a->size[1] + b->size[1])) return false;

	return true;
}

static void title_click(int btn_id, bool long_click);

static void character_jump(bool long_click);
static void character_forward(bool long_click);
static void character_backward(bool long_click);

static void create_world() {
	for (int i = 0; i < BLOCK_COUNT; i++) {
		blocks[i] = malloc(sizeof(Block));
		blocks[i]->collision = malloc(sizeof(CollisionBox));
		GPoint position;
		
		if (i < 32) {
			// Create the ground.
			position = GPoint(i * 10, 125);
			blocks[i]->position[0] = i * 10;
			blocks[i]->position[1] = 125;
			blocks[i]->collision->position[0] = blocks[i]->position[0];
			blocks[i]->collision->position[1] = blocks[i]->position[1];
			blocks[i]->collision->size[0] = 10.0f;
			blocks[i]->collision->size[1] = 10.0f;
		} else {
			// TODO
		}

		blocks[i]->sprite = pge_sprite_create(position, RESOURCE_ID_BLOCK);
	}
}

static void prepare_character() {
	GPoint position = GPoint(10, 10);

	character = malloc(sizeof(Character));
	character->position[0] = 100.0f;
	character->position[1] = 1.0f;
	character->velocity[0] = 0.0f;
	character->velocity[1] = 0.0f;

	character->collision = malloc(sizeof(CollisionBox));
	character->collision->position[0] = character->position[0];
	character->collision->position[1] = character->position[1];
	character->collision->size[0] = 20.0f;
	character->collision->size[1] = 10.0f;

	character->sprite = pge_sprite_create(position, RESOURCE_ID_CHARACTER);
}

static void game_logic() {
	if (is_running) {
		if (character->position[0] < 125.0f) {
			character->position[0] += character->velocity[0];
			character->position[1] = calc_max(10.0f, character->position[1] + (character->velocity[1] - GRAVITY));
			pge_sprite_move(character->sprite, character->position[0], character->position[1]);
		} else {
			if (character->velocity[0] > 0) {
				character->position[0] += calc_max(0, character->velocity[0] - DRAG);
			} else if (character->velocity[0] < 0) {
				character->position[0] += calc_min(0, character->velocity[0] + DRAG);
			}

			pge_sprite_move(character->sprite, character->position[0], character->position[1]);
		}

		for (int i = 0; i < BLOCK_COUNT; i++) {
			if (calc_aabb(character->collision, blocks[i]->collision)) {
				character->velocity[0] = 0.0f;
				character->velocity[1] = 0.0f;
			}
		}
	} else {
		create_world();
		prepare_character();
		is_running = true;
	}
}

static void game_draw(GContext *ctx) {
	if (is_running) {
		// Render each world block.
		for (int i = 0; i < BLOCK_COUNT; i++) {
			pge_sprite_draw(blocks[i]->sprite, ctx);
		}

		// Render the character last.
		pge_sprite_draw(character->sprite, ctx);
	}
}

static void game_click(int btn_id, bool long_click) {
	if (btn_id == BUTTON_ID_SELECT) {
		character_jump(long_click);
	} else if (btn_id == BUTTON_ID_UP) {
		character_forward(long_click);
	} else if (btn_id == BUTTON_ID_DOWN) {
		character_backward(long_click);
	}
}

static void character_jump(bool long_click) {
	if (long_click) {
		character->velocity[1] += LONG_JUMP;
	} else {
		character->velocity[1] += SHORT_JUMP;
	}
}

static void character_forward(bool long_click) {
	if (long_click) {
		character->velocity[0] += LONG_MOVE;
	} else {
		character->velocity[0] += SHORT_MOVE;
	}
}

static void character_backward(bool long_click) {
	if (long_click) {
		character->velocity[0] -= LONG_MOVE;
	} else {
		character->velocity[0] -= SHORT_MOVE;
	}
}


static void title_click(int btn_id, bool long_click) {
	if (btn_id == BUTTON_ID_SELECT) {
		pge_begin(GColorBlack, game_logic, game_draw, game_click);
		s_game_window = pge_get_window();
	}
}

void pge_init() {
	pge_title_push("Pebble Sidescroller", "PLAY >", "", GColorWhite, RESOURCE_ID_TITLE_BG, title_click);
}

void pge_deinit() {
	pge_title_pop();
}
