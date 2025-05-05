#include QMK_KEYBOARD_H
#define ONESHOT_TAP_TOGGLE 3
#define ONESHOT_TIMEOUT 2000

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BL,
    _FL
};
const uint8_t DELAY = 1;
const uint8_t HOLD_THRESHOLD = 38;
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
        KC_BSPC, KC_T,   MO(_FL), KC_LSFT,
        KC_4,    KC_R,   KC_F, KC_RALT,
        KC_3,    KC_E,   KC_D, KC_C,
        KC_2,    KC_W,   KC_S, KC_X,
        KC_1,    KC_Q,   KC_A, KC_Z,
        KC_ESC,  KC_N,   KC_LGUI, KC_GRAVE
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
        QK_BOOT, UG_TOGG, _______, QK_LOCK,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______,
        _______, _______, _______, _______
    )
};

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
    wait_ms(DELAY);
    if (broccoli_down != broccoli_state) {
        if (broccoli_down) unregister_code(BROCCOLI);
        else if (broccoli_state) register_code(BROCCOLI);
        broccoli_down = broccoli_state;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_A ... KC_F12:
        case KC_F15 ... KC_EXSEL:
            if (record->event.pressed) {
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
                if ((keycode != KC_S) && (keycode != KC_D)) register_code(keycode);
                return false;
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
