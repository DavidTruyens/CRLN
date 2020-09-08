#include <Arduino.h>
#include "MIDIUSB.h"
#include "AceButton.h"
#include "jled.h"

#define INPUTPIN1 A1
#define GREENLED 12
#define REDLED 10

using namespace ace_button;

AceButton button1(INPUTPIN1);

void handleEvent(AceButton *, uint8_t, uint8_t);

JLed GreenLed = JLed(GREENLED).Breathe(5000).Forever();
JLed RedLed = JLed(REDLED).Breathe(2000).Forever();

enum class programStates : uint8_t
{
  ready,
  started,
  running,
  stopping,
  stopped
} progState;

void setup()
{
  // put your setup code here, to run once:
  delay(2000);
  Serial.begin(115200);

  pinMode(INPUTPIN1, INPUT_PULLUP);
  ButtonConfig *buttonConfig = button1.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);

  progState = programStates::ready;
  Serial.println("ready");
}

uint8_t PushButton();
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void runStates();
void printState();

void loop()
{
  button1.check();
  runStates();
  GreenLed.Update();
  RedLed.Update();
}

programStates prevState = programStates::ready;

void runStates()
{
  if (prevState != progState)
  {
    prevState = progState;

    switch (progState)
    {
    case programStates::ready:
      GreenLed.Breathe(5000);
      GreenLed.Forever();
      Serial.println("ready");
      break;

    case programStates::started:
      Serial.println("started");
      break;

    case programStates::running:
      Serial.println("running");
      break;

    case programStates::stopping:
      Serial.println("stopping");
      break;

    case programStates::stopped:
      Serial.println("stopped");
      break;

    default:
      break;
    }
  }
}

void bumpState()
{
  printState();

  switch (progState)
  {
  case programStates::started:
    progState = programStates::running;
    GreenLed.Breathe(2000);
    GreenLed.Forever();
    break;

  case programStates::running:
    progState = programStates::stopping;
    GreenLed.Blink(5000, 5000).Repeat(1);
    break;

  case programStates::stopping:
    if (GreenLed.IsRunning() == 0)
    {
      progState = programStates::stopped;
      GreenLed.Breathe(10000);
      Serial.println("stopped");
    }
    break;

  default:
    Serial.println("invalid action");
    break;
  }
}

void printState()
{
  Serial.print("Current state: ");
  switch (progState)
  {
  case programStates::ready:
    Serial.println("ready");
    break;

  case programStates::started:
    Serial.println("started");
    break;

  case programStates::running:
    Serial.println("running");
    break;

  case programStates::stopping:
    Serial.println("stopping");
    break;

  case programStates::stopped:
    Serial.println("stopped");
    break;

  default:
    Serial.println("unknown");
    break;
  }
}

void handleEvent(AceButton * /*button*/, uint8_t eventType, uint8_t /* buttonState*/)
{
  printState();

  switch (eventType)
  {
  case AceButton::kEventPressed:
    noteOn(0, 48, 10); // Channel 0, middle C, normal velocity
    Serial.println("pressed");
    break;

  case AceButton::kEventReleased:
    noteOff(0, 48, 10); // Channel 0, middle C, normal velocity
    Serial.println("released");
    bumpState();
    break;

  case AceButton::kEventDoubleClicked:
    Serial.println("double clicked");
    if (progState == programStates::stopped)
    {
      progState = programStates::ready;
      Serial.println("ready");
    }
    break;

  case AceButton::kEventLongPressed:
    if (progState == programStates::ready)
    {
      progState = programStates::started;
      GreenLed.Breathe(1000);
      GreenLed.Repeat(3);
      Serial.println("started");
    }
    Serial.println("long pressed");
    break;

  default:
    break;
  }

  MidiUSB.flush();
}

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value)
{
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}
