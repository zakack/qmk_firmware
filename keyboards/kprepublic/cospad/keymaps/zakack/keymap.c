#include QMK_KEYBOARD_H
#define ONESHOT_TAP_TOGGLE 3
#define ONESHOT_TIMEOUT 2000

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
const uint8_t DELAY = 0;
// Was 38ms, going to try lowering it a bit
// 32ms seemed better. Going for 22ms...
// 22ms was a bit too fast. Settling on 24ms
const uint8_t HOLD_THRESHOLD = 24;
const uint16_t GRAPE = KC_F13;      // 🍇 virtual button
const uint16_t BROCCOLI = KC_F14;   // 🥦 virtual button
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Keymap _BL: (Base Layer) Default Layer
     * ,-------------------.
     * |Esc |TAB | FN | BS |
     * |----|----|----|----|
     * | NL | /  | *  | -  |
     * |----|----|----|----|
     * | 7  | 8  | 9  |    |
     * |----|----|----| +  |
     * | 4  | 5  | 6  |    |
     * |----|----|----|----|
     * | 1  | 2  | 3  |    |
     * |----|----|----| En |
     * |   0     | .  |    |
     * `-------------------'
     */
    [_BL] = LAYOUT_ortho_6x4(
        KC_NO,    CRD1,  CRD2,  CRD3,
        KC_ESC,   KC_F18,  KC_F19,  MO(_FL),
        KC_Q,    KC_W,   KC_E,   KC_R,
        KC_A,   KC_S,   KC_D, KC_F,
        KC_Z,    KC_X,   KC_C,   KC_RALT,
        KC_F,  KC_T,  KC_V, KC_LCTL
    ),
    /* Keymap _FL: Function Layer
     * ,-------------------.
     * |RGBT|    |    |    |
     * |----|----|----|----|
     * |RGBM|RGBP|BTOG|    |
     * |----|----|----|----|
     * |HUD |HUI |BON |    |
     * |----|----|----|    |
     * |SAD |SAI |BOFF|    |
     * |----|----|----|----|
     * |VAD |VAS |BSTP|    |
     * `-------------------'
    */
    [_FL] = LAYOUT_ortho_6x4(
        _______, UG_TOGG, _______, QK_LOCK,
        QK_BOOT, _______, _______, _______,
        RGB_HUI, RGB_SAI, RGB_VAD, _______,
        RGB_RMOD,RGB_TOG, RGB_MOD, _______,
        RGB_HUD, RGB_SAD, RGB_VAI, _______,
        _______, _______, _______, _______
    )
};

//uint16_t hold_timer = 0;
//layer_state_t prev_color = 0;
//bool threshold = false;
static uint16_t last_key1 = 0;      // First tracked physical key
static uint16_t last_key2 = 0;      // Second tracked physical key
static bool key1_pressed = false;    // State of first tracked key
static bool key2_pressed = false;    // State of second tracked key
static uint32_t key1_press_time = 0; // Timestamp of first key press
static uint32_t key2_press_time = 0; // Timestamp of second key press
static bool grape_down = false;      // 🍇 state
static bool broccoli_down = false;   // 🥦 state

void set_virtual_button_state(bool grape_state, bool broccoli_state) {
    if (grape_down != grape_state) {
        if (grape_down) unregister_code(GRAPE);
        else if (grape_state) register_code(GRAPE);
        grape_down = grape_state;
    }
    //wait_ms(DELAY);
    if (broccoli_down != broccoli_state) {
        if (broccoli_down) unregister_code(BROCCOLI);
        else if (broccoli_state) register_code(BROCCOLI);
        broccoli_down = broccoli_state;
    }
}

// layer_state_t layer_state_set_user(layer_state_t state) {
//     uint8_t color = state >> 1; // We don't care about default layer
//     if (!threshold && (color == 3)) { // Both buttons pressed, start timer
//         hold_timer = timer_read();
//     }
//     if (!prev_color) {
//         register_code(M1);
//     } else if (color > prev_color) {
//         unregister_code(M1);
//         register_code(M2);
//     } else if (threshold && prev_color - color)
//     if (((color ^ prev_color) >> 1) && (prev_color >> 1)) {
//
//     }
//     if (!layer_state_cmp(prev_color, _BLUE) && layer_state_cmp(state, _BLUE)) {
//
//     }
//
//
// }

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CRD1:
            if (record->event.pressed) {
                SEND_STRING("ie\%i!Z&4C*L=+4p"SS_TAP(X_ENTER));
            }
            break;
        case CRD2:
            if (record->event.pressed) {
                SEND_STRING("ApexPredator4thewin"SS_TAP(X_ENTER));
            }
            break;
        case CRD3:
            if (record->event.pressed) {
                SEND_STRING("pillowrocklizard"SS_TAP(X_ENTER));
            }
            break;
        // case OSU:
        //     if (record->event.pressed) {
        //         layer_on(_RED);
        //     } else {
        //         layer_off(_RED);
        //     }
        //     break;
        case KC_A ... KC_F12:
        case KC_F15 ... KC_EXSEL:
            if (record->event.pressed) {
                //layer_on(_BLUE);
                // If this is a new key, update our tracked keys
                if (keycode != last_key1 && keycode != last_key2) {
                    last_key1 = last_key2;
                    last_key2 = keycode;
                    key1_pressed = key2_pressed;
                    key2_pressed = true;
                    key1_press_time = key2_press_time;
                    key2_press_time = record->event.time;
                }
                // If this is one of our tracked keys, update its state and timestamp
                else if (keycode == last_key1) {
                    key1_pressed = true;
                    key1_press_time = record->event.time;
                }
                else if (keycode == last_key2) {
                    key2_pressed = true;
                    key2_press_time = record->event.time;
                }

                // Check timing for two-key behavior
                if (key1_pressed && key2_pressed) {
                    uint32_t first_press_time = (last_key1 == keycode) ? key2_press_time : key1_press_time;
                    uint32_t time_diff = record->event.time - first_press_time;

                    if (time_diff >= HOLD_THRESHOLD) {
                        // Enough time has passed, toggle virtual buttons
                        set_virtual_button_state(!grape_down, !broccoli_down);
                    } else {
                        // Not enough time has passed, treat as individual key
                        if (keycode == last_key1) {
                            set_virtual_button_state(true, false);
                        } else {
                            set_virtual_button_state(false, true);
                        }
                    }
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

// void matrix_scan_user(void) {
//     if (!threshold) {
//         if (timer_elapsed(hold_timer) >= HOLD_THRESHOLD) {
//             threshold = true;
//         }
//     }
// }

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
                    // One key is still down, check timing for toggle
                    uint32_t held_time = record->event.time -
                        (key1_pressed ? key1_press_time : key2_press_time);

                    if (held_time >= HOLD_THRESHOLD) {
                        set_virtual_button_state(!grape_down, !broccoli_down);
                    }
                } else {
                    // No keys are down, release both virtual buttons
                    set_virtual_button_state(false, false);
                }
            }
            break;
    }
}
