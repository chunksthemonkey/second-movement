/*
 * MIT License
 *
 * Copyright (c) 2022 Andrew Mike
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

#ifndef POINTS_FACE_H_
#define POINTS_FACE_H_

/*
 * POINTS face
 *
 * Points face is designed to act as a points counter.
 *
 * Alarm
 *    Press: Decrement
 *    Hold : Fast Decrement
 *
 * Light
 *    Press: Increment
 *    Hold : On initial value: Cycles through other initial values.
 *                       Else: Fast Increment
 *
 * Mode
 *    Press: Next face
 *    Hold : On initial value: Go to first face.
 *                       Else: Resets counter
 * 
 * Incrementing or Decrementing the points will beep if Beeping is set in the global Preferences
 */

#include "movement.h"

#define POINTS_PAGES 2

typedef struct {
    int8_t page_idx;
    int16_t points_idx[POINTS_PAGES];
} points_state_t;


void points_face_setup(uint8_t watch_face_index, void ** context_ptr);
void points_face_activate(void *context);
bool points_face_loop(movement_event_t event, void *context);
void points_face_resign(void *context);

void print_points(points_state_t *state, bool sound_on);

#define points_face ((const watch_face_t){ \
    points_face_setup, \
    points_face_activate, \
    points_face_loop, \
    points_face_resign, \
    NULL, \
})

#endif // POINTS_FACE_H_
