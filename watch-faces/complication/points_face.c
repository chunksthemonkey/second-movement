/*
 * MIT License
 *
 * Copyright (c) 2022-2025 Andrew Mike and David Volovskiy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include "points_face.h"
#include "watch.h"

#define POINTS_FACE_MAX 99
#define POINTS_FACE_MIN -9

const char _page_name[POINTS_PAGES][5] = { "OSMO", "FEMO" };

const char _page_name_fallback[POINTS_PAGES][3] = { "OS", "FE" };

void points_face_setup(uint8_t watch_face_index, void ** context_ptr) {
    (void) watch_face_index;
    if (*context_ptr == NULL) {
        *context_ptr = malloc(sizeof(points_state_t));
        memset(*context_ptr, 0, sizeof(points_state_t));
        points_state_t *state = (points_state_t *)*context_ptr;
        state->page_idx = 0;
        for (int n = 0; n < POINTS_PAGES; n++) {
            state->points_idx[n] = 0;
        }
    }
}

void points_face_activate(void *context) {
    (void) context;
}

static void points_face_increment(points_state_t *state, bool sound_on) {
        if (state->points_idx[state->page_idx] >= POINTS_FACE_MAX){
            if (sound_on) watch_buzzer_play_note(BUZZER_NOTE_E7, 30);
        }
        else {
            state->points_idx[state->page_idx]++;
            print_points(state, sound_on);
            if (sound_on) watch_buzzer_play_note(BUZZER_NOTE_E6, 30);
        }
}

static void points_face_decrement(points_state_t *state, bool sound_on) {
        if (state->points_idx[state->page_idx] <= POINTS_FACE_MIN){
            if (sound_on) watch_buzzer_play_note(BUZZER_NOTE_C5SHARP_D5FLAT, 30);
        }
        else {
            state->points_idx[state->page_idx]--;
            print_points(state, sound_on);
            if (sound_on) watch_buzzer_play_note(BUZZER_NOTE_C6SHARP_D6FLAT, 30);
        }
}

static void points_reset(points_state_t *state, bool sound_on) {
    state->points_idx[state->page_idx] = 0;
    if (sound_on) watch_buzzer_play_note(BUZZER_NOTE_C5SHARP_D5FLAT, 30);
    print_points(state, sound_on);
}

static void points_face_next_page(points_state_t *state, bool sound_on) {
    state->page_idx = (state->page_idx + 1) % POINTS_PAGES;
    if (sound_on) watch_buzzer_play_note(BUZZER_NOTE_C5SHARP_D5FLAT, 30);
    print_points(state, sound_on);
}

bool points_face_loop(movement_event_t event, void *context) {
    points_state_t *state = (points_state_t *)context;
    static bool using_led = false;

    if (using_led) {
        if(!HAL_GPIO_BTN_MODE_read() && !HAL_GPIO_BTN_LIGHT_read() && !HAL_GPIO_BTN_ALARM_read())
            using_led = false;
        else {
            if (event.event_type == EVENT_LIGHT_BUTTON_DOWN || event.event_type == EVENT_ALARM_BUTTON_DOWN)
                movement_illuminate_led();
            return true;
        }
    }

    switch (event.event_type) {
        case EVENT_TICK:
            break;
        case EVENT_ALARM_BUTTON_UP:
            points_face_increment(state, movement_button_should_sound());
            break;
        case EVENT_ALARM_LONG_PRESS:
            points_face_next_page(state, movement_button_should_sound());
            break;
        case EVENT_MODE_LONG_PRESS:
            movement_move_to_face(0);
            break;
        case EVENT_LIGHT_BUTTON_UP:
            points_face_decrement(state, movement_button_should_sound());
            break;
        case EVENT_LIGHT_BUTTON_DOWN:
        case EVENT_ALARM_BUTTON_DOWN:
            if (HAL_GPIO_BTN_MODE_read()) {
              movement_illuminate_led();
              using_led = true;
            }
            break;
        case EVENT_LIGHT_LONG_PRESS:
            points_reset(state, movement_button_should_sound());
            break;
        case EVENT_ACTIVATE:
            print_points(state, movement_button_should_sound());
            break;
        case EVENT_TIMEOUT:
            // ignore timeout
            break;
        default:
            movement_default_loop_handler(event);
            break;
    }

    return true;
}

// print points index at the center of display.
void print_points(points_state_t *state, bool sound_on) {
    char buf[7];
    int display_val = (int)(state->points_idx[state->page_idx]);
    uint16_t hours = (abs(display_val) * 5) / 60;
    uint16_t minutes = (abs(display_val) * 5) % 60;
    const char *page_name = _page_name[(int)(state->page_idx)];
    const char *page_name_fallback = _page_name_fallback[(int)(state->page_idx)];
    
    // Clamp to limits
    if (display_val > POINTS_FACE_MAX) display_val = POINTS_FACE_MAX;
    if (display_val < POINTS_FACE_MIN) display_val = POINTS_FACE_MIN;
    
    if (sound_on)
        watch_set_indicator(WATCH_INDICATOR_BELL);
    else
        watch_clear_indicator(WATCH_INDICATOR_BELL);
    watch_display_text_with_fallback(WATCH_POSITION_TOP, page_name, page_name_fallback);
    if (hours > 0)
      sprintf(buf, "%2d=%1u%02u", display_val, hours, minutes);
    else
      sprintf(buf, "%2d =%02u", display_val, minutes);
    watch_display_text(WATCH_POSITION_BOTTOM, buf);
}

void points_face_resign(void *context) {
    (void) context;
}
