# BTHubV2
new bluetooth steering wheel hub based on nrf52840

Firstly, in terms of design, it has changed from one large board to two small boards connected with one using the front side and the other using the back side, and the size of the boards has been reduced to just 10x10 cm, which is convenient for prototyping with JLCPCB.

Secondly, the main controller has been changed from ESP32 to NRF52840, which has significantly reduced power consumption and greatly enhanced battery life, from the original two to three days of battery life to almost infinite battery life now.

Thirdly, the previous mechanical keyboard switches have been replaced with simple 12x12 buttons with button caps, and the feel is quite good. After applying stickers, it also looks very professional.

Fourthly, the shift paddle buttons that previously required wiring connections have been changed to onboard connections, making the production process much more convenient.

## PCB: https://oshwhub.com/tinybox/bthubv2

## 3D print: https://makerworld.com/zh/models/388164#profileId-288880

# 希望被大家改进的功能：

1. 增加电量显示（检测目前已实现），可以配合第一条的LED闪烁之类的提醒用户。
2. 给每个双面按钮增加双面的LED，单色即可，暗一点，可能需要mos从电池驱动。
3. 大拇指附近再增加侧面的旋转编码器。
4. 把右手侧下方的编码器换成旋转波段开关，用来切换模式（可能需要通过不同阻值或移位寄存器增加通道数）
5. 把其中一个按钮改为七向摇杆。
6. 欢迎提出其他增加用户体验的新功能。

以上功能如果可用，我会合并进主分支，大家一起促进开源社区和模拟器的进步，谢谢！
