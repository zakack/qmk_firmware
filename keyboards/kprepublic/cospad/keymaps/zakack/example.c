const uint16_t DOWN_KEY = KC_F13;
const uint16_t UP_KEY = KC_F14;
static uint16_t sticky_tracker;
static uint16_t double_tracker;

uint16_t toggle(uint16_t keycode) {
  if ((keycode == DOWN_KEY) || (!keycode)) {
    unregister_code(DOWN_KEY);
    keycode = UP_KEY;
  } else {
    unregister_code(UP_KEY);
    keycode = DOWN_KEY;
  }
  register_code(keycode);
  return keycode;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_A ... KC_F12: //skip over F13 & F14
    case KC_F15 ... KC_EXSEL: //exsel is the last one before the modifier keys
      if (record->event.pressed) {
        double_tracker++;
        //wait_ms(DELAY);
        sticky_tracker = toggle(sticky_tracker);
        register_code(keycode);
        return false;
      }
      break;
  }
  return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_A ... KC_F12: //skip over F13 & F14
    case KC_F15 ... KC_EXSEL: //exsel is the last one before the modifier keys
      if (!record->event.pressed) {
        wait_ms(DELAY);
        double_tracker--;
        if (double_tracker < 1)) break;
        sticky_tracker = toggle(sticky_tracker);
  }
}
}
