#include QMK_KEYBOARD_H

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BL
    // _FL
};

const uint16_t GRAPE = KC_F13;      // \U0001f347 virtual button
const uint16_t BROCCOLI = KC_F14;   // \U0001f966 virtual button

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BL] = LAYOUT_ortho_6x4(
        KC_SPACE,    KC_F10,  KC_F11,  KC_F12,
        KC_ESC,   KC_F8,   KC_F9,   KC_R,
        KC_Q,     KC_W,    KC_E,    QK_BOOT,
        KC_A,     KC_S,    KC_D,    KC_F,
        KC_Z,     KC_X,    KC_C,    KC_H,
        KC_F,     KC_T,    KC_V,    KC_ENTER
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
static bool grape_down = false;      // \U0001f347 state
static bool broccoli_down = false;   // \U0001f966 state

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_A ... KC_F12:
        case KC_F15 ... KC_EXSEL:
            if (record->event.pressed) {
                // Streamlined: Check matches first, then handle new key
                if (keycode == last_key1) {
                    key1_pressed = true;
                } else if (keycode == last_key2) {
                    key2_pressed = true;
                } else {
                    // New key: Shift and update states
                    last_key1 = last_key2;
                    last_key2 = keycode;
                    key1_pressed = key2_pressed;
                    key2_pressed = true;
                }

                if (key1_pressed && key2_pressed) {
                    // Inline toggle logic for dual-key state
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
                    // Single key behavior (completed based on context)
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
            return true;  // Allow further processing if needed
        default:
            return true;
    }
}

// Assuming this exists for release handling (not fully in context, but implied)
void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {  // Handle releases
        if (keycode == last_key1) {
            key1_pressed = false;
        } else if (keycode == last_key2) {
            key2_pressed = false;
        }
        if (key1_pressed || key2_pressed) {
            // Inline toggle for remaining key
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
            // Both released: Reset to off
            if (grape_down) unregister_code(GRAPE);
            if (broccoli_down) unregister_code(BROCCOLI);
            grape_down = false;
            broccoli_down = false;
        }
    }
}
