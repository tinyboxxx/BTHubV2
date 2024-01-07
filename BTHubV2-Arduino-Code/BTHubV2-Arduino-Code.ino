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

const int buttonPin = 2;  // P0.02用作按钮输入

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);  // 将按钮引脚设置为输入，并启用内部上拉电阻


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

  // 读取按钮状态
  bool buttonState = digitalRead(buttonPin);

  // 更新游戏手柄的状态
  gp.buttons = buttonState ? 0x00 : 0x01;  // 如果按钮被按下，设置第一个按钮的状态为1，否则为0

  // 发送游戏手柄的状态
  blegamepad.report(&gp);

  delay(100);  // 稍微延迟以减少CPU占用
}