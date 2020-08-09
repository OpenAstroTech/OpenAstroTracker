* Electronic Level
** General
This is a replacement for the bubble level with the aluminium extrusion body.
It uses a Gyro/Accelerometer board using the MPU6050 chip (e.g. https://www.amazon.com/gp/product/B019SX74TE/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)

** Supported Axes
Only two axis are used, roll and pitch.
- Roll is the rotation about the front to back axis of the mount, so it is changed by turning the front two M14s in different directions (tilting the mount).
- Pitch is the rotation that results when you raise or lower the back of the mount. 

Pitch is not critical to polar alignment, but roll is (unless you are plate solving and syncing the mount).

**Wiring
The module is only supported on Arduino Mega out of the box, but you could probably make it work with the ESP32.
Only 4 wires need to be connected, VCC, GND, SCL, and SDA. SCL is connected to pin 21 and SDA ic onnected to pin 20.

**Code changes
In order to use the digital level we need to enable it in the code, recompile and deploy.
In the configuration_adv.hpp file, search for GYRO_LEVEL and set it to 1. 

If your level is mounted such that the roll and pitch axis are swapped, set the GYRO_AXIS_SWAP variable to 1.

**LCD support
There are two new menu items in the LCD CAL menu, Roll Offset and Pitch Offset. 

**Calibration
Before using the digital level, we need to tell the mount what horizontal is. To do this, set the mount on a level surface and level the mount with the bubble level or just make sure the screws are all at the same point (fully unscrewed, for example). Then go to the LCD menu and into the Roll Offset and hit Select. This stores the current value as level. Do the same for the Pitch Offset.

**Usage
When you setup the mount, go to the LCD Roll menu and use the front M14s to make it level. Press the LEFT button (NOT SELECT) to quit the menu.
If you want to level in pitch, repeat, but this is not neccessary.