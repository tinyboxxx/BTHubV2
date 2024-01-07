/*********************************************************************
 这是我们基于nRF52的Bluefruit LE模块的示例

 今天就在adafruit商店购买吧！

 Adafruit投入时间和资源提供这个开源代码，
 请通过购买Adafruit的产品来支持Adafruit和开源硬件！

 遵循MIT许可证，更多信息请查阅LICENSE
 上述所有文字，以及下面的启动屏幕必须包含在
 任何重新分发中
*********************************************************************/

/* 本示例演示如何使用BLEHidGamepad发送
 * 按钮、方向键和摇杆报告
 */

#include <bluefruit.h>

BLEDis bledis;
BLEHidGamepad blegamepad;

// 在Adafruit_TinyUSB_Arduino的hid.h中定义
hid_gamepad_report_t gp;

// 定义所有按钮的引脚
const int buttonPins[] = {
    12,  // 原P0_12
    8,   // 原P0_08
    24,  // 原P0_24
    17,  // 原P0_17
    21,  // 原P0_21
    23,  // 原P0_23
    7,   // 原P0_07
    11,  // 原P0_11
    22,  // 原P0_22
    20,  // 原P0_20
    6    // 原P0_06
};
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

void setup() {
  Serial.begin(115200);

  // 初始化所有按钮引脚
  for (int i = 0; i < numButtons; ++i) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }


#if CFG_DEBUG
  // 当通过IDE启用调试模式时，阻塞等待连接
  while (!Serial) delay(10);
#endif

  Serial.println("Bluefruit52 HID Gamepad 示例");
  Serial.println("-------------------------------\n");

  Serial.println();
  Serial.println("转到手机的蓝牙设置以配对您的设备");
  Serial.println("然后打开一个接受游戏手柄输入的应用程序");

  Bluefruit.begin();
  Bluefruit.setTxPower(4);  // 查看bluefruit.h了解支持的值

  // 配置并启动设备信息服务
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
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

  // 广播包中有足够的空间放置设备名称
  Bluefruit.setName("BtHub");
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
void loop() {
  // 如果没有连接，或者
  if (!Bluefruit.connected()) return;
  // 重置按钮状态
  gp.buttons = 0;

  // 读取每个按钮的状态并更新游戏手柄的按钮状态
  for (int i = 0; i < numButtons; ++i) {
    if (digitalRead(buttonPins[i]) == LOW) { // 按钮按下
      gp.buttons |= (1 << i);
    }
  }
  // 发送游戏手柄的状态
  blegamepad.report(&gp);

  delay(100);  // 稍微延迟以减少CPU占用
}