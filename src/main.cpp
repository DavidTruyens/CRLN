#include <Arduino.h>
#include "MIDIUSB.h"
#include "AceButton.h"
#include "jled.h"

#define MIDI_IN A0
#define RED_IN A1
#define GREEN_IN A2
#define BLUE_IN A3
#define YELLOW_IN A4

#define YELLOWPIN 5
#define BLUEPIN 9
#define GREENPIN 10
#define REDPIN 11

enum class ledStates
{
  off,
  on,
  breathe,
  flickr,
  unknown,
} redState,
    blueState, greenState, yellowState;

//button declarations
using namespace ace_button;
AceButton midiBtn(MIDI_IN, HIGH, 0);
AceButton redBtn(RED_IN, HIGH, 1);
AceButton greenBtn(GREEN_IN, HIGH, 2);
AceButton blueBtn(BLUE_IN, HIGH, 3);
AceButton yellowBtn(YELLOW_IN, HIGH, 4);

//buttons event handler
void handleEvent(AceButton *, uint8_t, uint8_t);

//led declarations
JLed greenLed = JLed(GREENPIN).Breathe(5000).Forever();
JLed redLed = JLed(REDPIN).Breathe(3000).Forever();
JLed blueLed = JLed(BLUEPIN).Breathe(1000).Forever();
JLed yellowLed = JLed(YELLOWPIN).Breathe(2000).Forever();

//functions
uint8_t PushButton();
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void runStates();
void printState();

void setup()
{
  // put your setup code here, to run once:
  delay(2000);
  Serial.begin(115200);
  Serial.println("The buzz has started");

  pinMode(MIDI_IN, INPUT_PULLUP);
  pinMode(RED_IN, INPUT_PULLUP);
  pinMode(YELLOW_IN, INPUT_PULLUP);
  pinMode(BLUE_IN, INPUT_PULLUP);
  pinMode(GREEN_IN, INPUT_PULLUP);

  ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);

  redState = ledStates::breathe;
  yellowState = ledStates::breathe;
  greenState = ledStates::breathe;
  blueState = ledStates::breathe;

  Serial.println("ready");
}

void loop()
{
  midiBtn.check();
  redBtn.check();
  greenBtn.check();
  blueBtn.check();
  yellowBtn.check();

  redLed.Update();
  greenLed.Update();
  blueLed.Update();
  yellowLed.Update();
}

void handleEvent(AceButton *aceBtn, uint8_t eventType, uint8_t /* buttonState*/)
{

  uint8_t ledID = aceBtn->getId();
  Serial.println(ledID);
  //Serial.printf("button %u \n", ledID);

  //-----MIDI BUTTON--------
  if (ledID == 0)
  {
    Serial.println("midi button");

    switch (eventType)
    {
    case AceButton::kEventPressed:

      noteOn(0, 48, 10); // Channel 0, middle C, normal velocity
      Serial.println("note on");
      break;

    case AceButton::kEventReleased:
      noteOff(0, 48, 10); // Channel 0, middle C, normal velocity
      Serial.println("released");
      break;

    default:
      Serial.println("wrong midi button event");
      break;
    }

    MidiUSB.flush();
  }

  //-----LED BUTTONS--------
  else
  {
    Serial.println("led button");

    if (eventType == AceButton::kEventPressed)
    {
      Serial.println("Button pressed");
      switch (ledID)
      {
      case 1: //RED LED
        switch (redState)
        {
        case ledStates::breathe:
          redState = ledStates::flickr;
          redLed.Candle().Forever();
          Serial.println("red candle");
          break;

        case ledStates::flickr:
          redState = ledStates::off;
          redLed.Off();
          redLed.FadeOff(1000);
          Serial.println("red off");
          break;

        case ledStates::off:
          redState = ledStates::on;
          redLed.On();
          Serial.println("red on");
          break;

        case ledStates::on:
          redState = ledStates::breathe;
          redLed.Breathe(5000).Forever();
          Serial.println("red breathe");
          break;

        case ledStates::unknown:
        default:
          Serial.println("red unknown");
          break;
        }

        break;

      case 2: //GREEN LED
        switch (greenState)
        {
        case ledStates::breathe:
          greenState = ledStates::flickr;
          greenLed.Candle().Forever();;
          Serial.println("green candle");
          break;

        case ledStates::flickr:
          greenState = ledStates::off;
          greenLed.Off();
          Serial.println("green off");
          break;

        case ledStates::off:
          greenState = ledStates::on;
          greenLed.On();
          Serial.println("green on");
          break;

        case ledStates::on:
          greenState = ledStates::breathe;
          greenLed.Breathe(5000).Forever();
          Serial.println("green breathe");
          break;

        case ledStates::unknown:
        default:
          Serial.println("green unknown");
          break;
        }

        break;

      case 3: //BLUE LED
        switch (blueState)
        {
        case ledStates::breathe:
          blueState = ledStates::flickr;
          blueLed.Candle().Forever();;
          Serial.println("blue candle");
          break;

        case ledStates::flickr:
          blueState = ledStates::off;
          blueLed.Off();
          Serial.println("blue off");
          break;

        case ledStates::off:
          blueState = ledStates::on;
          blueLed.On();
          Serial.println("blue on");
          break;

        case ledStates::on:
          blueState = ledStates::breathe;
          blueLed.Breathe(5000).Forever();
          Serial.println("blue breathe");
          break;

        case ledStates::unknown:
        default:
          Serial.println("blue unknown");
          break;
        }
        break;

      case 4: //YELLOW LED
        switch (yellowState)
        {
        case ledStates::breathe:
          yellowState = ledStates::flickr;
          yellowLed.Candle().Forever();
          Serial.println("yellow candle");
          break;

        case ledStates::flickr:
          yellowState = ledStates::off;
          yellowLed.Off();
          Serial.println("yellow off");
          break;

        case ledStates::off:
          yellowState = ledStates::on;
          yellowLed.On();
          Serial.println("yellow on");
          break;

        case ledStates::on:
          yellowState = ledStates::breathe;
          yellowLed.Breathe(5000).Forever();
          Serial.println("yellow breathe");
          break;

        case ledStates::unknown:
        default:
          Serial.println("yellow unknown");
          break;
        }
        break;

      default:
        Serial.println("wrong led...");
        break;
      }
    }
    else if (eventType == AceButton::kEventLongPressed)
    {
      Serial.println("long press");
      redState = ledStates::off;
      greenState = ledStates::off;
      blueState = ledStates::off;
      yellowState = ledStates::off;

      redLed.Off();
      greenLed.Off();
      blueLed.Off();
      yellowLed.Off();
    }
  }
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
