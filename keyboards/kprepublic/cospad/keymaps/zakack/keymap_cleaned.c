#include QMK_KEYBOARD_H

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BL,
    _FL
};
enum custom_keycodes {
    CRD1 = SAFE_RANGE,
    CRD2,
    CRD3
};
// Was 38ms, going to try lowering it a bit
// 32ms seemed better. Going for 22ms...
// 22ms was a bit too fast. Settling on 24ms
const uint16_t GRAPE = KC_F13;      // 🍇 virtual button
const uint16_t BROCCOLI = KC_F14;   // 🥦 virtual button
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BL] = LAYOUT_ortho_6x4(
        KC_NO,    CRD1,  CRD2,  CRD3,
        KC_ESC,   KC_F13,  KC_F14,  MO(_FL),
        KC_Q,    KC_W,   KC_E,   KC_R,
        KC_A,   KC_S,   KC_D, KC_F,
        KC_ESC,    KC_X,   KC_C,   KC_H,
        KC_F,  KC_T,  KC_V, KC_LCTL
    ),
    [_FL] = LAYOUT_ortho_6x4(
        _______, UG_TOGG, _______, QK_LOCK,
        QK_BOOT, _______, _______, _______,
        RGB_HUI, RGB_SAI, RGB_VAD, _______,
        RGB_RMOD,RGB_TOG, RGB_MOD, _______,
        RGB_HUD, RGB_SAD, RGB_VAI, _______,
        _______, _______, _______, _______
    )
};

static uint16_t last_key1 = 0;      // First tracked physical key
static uint16_t last_key2 = 0;      // Second tracked physical key
static bool key1_pressed = false;    // State of first tracked key
static bool key2_pressed = false;    // State of second tracked key
static bool grape_down = false;      // 🍇 state
static bool broccoli_down = false;   // 🥦 state

void set_virtual_button_state(bool grape_state, bool broccoli_state) {
    if (grape_down != grape_state) {
        if (grape_down) unregister_code(GRAPE);
        else if (grape_state) register_code(GRAPE);
        grape_down = grape_state;
    }
    if (broccoli_down != broccoli_state) {
        if (broccoli_down) unregister_code(BROCCOLI);
        else if (broccoli_state) register_code(BROCCOLI);
        broccoli_down = broccoli_state;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CRD1:
            if (record->event.pressed) {
                SEND_STRING("glhf"SS_TAP(X_ENTER));
            }
            break;
        case CRD2:
            if (record->event.pressed) {
                SEND_STRING("ggs"SS_TAP(X_ENTER));
            }
            break;
        case CRD3:
            if (record->event.pressed) {
                SEND_STRING("R0GU3nicpatches!"SS_TAP(X_ENTER));
            }
            break;
        case KC_A ... KC_F12:
        case KC_F15 ... KC_EXSEL:
            if (record->event.pressed) {
                // If this is a new key, update our tracked keys
                if (keycode != last_key1 && keycode != last_key2) {
                    last_key1 = last_key2;
                    last_key2 = keycode;
                    key1_pressed = key2_pressed;
                    key2_pressed = true;
                }
                else if (keycode == last_key1) {
                    key1_pressed = true;
                }
                else if (keycode == last_key2) {
                    key2_pressed = true;
                }
                if (key1_pressed && key2_pressed) {
                    set_virtual_button_state(!grape_down, !broccoli_down);
                } else {
                    // Single key behavior
                    if (keycode == last_key1) {
                        set_virtual_button_state(true, false);
                    } else {
                        set_virtual_button_state(false, true);
                    }
                }
            }
            break;
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_A ... KC_F12:
        case KC_F15 ... KC_EXSEL:
            if (!record->event.pressed) {
                // Update key states
                if (keycode == last_key1) {
                    key1_pressed = false;
                }
                else if (keycode == last_key2) {
                    key2_pressed = false;
                }

                if (key1_pressed || key2_pressed) {
                    set_virtual_button_state(!grape_down, !broccoli_down);
                } else {
                    // No keys are down, release both virtual buttons
                    set_virtual_button_state(false, false);
                }
            }
            break;
    }
}
