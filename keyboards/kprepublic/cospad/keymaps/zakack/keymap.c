#include QMK_KEYBOARD_H
#include <avr/io.h>

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BL
    // _FL
};

const uint16_t GRAPE = KC_F13;      // \U0001f347 virtual button
const uint16_t BROCCOLI = KC_F14;   // \U0001f966 virtual button

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BL] = LAYOUT_ortho_6x4(
        KC_F1,   KC_TAB,  KC_SPACE,  KC_F4,
        KC_ESC,   KC_ASTERISK,   KC_SLASH,   KC_MINUS,
        KC_7,     KC_8,    KC_9,    KC_P,
        KC_4,     KC_5,    KC_6,    KC_SEMICOLON,
        KC_1,     KC_2,    KC_3,    KC_V,
        KC_LEFT,  KC_DOWN, KC_UP,   KC_RIGHT
    )
    // [_FL] = LAYOUT_ortho_6x4(
    //     _______,  UG_TOGG, _______, QK_LOCK,
    //     QK_BOOT,  _______, _______, QK_BOOT,
    //     RGB_HUI,  RGB_SAI, RGB_VAD, _______,
    //     RGB_RMOD, RGB_TOG, RGB_MOD, _______,
    //     RGB_HUD,  RGB_SAD, RGB_VAI, _______,
    //     _______,  _______, _______, _______
    // )
};

static uint16_t last_key1 = 0;      // First tracked physical key
static uint16_t last_key2 = 0;      // Second tracked physical key
static bool key1_pressed = false;    // State of first tracked key
static bool key2_pressed = false;    // State of second tracked key
static bool grape_down = false;      // 🍇 state
static bool broccoli_down = false;   // 🥦 state

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_A ... KC_F12:
        case KC_F15 ... KC_EXSEL:
            if (record->event.pressed) {
                if (keycode == last_key1) {
                    key1_pressed = true;
                } else if (keycode == last_key2) {
                    key2_pressed = true;
                } else {
                    // new key, update states
                    last_key1 = last_key2;
                    last_key2 = keycode;
                    key1_pressed = key2_pressed;
                    key2_pressed = true;
                }

                if (key1_pressed && key2_pressed) {
                    // toggle
                    bool new_grape = !grape_down;
                    bool new_broccoli = !broccoli_down;
                    if (grape_down != new_grape) {
                        if (grape_down) unregister_code(GRAPE);
                        else if (new_grape) register_code(GRAPE);
                        grape_down = new_grape;
                    }
                    if (broccoli_down != new_broccoli) {
                        if (broccoli_down) unregister_code(BROCCOLI);
                        else if (new_broccoli) register_code(BROCCOLI);
                        broccoli_down = new_broccoli;
                    }
                } else {
                    // single key
                    bool target_grape = (keycode == last_key1);
                    bool new_grape = target_grape;
                    bool new_broccoli = !target_grape;
                    if (grape_down != new_grape) {
                        if (grape_down) unregister_code(GRAPE);
                        else if (new_grape) register_code(GRAPE);
                        grape_down = new_grape;
                    }
                    if (broccoli_down != new_broccoli) {
                        if (broccoli_down) unregister_code(BROCCOLI);
                        else if (new_broccoli) register_code(BROCCOLI);
                        broccoli_down = new_broccoli;
                    }
                }
            }
            return true;
        default:
            return true;
    }
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {  // key release
        if (keycode == last_key1) {
            key1_pressed = false;
        } else if (keycode == last_key2) {
            key2_pressed = false;
        }
        if (key1_pressed || key2_pressed) {
            // toggle
            bool new_grape = !grape_down;
            bool new_broccoli = !broccoli_down;
            if (grape_down != new_grape) {
                if (grape_down) unregister_code(GRAPE);
                else if (new_grape) register_code(GRAPE);
                grape_down = new_grape;
            }
            if (broccoli_down != new_broccoli) {
                if (broccoli_down) unregister_code(BROCCOLI);
                else if (new_broccoli) register_code(BROCCOLI);
                broccoli_down = new_broccoli;
            }
        } else {
            // both released, reset
            if (grape_down) unregister_code(GRAPE);
            if (broccoli_down) unregister_code(BROCCOLI);
            grape_down = false;
            broccoli_down = false;
        }
    }
}
