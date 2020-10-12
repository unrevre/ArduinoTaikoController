#include "nonblock_read.h"

#include <Arduino.h>

#define ENABLE_KEYBOARD
/* #define ENABLE_NSSWITCH */

#define ENABLE_DEBUG

#ifdef ENABLE_KEYBOARD
#include <Keyboard.h>

const char kb_key_map[4] = { 'd', 'f', 'j', 'k', };
#endif /* ENABLE_KEYBOARD */

#ifdef ENABLE_NSSWITCH
#include "Joystick.h"

const int ns_sns_map[4] = {
  SWITCH_BTN_ZL,
  SWITCH_BTN_LCLICK,
  SWITCH_BTN_RCLICK,
  SWITCH_BTN_ZR,
};
#endif /* ENABLE_NSSWITCH */

const int   k_thres = 15;
const long  k_relax = 10000;
const float k_decay = 0.97f;
const float k_mult  = 1.5f;

const int   mkd_pin[4] = {  A0,  A1,  A2,  A3, };
const int   mkd_nxt[4] = {   3,   2,   0,   1, };
const float mkd_amp[4] = { 1.f, 1.f, 1.f, 1.f, };

bool  mkd_dwn[4] = {   0,   0,   0,   0, };
int   mkd_raw[4] = {   0,   0,   0,   0, };
float mkd_sig[4] = { 0.f, 0.f, 0.f, 0.f, };

long  mkd_busy = 0;
float mkd_enve = 20.f;

#ifdef ENABLE_NSSWITCH
int ns_state = 0;
#endif /* ENABLE_NSSWITCH */

unsigned long t0;

#ifdef ENABLE_NSSWITCH
unsigned long tblock;
#endif /* ENABLE_NSSWITCH */

void setup() {
  analogReference(DEFAULT);
  analogPrepareRead(mkd_pin[0]);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

#ifdef ENABLE_KEYBOARD
  Keyboard.begin();
#endif /* ENABLE_KEYBOARD */

#ifdef ENABLE_NSSWITCH
  tblock = 0;
#endif /* ENABLE_NSSWITCH */

#ifdef ENABLE_DEBUG
  Serial.begin(9600);
#endif /* ENABLE_DEBUG */

  t0 = micros();
}

void loop() {
  static int idx = 0;

  unsigned long t1 = micros();
  unsigned long dt = t1 - t0;

  t0 = t1;

  mkd_busy = max((long)(mkd_busy - dt), 0);
  mkd_enve = mkd_enve * k_decay;

  int raw_prev = mkd_raw[idx];
  float sig_prev = mkd_sig[idx];

  int raw_data = analogReadData();
  float sig_data = abs(raw_data - raw_prev) * mkd_amp[idx];

  mkd_raw[idx] = raw_data;
  mkd_sig[idx] = (sig_data + sig_prev * 2) / 3;

  analogPrepareRead(mkd_pin[mkd_nxt[idx]]);

  for (int i = 0; i != 4; ++i) {
    if (mkd_busy || !mkd_dwn[i])
      continue;

    mkd_dwn[i] = false;

#ifdef ENABLE_KEYBOARD
    Keyboard.release(kb_key_map[i]);
#endif /* ENABLE_KEYBOARD */
  }

  int max_idx = -1;
  float max_sig = mkd_enve;

  for (int i = 0; i != 4; ++i) {
    if (mkd_sig[i] < max_sig)
      continue;

    max_idx = i;
    max_sig = mkd_sig[i];
  }

  if (max_idx == idx && max_sig >= k_thres && !mkd_busy) {
    if (!mkd_dwn[idx]) {
      mkd_dwn[idx] = true;

#ifdef ENABLE_KEYBOARD
      Keyboard.press(kb_key_map[idx]);
#endif /* ENABLE_KEYBOARD */

#ifdef ENABLE_NSSWITCH
      ns_state |= ns_sns_map[idx];
#endif /* ENABLE_NSSWITCH */
    }

    mkd_busy = k_relax;
  }

  if (max_idx != -1 && mkd_busy)
    mkd_enve = max(mkd_enve, max_sig * k_mult);

#ifdef ENABLE_NSSWITCH
  tblock += dt;

  if (tblock > 32000 || (tblock > 8000 && ns_state)) {
    Joystick.report(ns_state);

    ns_state = 0;
    tblock = 0;
  }
#endif /* ENABLE_NSSWITCH */

#ifdef ENABLE_DEBUG
  static bool output = false;

  if (mkd_enve < 5)
    output = false;

  if (mkd_enve > 10)
    output = true;

  if (output) {
    for (int i = 0; i < 4; ++i) {
      Serial.print(mkd_sig[i], 1);
      Serial.print('\t');
    }

    Serial.print("| ");

    for (int i = 0; i < 4; ++i) {
      Serial.print(mkd_busy ? (mkd_dwn[i] ? '#' : '*') : ' ');
      Serial.print(' ');
    }

    Serial.print("| ");

    Serial.print(mkd_enve, 1);
    Serial.println();
  }
#endif /* ENABLE_DEBUG */

  unsigned long tlapse = micros() - t0;

  if (tlapse < 297)
    delayMicroseconds(300 - tlapse);

  idx = mkd_nxt[idx];
}
