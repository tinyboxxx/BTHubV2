/*********************************************************************
 这是我们基于nRF52的Bluefruit LE模块的示例

 今天就在adafruit商店购买吧！

 Adafruit投入时间和资源提供这个开源代码，
 请通过购买Adafruit的产品来支持Adafruit和开源硬件！

 遵循MIT许可证，更多信息请查阅LICENSE
 上述所有文字，以及下面的启动屏幕必须包含在
 任何重新分发中
*********************************************************************/

#include <bluefruit.h>
#include "RotaryEncoder.h"

BLEDis bledis;
BLEHidGamepad blegamepad;
HwRotaryEncoder ENC1;
SwRotaryEncoder ENC2;

// 在Adafruit_TinyUSB_Arduino的hid.h中定义
hid_gamepad_report_t gp;

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

// 按钮通道
#define ENC1L 16
#define ENC1R 17
#define ENC2L 18
#define ENC2R 19

// 定义GP通道的每个IO
#define GP_1 PIN1_15   // BTN1
#define GP_2 PIN1_13   // BTN2
#define GP_3 PIN1_11   // BTN3
#define GP_4 PIN1_08   // BTN4
#define GP_5 PIN0_07   // BTN5
#define GP_6 PIN1_02   // BTN6
#define GP_7 PIN0_22   // BTN7
#define GP_8 PIN0_24   // BTN8
#define GP_9 PIN1_04   // BTN11
#define GP_10 PIN1_06  // BTN12
#define GP_11 PIN0_08  // BTN13
#define GP_12 PIN0_06  // BTN14
#define GP_13 PIN0_20  // BTN15
#define GP_14 PIN0_17  // BTN16
#define GP_15 PIN0_12  // BTN17
#define GP_16 PIN0_23  // BTN18

// P1.10/P1.09 暂未使用。由于定义问题，直接写为9,10。最多25个IO
// 定义所有按钮
const int buttonPins[] = {
  GP_1, GP_2, GP_3, GP_4, GP_5, GP_6, GP_7, GP_8, GP_9, GP_10,
  GP_11, GP_12, GP_13, GP_14, GP_15, GP_16
};

bool blinking = false;
bool ledon = true;

const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
// 定义模拟量输入引脚
const int analogPins[] = { PIN0_31, PIN0_29 };
unsigned long lastActionTime = 0;   // 记录最后一次操作的时间
bool prev_gp_buttons = gp.buttons;  // 记录最后一次操作

void setup() {
  Serial.begin(115200);
  digitalWrite(PIN_LED, HIGH);  // P0.15

  // 初始化所有按钮引脚
  for (int i = 0; i < numButtons; ++i) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  // 初始化所有模拟量输入引脚
  for (int i = 0; i < sizeof(analogPins) / sizeof(analogPins[0]); ++i) {
    pinMode(analogPins[i], INPUT);
  }

  ENC1.begin(ENC1_A, ENC1_B);
  ENC1.start();
  ENC2.begin(ENC2_A, ENC2_B);

#if CFG_DEBUG
  // 当通过IDE启用调试模式时，阻塞等待连接
  while (!Serial)
    delay(10);
#endif

  Bluefruit.begin();
  Bluefruit.setTxPower(8);  // 查看bluefruit.h了解支持的值

  // 配置并启动设备信息服务
  bledis.setManufacturer("Tinybox");
  bledis.setModel("TiHub v2");
  bledis.begin();

  /* 启动BLE HID
   * 注意：苹果要求BLE设备的最小连接间隔必须>= 20ms
   * （连接间隔越小，我们发送数据越快）。
   * 但是对于HID和MIDI设备，苹果可以接受最小连接间隔
   * 高达11.25 ms。因此BLEHidAdafruit::begin()会尝试将最小和最大
   * 连接间隔设置为11.25 ms和15 ms，以获得最佳性能。
   */
  blegamepad.begin();

  /* 将连接间隔（最小值，最大值）设置为您偏好的值。
   * 注意：它已经被BLEHidAdafruit::begin()设置为11.25ms - 15ms
   * 最小值 = 9*1.25=11.25 ms, 最大值 = 12*1.25= 15 ms
   */
  /* Bluefruit.Periph.setConnInterval(9, 12); */

  // 设置并开始广播
  startAdv();
}

void startAdv(void) {
  // 广播包
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_GAMEPAD);

  // 包含BLE HID服务
  Bluefruit.Advertising.addService(blegamepad);
  Bluefruit.setName("TiHub v2");
  Bluefruit.Advertising.addName();

  /* 开始广播
   * - 如果断开连接则启用自动广播
   * - 间隔：快速模式 = 20 ms, 慢速模式 = 152.5 ms
   * - 快速模式的超时时间为30秒
   * - 使用超时时间为0的start(timeout)将会永久广播（直到连接）
   *
   * 推荐的广播间隔
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // 单位为0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // 快速模式下的秒数
  Bluefruit.Advertising.start(0);              // 0 = 不在n秒后停止广播
}
bool dir = LOW;
int value1 = 0;
int value2 = 0;
void loop() {

  // 如果没有连接，或者
  if (!Bluefruit.connected())
    return;
  uint32_t bat = analogReadVDD();
  if (bat < 800)  // 940→4.12v, 800~3V 640~1.7V
  {
    blinking = true;
  } else if (bat > 900) {
    blinking = false;
  }

  if (blinking) {
    if (ledon = true) {
      ledon = false;
      digitalWrite(PIN_LED, LOW);
    } else {
      ledon = true;
      digitalWrite(PIN_LED, HIGH);
    }
  } else if (ledon = true) {
    ledon = false;
    digitalWrite(PIN_LED, LOW);
  }

  unsigned long currentTime = millis();  // 获取当前时间
  // 读取每个按钮的状态并更新游戏手柄的按钮状态
  for (size_t t = 0; t < 80; t++)  // 进行10次更新
  {
    // 重置按钮状态和轴状态
    memset(&gp, 0, sizeof(hid_gamepad_report_t));
    for (int i = 0; i < numButtons; ++i) {
      if (i == 2 || i == 10)  // GP3 和 GP11
      {
        dir = HIGH;
      } else {
        dir = LOW;
      }
      if (digitalRead(buttonPins[i]) == dir) {  // 按钮按下
        gp.buttons |= (1 << i);
        lastActionTime = currentTime;  // 更新最后一次操作时间
      }
      value1 = ENC1.read();
      if (value1) {
        lastActionTime = currentTime;
        gp.buttons |= (value1 > 0 ? (1 << ENC1L) : (1 << ENC1R));  // 直接根据 value 的正负更新相应的按键状态
      }
      value2 = ENC2.read();
      if (value2) {
        lastActionTime = currentTime;
        gp.buttons |= (value2 > 0 ? (1 << ENC2L) : (1 << ENC2R));
      }
    }
    // 发送游戏手柄的状态
    if (gp.buttons != prev_gp_buttons) {
      blegamepad.report(&gp);
      prev_gp_buttons = gp.buttons;
    }
  }
  // 读取模拟量输入引脚
  // Serial.print(analogRead(analogPins[0]));
  // Serial.print("\t");
  // Serial.println(map(analogRead(analogPins[0]), 0, 1023, -32768, 32767));

  // gp.x = map(analogRead(analogPins[0]), 0, 1023, -32768, 32767);  // 假设为X轴
  // gp.y = map(analogRead(analogPins[1]), 0, 1023, -32768, 32767);  // 假设为Y轴

  gotoSleep(currentTime);
}
void gotoSleep(unsigned long currentTime) {
  // 如果当前时间与最后一次操作时间的差值大于睡眠延迟时间，则进入睡眠模式
  if (currentTime - lastActionTime > SLEEPING_DELAY) {
    // to reduce power consumption when sleeping, turn off all your LEDs (and other power hungry devices)
    digitalWrite(PIN_LED, LOW);  // P0.15

    // setup your wake-up pins.
    for (int i = 0; i < numButtons; ++i) {
      if (i != 2 && i != 10)  // GP3 和 GP11 以外的所有按钮
      {
        pinMode(buttonPins[i], INPUT_PULLUP_SENSE);  // this pin (WAKE_LOW_PIN) is pulled up and wakes up the feather when externally connected to ground.
      }
    }

    // pinMode(WAKE_HIGH_PIN, INPUT_PULLDOWN_SENSE); // this pin (WAKE_HIGH_PIN) is pulled down and wakes up the feather when externally connected to 3.3v.

    // power down nrf52.
    sd_power_system_off();  // this function puts the whole nRF52 to deep sleep (no Bluetooth).  If no sense pins are setup (or other hardware interrupts), the nrf52 will not wake up.
  }
}