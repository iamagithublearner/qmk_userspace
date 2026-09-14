#include QMK_KEYBOARD_H
#include "timer.h"
#include "rgblight.h"
//#include "autocorrect_data.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_all(
        QK_GESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC,    KC_DEL, KC_PSCR,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,             KC_BRID, KC_BRIU,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,  RSFT_T(KC_CAPS), RSFT_T(KC_CAPS),     KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,          KC_SPC,           KC_MPLY,           KC_SPC,           MO(1), MO(2),   MO(3),             KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT_all(
        _______, KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______,    _______,_______,
        _______, MS_BTN1, MS_UP, MS_BTN2, _______, _______, _______, RGB_M_SW, RGB_M_SN, _______, _______, _______, _______, VK_TOGG,             _______, _______,
        AC_TOGG, MS_LEFT, MS_DOWN, MS_RGHT, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             KC_VOLU,
        _______, _______, _______,          _______,          _______,          _______,          XXXXXXX, XXXXXXX, XXXXXXX,             KC_MPRV, KC_VOLD, KC_MNXT
    ),
    [2] = LAYOUT_all(
        _______, _______, _______, KC_F13, KC_F14, KC_F15, KC_F16, KC_F17, KC_F18, KC_F19, KC_F20, _______, _______,  _______, _______,            _______,_______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             UG_VALD, UG_VALU,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             UG_HUEU,
        _______, _______, _______,          _______,          UG_TOGG,          _______,          XXXXXXX, XXXXXXX, XXXXXXX,             UG_PREV, UG_HUED, UG_NEXT
    ),
    [3] = LAYOUT_all(
        QK_BOOT, PB_1,    PB_2,    PB_3,     PB_4,    PB_5,   PB_6,    PB_7,    PB_8,    PB_9,    PB_10,   _______,  _______, _______, _______,    DM_REC1,DM_REC2,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             DM_PLY1, DM_PLY2,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             _______,
        _______, _______, _______,          _______,          DM_RSTP,          _______,          XXXXXXX, XXXXXXX, XXXXXXX,             _______, _______, _______
    ),
};
// ==================================================
// RGB Light status layers
// ==================================================

enum rgb_status_layers {
    RGB_LAYER_DYNAMIC_RECORD,
    RGB_LAYER_DYNAMIC_FLASH,
    RGB_LAYER_DYNAMIC_STOP,
    RGB_LAYER_AUTOCORRECT_ON,
    RGB_LAYER_AUTOCORRECT_OFF,
    narayan,
};

const rgblight_segment_t PROGMEM rgb_dynamic_record_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, RGBLIGHT_LED_COUNT, HSV_GREEN}
);

const rgblight_segment_t PROGMEM rgb_dynamic_flash_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, RGBLIGHT_LED_COUNT, HSV_WHITE}
);

const rgblight_segment_t PROGMEM rgb_dynamic_stop_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, RGBLIGHT_LED_COUNT, HSV_RED}
);

const rgblight_segment_t PROGMEM rgb_autocorrect_on_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, RGBLIGHT_LED_COUNT, HSV_GREEN}
);

const rgblight_segment_t PROGMEM rgb_autocorrect_off_layer[] = RGBLIGHT_LAYER_SEGMENTS(
    {0, RGBLIGHT_LED_COUNT, HSV_RED}
);

// Later entries take precedence when layers overlap.
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    rgb_dynamic_record_layer,
    rgb_dynamic_flash_layer,
    rgb_dynamic_stop_layer,
    rgb_autocorrect_on_layer,
    rgb_autocorrect_off_layer
);

void keyboard_post_init_user(void) {
    rgblight_layers = my_rgb_layers;
}

// ==================================================
// Dynamic Macro LED feedback
// ==================================================

static bool dynamic_macro_recording = false;
static uint16_t dynamic_macro_flash_timer = 0;

// Used for the red "recording stopped" animation
static bool dynamic_macro_end_animation = false;
static uint16_t dynamic_macro_end_timer = 0;


bool dynamic_macro_record_start_user(int8_t direction) {
    dynamic_macro_recording = true;
    dynamic_macro_end_animation = false;

    // Clear any stale indicator layers, then show green while recording.
    rgblight_set_layer_state(RGB_LAYER_DYNAMIC_FLASH, false);
    rgblight_set_layer_state(RGB_LAYER_DYNAMIC_STOP, false);
    rgblight_set_layer_state(RGB_LAYER_DYNAMIC_RECORD, true);

    return true;
}


bool dynamic_macro_record_key_user(int8_t direction, keyrecord_t *record) {
    // Flash white on key press.
    if (record->event.pressed) {
        rgblight_set_layer_state(RGB_LAYER_DYNAMIC_FLASH, true);
        dynamic_macro_flash_timer = timer_read();
    }

    return true;
}


bool dynamic_macro_record_end_user(int8_t direction) {
    dynamic_macro_recording = false;

    // Stop the recording layer and show solid red.
    rgblight_set_layer_state(RGB_LAYER_DYNAMIC_RECORD, false);
    rgblight_set_layer_state(RGB_LAYER_DYNAMIC_FLASH, false);
    rgblight_set_layer_state(RGB_LAYER_DYNAMIC_STOP, true);

    // Keep red visible for 300 ms.
    dynamic_macro_end_animation = true;
    dynamic_macro_end_timer = timer_read();

    return true;
}


// ==================================================
// Autocorrect LED feedback
// ==================================================

static bool autocorrect_led_animation = false;
static uint16_t autocorrect_led_timer = 0;


static void autocorrect_led_on(void) {
    // Autocorrect ON -> green
    rgblight_set_layer_state(RGB_LAYER_AUTOCORRECT_OFF, false);
    rgblight_set_layer_state(RGB_LAYER_AUTOCORRECT_ON, true);

    autocorrect_led_animation = true;
    autocorrect_led_timer = timer_read();
}


static void autocorrect_led_off(void) {
    // Autocorrect OFF -> red
    rgblight_set_layer_state(RGB_LAYER_AUTOCORRECT_ON, false);
    rgblight_set_layer_state(RGB_LAYER_AUTOCORRECT_OFF, true);

    autocorrect_led_animation = true;
    autocorrect_led_timer = timer_read();
}


void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return;
    }

    switch (keycode) {
        case AC_TOGG:
            // QMK has already toggled Autocorrect here.
            if (autocorrect_is_enabled()) {
                autocorrect_led_on();
            } else {
                autocorrect_led_off();
            }
            break;
    }
}


// ==================================================
// Timers
// ==================================================

void matrix_scan_user(void) {

    // --------------------------------------------------
    // Dynamic Macro white flash
    // --------------------------------------------------

    if (dynamic_macro_recording &&
        timer_elapsed(dynamic_macro_flash_timer) >= 100) {

        // White flash finished -> back to green.
        rgblight_set_layer_state(RGB_LAYER_DYNAMIC_FLASH, false);
    }


    // --------------------------------------------------
    // Dynamic Macro stop animation
    // --------------------------------------------------

    if (dynamic_macro_end_animation &&
        timer_elapsed(dynamic_macro_end_timer) >= 300) {

        dynamic_macro_end_animation = false;

        // Red animation finished -> clear the indicator layer.
        rgblight_set_layer_state(RGB_LAYER_DYNAMIC_STOP, false);
    }


    // --------------------------------------------------
    // Autocorrect animation
    // --------------------------------------------------

    if (autocorrect_led_animation &&
        timer_elapsed(autocorrect_led_timer) >= 300) {

        autocorrect_led_animation = false;

        // Animation finished -> clear the indicator layer.
        rgblight_set_layer_state(RGB_LAYER_AUTOCORRECT_ON, false);
        rgblight_set_layer_state(RGB_LAYER_AUTOCORRECT_OFF, false);
    }
}
