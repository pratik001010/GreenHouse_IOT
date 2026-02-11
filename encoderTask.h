// ============================================================
// MCU - HELTEC LoRa V3 - GPIO PIN  (Pratik: Encoder Task)
// 4 stages: FAN_OFF, FAN_ON, HEATER_OFF, HEATER_ON
// Serial Monitor shows the stage while rotating the encoder.
// ============================================================

#include <Arduino.h>
#include <RotaryEncoder.h>

// ----------------- 32 Header right [RST] Button -- Top ----
#define GPIO_ENC_DT     19   // Encoder Data 
#define GPIO_ENC_CLK    20   // Encoder Clock 
#define GPIO_BOARD_BP    0   // Encoder switch / button 

// ----------------- Forward declarations (avoid Arduino proto issues)
enum Stage : int;
const char* stageText(Stage);

// ----------------- Encoder object
RotaryEncoder encoder(GPIO_ENC_CLK, GPIO_ENC_DT, RotaryEncoder::LatchMode::FOUR0);

volatile bool encFlag = false;
void IRAM_ATTR isrEncoder() { encFlag = true; }

// ----------------- 4 stages
enum Stage : int {
  FAN_OFF = 0,
  FAN_ON,
  HEATER_OFF,
  HEATER_ON,
  STAGE_COUNT
};

const char* stageText(Stage s) {
  switch (s) {
    case FAN_OFF:     return "FAN OFF";
    case FAN_ON:      return "FAN ON";
    case HEATER_OFF:  return "HEATER OFF";
    case HEATER_ON:   return "HEATER ON";
    default:          return "?";
  }
}

Stage currentStage = FAN_OFF;

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(GPIO_ENC_CLK, INPUT_PULLUP);
  pinMode(GPIO_ENC_DT,  INPUT_PULLUP);
  pinMode(GPIO_BOARD_BP, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(GPIO_ENC_CLK), isrEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(GPIO_ENC_DT),  isrEncoder, CHANGE);

  encoder.setPosition(0);

  Serial.println("Student 1 - Encoder Task (Heltec LoRa V3)");
  Serial.println("Rotate encoder to change stage; press button to SELECT.");
  Serial.println(stageText(currentStage));
}

void loop() {
  if (encFlag) {
    encFlag = false;

    encoder.tick();

    long pos = encoder.getPosition();
    int idx = (int)(pos % STAGE_COUNT);
    if (idx < 0) idx += STAGE_COUNT;

    Stage newStage = (Stage)idx;
    if (newStage != currentStage) {
      currentStage = newStage;
      Serial.println(stageText(currentStage));
    }
  }

  // Optional: button press prints current selection
  if (digitalRead(GPIO_BOARD_BP) == LOW) {
    delay(30); // debounce
    if (digitalRead(GPIO_BOARD_BP) == LOW) {
      Serial.print("SELECT: ");
      Serial.println(stageText(currentStage));
      while (digitalRead(GPIO_BOARD_BP) == LOW) delay(5);
    }
  }

  delay(1);
}

