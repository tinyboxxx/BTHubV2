#include <bluefruit.h>
#include "RotaryEncoder.h"

BLEDis bledis;
BLEBas blebas;  // BAS (Battery Service) helper class instance
BLEHidAdafruit blehid;
HwRotaryEncoder ENC1;
SwRotaryEncoder ENC2;

#define SLEEPING_DELAY 15 * 60 * 1000  // sleep after 15 min (to ms)
// #define SLEEPING_DELAY 50 * 1000 // sleep after 50sec (to ms) (for debug)
// 定义各个引脚
#define PIN0_02 NRF_GPIO_PIN_MAP(0, 2)   // 定义P0.02
#define PIN0_06 NRF_GPIO_PIN_MAP(0, 6)   // 定义P0.06
#define PIN0_07 NRF_GPIO_PIN_MAP(0, 7)   // 定义P0.07
#define PIN0_08 NRF_GPIO_PIN_MAP(0, 8)   // 定义P0.08
#define PIN0_11 NRF_GPIO_PIN_MAP(0, 11)  // 定义P0.11
#define PIN0_12 NRF_GPIO_PIN_MAP(0, 12)  // 定义P0.12
#define PIN0_17 NRF_GPIO_PIN_MAP(0, 17)  // 定义P0.17
#define PIN0_19 NRF_GPIO_PIN_MAP(0, 19)  // 定义P0.19
#define PIN0_20 NRF_GPIO_PIN_MAP(0, 20)  // 定义P0.20
#define PIN0_21 NRF_GPIO_PIN_MAP(0, 21)  // 定义P0.21
#define PIN0_22 NRF_GPIO_PIN_MAP(0, 22)  // 定义P0.22
#define PIN0_23 NRF_GPIO_PIN_MAP(0, 23)  // 定义P0.23
#define PIN0_24 NRF_GPIO_PIN_MAP(0, 24)  // 定义P0.24
#define PIN0_29 NRF_GPIO_PIN_MAP(0, 29)  // 定义P0.29
#define PIN0_31 NRF_GPIO_PIN_MAP(0, 31)  // 定义P0.31
#define PIN1_00 NRF_GPIO_PIN_MAP(1, 0)   // 定义P1.00
#define PIN1_02 NRF_GPIO_PIN_MAP(1, 2)   // 定义P1.02
#define PIN1_04 NRF_GPIO_PIN_MAP(1, 4)   // 定义P1.04
#define PIN1_06 NRF_GPIO_PIN_MAP(1, 6)   // 定义P1.06
#define PIN1_08 NRF_GPIO_PIN_MAP(1, 8)   // 定义P1.08
#define PIN1_09 NRF_GPIO_PIN_MAP(1, 9)   // 定义P1.09
#define PIN1_10 NRF_GPIO_PIN_MAP(1, 10)  // 定义P1.10
#define PIN1_11 NRF_GPIO_PIN_MAP(1, 11)  // 定义P1.11
#define PIN1_13 NRF_GPIO_PIN_MAP(1, 13)  // 定义P1.13
#define PIN1_15 NRF_GPIO_PIN_MAP(1, 15)  // 定义P1.15
#define PIN_LED NRF_GPIO_PIN_MAP(0, 15)  // 定义P0.15 LED 蓝灯是0.15控制的，高电平亮，低电平熄灭

// 编码器1
#define ENC1_A PIN0_11  // BTN10
#define ENC1_B PIN1_00  // BTN9
// 编码器2
#define ENC2_A PIN0_19  // BTN20
#define ENC2_B PIN0_21  // BTN19
const int buttonPins[] = {
  PIN1_15, PIN1_13, PIN1_11, PIN1_08, PIN0_07, PIN1_02,
  PIN0_22, PIN0_24, PIN1_04, PIN1_06, PIN0_08, PIN0_06,
  PIN0_20, PIN0_17, PIN0_12, PIN0_23
};

bool blinking = false;
bool ledon = true;

// 使用ASCII字符定义键值映射
const char keymap[] = {
  'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l',
  'm', 'n', 'o', 'p'
};

const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
unsigned long lastActionTime = 0;  // 记录最后一次操作的时间

// 消抖
unsigned long debounceDelay = 30;  // 定义按钮防抖动延迟时间，单位为毫秒
unsigned long lastDebounceTime[numButtons] = {0};
bool buttonState[numButtons];
bool lastButtonState[numButtons];

void setup() {
  // Serial.begin(115200);
  // while (!Serial) delay(10); // 等待串口连接

  Bluefruit.begin();
  Bluefruit.setTxPower(4);  // 设置发射功率

  bledis.setManufacturer("Tinybox");
  bledis.setModel("TiHUBv2");
  bledis.begin();

  blehid.begin();

  for (int i = 0; i < numButtons; ++i) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    lastButtonState[i] = digitalRead(buttonPins[i]);
  }

  // 初始化编码器
  ENC1.begin(ENC1_A, ENC1_B);
  ENC1.start();
  ENC2.begin(ENC2_A, ENC2_B);

  startAdv();
}

void startAdv() {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);

  Bluefruit.Advertising.addService(blehid);
  Bluefruit.setName("TiHUBv2-KB");
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // 单位为0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // 快速模式下的秒数
  Bluefruit.Advertising.start(0);              // 0 = 不在n秒后停止广播
}

void loop() {
  bool anyKeyPressed = false;

  if (!Bluefruit.connected()) {
    return;
  }

  uint32_t bat = analogReadVDD();
  int bat_percent = constrain(map(bat, 860, 942, 0, 100), 0, 100);
  blebas.write(bat_percent);

  if (bat < 800) {
    blinking = true;
  } else if (bat > 900) {
    blinking = false;
  }

  if (blinking) {
    if (ledon == true) {
      ledon = false;
      digitalWrite(PIN_LED, LOW);
    } else {
      ledon = true;
      digitalWrite(PIN_LED, HIGH);
    }
  } else if (ledon == false) {
    ledon = true;
    digitalWrite(PIN_LED, HIGH);
  }

  unsigned long currentTime = millis();  // 获取当前时间

  for (int i = 0; i < numButtons; ++i) {
    bool reading = digitalRead(buttonPins[i]);  // 读取按钮状态

    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;

        if (buttonState[i] == LOW) {  // 检查按键是否被按下（LOW）
          blehid.keyPress(keymap[i]);  // 更新键盘按键状态
          lastActionTime = currentTime;
          anyKeyPressed = true;
        }
      }
    }

    lastButtonState[i] = reading;
  }

  int value1 = ENC1.read();
  if (value1) {
    lastActionTime = currentTime;
    blehid.keyPress(value1 > 0 ? 'o' : 'p');  // 根据旋转编码器的值更新键盘按键状态
    anyKeyPressed = true;
  }

  int value2 = ENC2.read();
  if (value2) {
    lastActionTime = currentTime;
    blehid.keyPress(value2 > 0 ? 'q' : 'r');  // 根据旋转编码器的值更新键盘按键状态
    anyKeyPressed = true;
  }

  if (!anyKeyPressed) {
    blehid.keyRelease();  // 释放所有按键
  }

  gotoSleep(currentTime);
}

void gotoSleep(unsigned long currentTime) {
  // 如果当前时间与最后一次操作时间的差值大于睡眠延迟时间，则进入睡眠模式
  if (currentTime - lastActionTime > SLEEPING_DELAY) {
    digitalWrite(PIN_LED, LOW);  // 关闭LED

    for (int i = 0; i < numButtons; ++i) {
      if (i != 2 && i != 10)  // GP3 和 GP11 以外的所有按钮
      {
        pinMode(buttonPins[i], INPUT_PULLUP_SENSE);  // 设置为唤醒引脚
      }
    }

    sd_power_system_off();  // 进入深度睡眠模式
  }
}
