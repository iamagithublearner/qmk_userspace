/* Copyright 2021 wuquestudio
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "timer.h"
#include "rgblight.h"
//#include "autocorrect_data.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_all(
        QK_GESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, KC_BSPC,    KC_DEL, KC_PSCR,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS,             KC_BRID, KC_BRIU,
        QK_LOCK, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,
        KC_LSFT, KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, RSFT_T(KC_CAPS),     KC_UP,
        KC_LCTL, KC_LGUI, KC_LALT,          KC_SPC,           KC_MPLY,           KC_SPC,           MO(1), MO(2),   MO(3),             KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [1] = LAYOUT_all(
        _______, KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______,    _______,_______,
        _______, MS_BTN1, MS_UP, MS_BTN2, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,             _______, _______,
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
// Dynamic Macro LED feedback
// ==================================================

static bool dynamic_macro_recording = false;
static uint16_t dynamic_macro_flash_timer = 0;

// Used for the red "recording stopped" animation
static bool dynamic_macro_end_animation = false;
static uint16_t dynamic_macro_end_timer = 0;

// Saved RGB state
static bool dynamic_macro_saved_enabled;
static uint8_t dynamic_macro_saved_mode;
static uint8_t dynamic_macro_saved_hue;
static uint8_t dynamic_macro_saved_sat;
static uint8_t dynamic_macro_saved_val;
static uint8_t dynamic_macro_saved_speed;


static void dynamic_macro_save_rgb_state(void) {
    dynamic_macro_saved_enabled = rgblight_is_enabled();
    dynamic_macro_saved_mode = rgblight_get_mode();
    dynamic_macro_saved_hue = rgblight_get_hue();
    dynamic_macro_saved_sat = rgblight_get_sat();
    dynamic_macro_saved_val = rgblight_get_val();
    dynamic_macro_saved_speed = rgblight_get_speed();
}


static void dynamic_macro_restore_rgb_state(void) {
    if (dynamic_macro_saved_enabled) {
        rgblight_enable_noeeprom();
    } else {
        rgblight_disable_noeeprom();
    }

    rgblight_sethsv_noeeprom(
        dynamic_macro_saved_hue,
        dynamic_macro_saved_sat,
        dynamic_macro_saved_val
    );

    rgblight_mode_noeeprom(dynamic_macro_saved_mode);
    rgblight_set_speed_noeeprom(dynamic_macro_saved_speed);
}


bool dynamic_macro_record_start_user(int8_t direction) {
    dynamic_macro_recording = true;
    dynamic_macro_end_animation = false;

    // Save the user's current RGB state
    dynamic_macro_save_rgb_state();

    // Recording started -> solid green
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv_noeeprom(HSV_GREEN);

    return true;
}


bool dynamic_macro_record_key_user(int8_t direction, keyrecord_t *record) {
    // Flash white on key press
    if (record->event.pressed) {
        rgblight_enable_noeeprom();
        rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
        rgblight_sethsv_noeeprom(HSV_WHITE);

        dynamic_macro_flash_timer = timer_read();
    }

    return true;
}


bool dynamic_macro_record_end_user(int8_t direction) {
    dynamic_macro_recording = false;

    // Recording stopped -> solid red
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv_noeeprom(HSV_RED);

    // Keep red visible for 300 ms
    dynamic_macro_end_animation = true;
    dynamic_macro_end_timer = timer_read();

    return true;
}


// ==================================================
// Autocorrect LED feedback
// ==================================================

static bool autocorrect_led_animation = false;
static uint16_t autocorrect_led_timer = 0;

// Saved RGB state
static bool autocorrect_saved_enabled;
static uint8_t autocorrect_saved_mode;
static uint8_t autocorrect_saved_hue;
static uint8_t autocorrect_saved_sat;
static uint8_t autocorrect_saved_val;
static uint8_t autocorrect_saved_speed;


static void autocorrect_save_rgb_state(void) {
    autocorrect_saved_enabled = rgblight_is_enabled();
    autocorrect_saved_mode = rgblight_get_mode();
    autocorrect_saved_hue = rgblight_get_hue();
    autocorrect_saved_sat = rgblight_get_sat();
    autocorrect_saved_val = rgblight_get_val();
    autocorrect_saved_speed = rgblight_get_speed();
}


static void autocorrect_restore_rgb_state(void) {
    if (autocorrect_saved_enabled) {
        rgblight_enable_noeeprom();
    } else {
        rgblight_disable_noeeprom();
    }

    rgblight_sethsv_noeeprom(
        autocorrect_saved_hue,
        autocorrect_saved_sat,
        autocorrect_saved_val
    );

    rgblight_mode_noeeprom(autocorrect_saved_mode);
    rgblight_set_speed_noeeprom(autocorrect_saved_speed);
}


static void autocorrect_led_on(void) {
    // Save current RGB state
    autocorrect_save_rgb_state();

    // Autocorrect ON -> green
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv_noeeprom(HSV_GREEN);

    autocorrect_led_animation = true;
    autocorrect_led_timer = timer_read();
}


static void autocorrect_led_off(void) {
    // Save current RGB state
    autocorrect_save_rgb_state();

    // Autocorrect OFF -> red
    rgblight_enable_noeeprom();
    rgblight_mode_noeeprom(RGBLIGHT_MODE_STATIC_LIGHT);
    rgblight_sethsv_noeeprom(HSV_RED);

    autocorrect_led_animation = true;
    autocorrect_led_timer = timer_read();
}


void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return;
    }

    switch (keycode) {
        case AC_TOGG:
            // QMK has already toggled Autocorrect here
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

        // White flash finished -> back to green
        rgblight_sethsv_noeeprom(HSV_GREEN);
    }


    // --------------------------------------------------
    // Dynamic Macro stop animation
    // --------------------------------------------------

    if (dynamic_macro_end_animation &&
        timer_elapsed(dynamic_macro_end_timer) >= 300) {

        dynamic_macro_end_animation = false;

        // Red animation finished -> restore previous RGB state
        dynamic_macro_restore_rgb_state();
    }


    // --------------------------------------------------
    // Autocorrect animation
    // --------------------------------------------------

    if (autocorrect_led_animation &&
        timer_elapsed(autocorrect_led_timer) >= 300) {

        autocorrect_led_animation = false;

        // Animation finished -> restore previous RGB state
        autocorrect_restore_rgb_state();
    }
}
