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

void setup() {
  Serial.begin(115200);

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

  // 设置并开始广告
  startAdv();
}

void startAdv(void) {
  // 广告包
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_GAMEPAD);

  // 包含BLE HID服务
  Bluefruit.Advertising.addService(blegamepad);

  // 广告包中有足够的空间放置设备名称
  Bluefruit.setName("BtHub");
  Bluefruit.Advertising.addName();

  /* 开始广告
   * - 如果断开连接则启用自动广告
   * - 间隔：快速模式 = 20 ms, 慢速模式 = 152.5 ms
   * - 快速模式的超时时间为30秒
   * - 使用超时时间为0的start(timeout)将会永久广告（直到连接）
   *
   * 推荐的广告间隔
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // 单位为0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // 快速模式下的秒数
  Bluefruit.Advertising.start(0);              // 0 = 不在n秒后停止广告
}

void loop() {
  // 如果没有连接，或者
  if (!Bluefruit.connected()) return;

  Serial.println("无按键按下");
  gp.x = 0;
  gp.y = 0;
  gp.z = 0;
  gp.rz = 0;
  gp.rx = 0;
  gp.ry = 0;
  gp.hat = 0;
  gp.buttons = 0;
  blegamepad.report(&gp);
  delay(1000);

  //------------- DPAD / HAT -------------//
  Serial.println("Hat/DPAD 向上");
  gp.hat = GAMEPAD_HAT_UP;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向右上");
  gp.hat = GAMEPAD_HAT_UP_RIGHT;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向右");
  gp.hat = GAMEPAD_HAT_RIGHT;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向右下");
  gp.hat = GAMEPAD_HAT_DOWN_RIGHT;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向下");
  gp.hat = GAMEPAD_HAT_DOWN;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向左下");
  gp.hat = GAMEPAD_HAT_DOWN_LEFT;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向左");
  gp.hat = GAMEPAD_HAT_LEFT;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 向左上");
  gp.hat = GAMEPAD_HAT_UP_LEFT;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("Hat/DPAD 居中");
  gp.hat = GAMEPAD_HAT_CENTERED;
  blegamepad.report(&gp);
  delay(1000);

  //------------- 摇杆1 -------------//

  Serial.println("摇杆1 向上");
  gp.x = 0;
  gp.y = -127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆1 向下");
  gp.x = 0;
  gp.y = 127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆1 向右");
  gp.x = 127;
  gp.y = 0;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆1 向左");
  gp.x = -127;
  gp.y = 0;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆1 居中");
  gp.x = 0;
  gp.y = 0;
  blegamepad.report(&gp);
  delay(1000);


  //------------- 摇杆2 -------------//
  Serial.println("摇杆2 向上");
  gp.z = 0;
  gp.rz = 127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆2 向下");
  gp.z = 0;
  gp.rz = -127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆2 向右");
  gp.z = 127;
  gp.rz = 0;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆2 向左");
  gp.z = -127;
  gp.rz = 0;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("摇杆2 居中");
  gp.z = 0;
  gp.rz = 0;
  blegamepad.report(&gp);
  delay(1000);

  //------------- 模拟触发器1 -------------//
  Serial.println("模拟触发器1 向上");
  gp.rx = 127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("模拟触发器1 向下");
  gp.rx = -127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("模拟触发器1 居中");
  gp.rx = 0;
  blegamepad.report(&gp);
  delay(1000);

  //------------- 模拟触发器2 -------------//
  Serial.println("模拟触发器2 向上");
  gp.ry = 127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("模拟触发器2 向下");
  gp.ry = -127;
  blegamepad.report(&gp);
  delay(1000);

  Serial.println("模拟触发器2 居中");
  gp.ry = 0;
  blegamepad.report(&gp);
  delay(1000);

  //------------- 按钮 -------------//
  for (int i = 0; i < 16; ++i) {
    Serial.print("按下按钮 ");
    Serial.println(i);
    gp.buttons = (1ul << i);
    //用于设置游戏手柄的第 i 个按钮为按下状态。其中 1ul << i 表示将数字 1 的二进制形式向左移动 i 位，从而在相应的位置上创建一个标记，表示特定的按钮被按下。
    blegamepad.report(&gp);
    delay(1000);
  }

  // 随机触摸
  Serial.println("随机触摸");
  gp.x = random(-127, 128);
  gp.y = random(-127, 128);
  gp.z = random(-127, 128);
  gp.rz = random(-127, 128);
  gp.rx = random(-127, 128);
  gp.ry = random(-127, 128);
  gp.hat = random(0, 9);
  gp.buttons = random(0, 0xffff);
  blegamepad.report(&gp);
  delay(1000);
}
