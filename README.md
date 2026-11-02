<p align="center"><strong>Rien Index Beeper Device.</strong></p>

<p align="center">
  <img src="images/Main-nRF52840.gif" alt="Rien Prescript Beeper Thumbnail" width="25%" />
    <br/>
  <a href="https://opensource.org/licenses/MIT"><img src="https://img.shields.io/badge/License-MIT-green.svg" alt="License: MIT"></a> <a href="https://quantdrent.github.io/Rien-Prescript-Beeper"><img src="https://img.shields.io/badge/Web-BLE-blue?logo=bluetooth" alt="Web BLE"></a>
</p>

<p align="center">
The main goal for this project was to make a budget Index Oracle's proxy / Rien device. This runs on Web BLE (so it connects directly to your browser without needing weird setups). This project is under the MIT license so you are free to do anything with it (this includes selling this stuff) but don't try to gatekeep lol. Also, if you decide to make this, feel free to show me! I wanna see it. Hit up @quantdrent on TikTok, Twitter, and Discord.
<br/><br/>
If you want to use the beeper, <a href="https://quantdrent.github.io/Rien-Prescript-Beeper">visit the Web App</a>. or open the html from source code<br/>
<strong>Note: You need to use a browser that supports Web BLE</strong> (Chrome, Edge, Opera, etc).
</p>

## Features
- **Web BLE Integration:** Control the device, sync settings, and beam prescripts right from Chrome/Edge/Opera (no app installs needed).
- **Custom Prescripts:** Write, import, and export your own custom prescripts via the web interface.
- **Stats Tracking:** Physically tap the capacitive touch buttons to **Pass** or **Fail** a prescript, and the device will keep track of your stats.
- **Deep Sleep Mode:** (Recommended Build only) Actually decent battery life for standby.

## Hardware Versions & Required Materials

> There are four different hardware setups in this project:
> * **The nRF52840 (Recommended Build).** It has a TFT color screen, custom fonts, and deep sleep mode. This is the recommended version you see in the GIF.
> * **The ESP32-C3 (Alternative Build).** This uses the exact same TFT color screen as the recommended build, but runs on a much cheaper ESP32-C3 SuperMini. It lacks deep sleep but is a great budget alternative.
> * **The ESP32-C3 (Legacy OLED Build).** An older, archived build that uses a basic blue 1.3" OLED screen. You can find its firmware in `firmware/ESP32/PrescriptBeeper_Legacy_OLED`.
> * **The LilyGO T-Display-S3 (One-Button Build).** A ready-made ESP32-S3 board with a 1.9" 170x320 TFT screen on the PCB. It uses one TTP223 touch button and one active buzzer. Build it with Nix and arduino-cli. Firmware is in `firmware/T-Display-S3`.
> 
> **DO NOT mix the parts (if you dont know what youre doing).** The firmwares and 3D printed cases are **NOT** interchangeable between the different screens and boards. Make sure you pick the right firmware and 3D model for your parts.

<details>
<summary><strong>Recommended Build (nRF52840)</strong></summary>

This is the recommended version with a tft color screen, custom proportional fonts, and deep sleep support.

> **Note:** Don't forget to solder the boost pads on the SuperMini board if your battery is more than 500mAh.
>
> The TFT `BL` (Backlight) pin MUST be wired to `GND` for the screen to turn on

**Required Parts:**
- SuperMini nRF52840 [[AliExpress](https://www.aliexpress.com/item/1005006019812115.html)]
- 2.25 Inch TFT LCD Module 76x284 ST7789 [[AliExpress](https://www.aliexpress.com/item/1005011855033572.html)]
- 2x Touch capacitive switches TTP-223 [[AliExpress](https://www.aliexpress.com/item/32964219843.html)]
- Any small buttons that can fit inside the case
- Wires (AWG 24 recommended)
- 3.7V Lipo Battery (just make sure it fits in the case)

**Required Arduino Libraries:**
- `Adafruit GFX Library`
- `Adafruit LittleFS`
- `Adafruit ST7735 and ST7789 Library`
**3D Printing (Case):**
- **For the Recommended Build (nRF52840 Supermini):** Use `models/Case.3mf` or `models/Case.stl`
- *(Tinkercad remix link available in `models/tinkercadlink.txt`. I recommend using `.3mf` files. Best results were on a BambuLab A1 with 0.4 nozzle and SUNLU PLA+ 2.0)*

**Wiring Diagram:**
<p align="center">
  <img src="images/Main-nRF52840-wiring.png" width="50%" alt="Main nRF52840 Wiring">
</p>

#### Board Setup (Arduino IDE)
1. Open Arduino v2.x IDE.
2. Go to **File > Preferences**.
3. Click the icon on the right-hand side of “Additional boards manager URLs”.
4. Add the following URL lines into the text box and click OK:
   * **Temporary fix (use this for now):**
     `https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json`
     `https://raw.githubusercontent.com/pdcook/nRFMicro-Arduino-Core/3dab6477754d9b28053fe36b06c718cde6e93d3f/package_nRFMicro_index.json`
   * *(Note: The official URL without the hash currently has issues, use the temporary fix).*
5. Go to the Boards Manager (**Tools > Board > Boards Manager**).
6. Search for “nrfmicro”.
7. Find the “nRFMicro-like-Boards” by pdcook, and click “Install”.
8. Go to **Tools > Board > nRFMicro-like-Boards > “SuperMini nRF52840”** (Restart Arduino IDE if it is not listed).
9. Plug in your board to a USB port, click the board selector, and choose the relevant port.

#### Updating the Bootloader
The SuperMini nRF52840 boards have nice!nano v2 compatibility, meaning the Adafruit nRF52 Bootloader can be easily updated.

1. Open your file explorer and look for a USB drive related to your board, often called “NICENANO” (if it isn’t visible, quickly ground the RST pin twice to enter bootloader mode).
2. Inside, `INFO_UF2.TXT` is a small text file that states the current bootloader version.
3. You can see the latest bootloader version on the [bootloader github releases page](https://github.com/adafruit/Adafruit_nRF52_Bootloader/releases).
4. **To update via Arduino IDE:**
   * Go to **Tools > Programmer**, and select **“Bootloader DFU for Bluefruit nRF52”**.
   * Go to **Tools > Burn Bootloader**.
   * **Be patient whilst the bootloader is updated. It is critical not to disconnect mid-update, as you could brick the board.**
5. Once updated, recheck the `INFO_UF2.TXT` file to ensure the version has updated (e.g., from 0.6.0 to 0.9.2).
</details>

<details>
<summary><strong>Alternative Build (ESP32-C3 TFT)</strong></summary>

This version is kept here because the ESP32 parts are generally much cheaper and easier to source than the nRF52840, while still allowing you to use the premium TFT color screen.

> **If you are using an ESP32C3** check out [this guide](https://randomnerdtutorials.com/getting-started-esp32-c3-super-mini/) to set up your board in Arduino IDE.
>
> The TFT `BL` (Backlight) pin MUST be wired to `GND` for the screen to turn on

instruction for this version have not been completed. this was a port made by someone else

**Required Parts:**
- ESP32C3 SuperMini (any ESP32 SuperMini should work)
- 2.25 Inch TFT LCD Module 76x284 ST7789 [[AliExpress](https://www.aliexpress.com/item/1005011855033572.html)]
- 2x Touch capacitive switches TTP-223 [[AliExpress](https://www.aliexpress.com/item/32964219843.html)]
- Any small slide switch that fits inside the case [[AliExpress](https://www.aliexpress.com/item/1005011902173007.html)]
- Wires (AWG 26 recommended)

(optional)
- Any small slide switch that fits inside the case [[AliExpress](https://www.aliexpress.com/item/1005011902173007.html)]
- TP4056 Type-C Charger [[AliExpress](https://www.aliexpress.com/item/1005006043031985.html)]
- 3.7V Lipo Battery (just make sure it fits in the case)

**Required Arduino Libraries:**
- `Adafruit GFX Library`
- `Adafruit LittleFS`
- `Adafruit ST7735 and ST7789 Library`

**3D Printing (Case):**
- **For the Alternative Build (ESP32-C3 Supermini):** Use `models/Case.3mf` or `models/Case.stl`
- Best results were on a BambuLab A1 with 0.4 nozzle and SUNLU PLA+ 2.0)*

**Wiring Table:**
| ESP32-C3 SuperMini Pin | Component / Peripheral Pin |
| :--- | :--- |
| **`GPIO 0`** | TFT `MOSI` (or `SDA`) |
| **`GPIO 1`** | TFT `SCLK` (or `SCL`) |
| **`GPIO 2`** | TFT `CS` |
| **`GPIO 3`** | TFT `DC` (or `RS`) |
| **`GPIO 4`** | TFT `RST` |
| **`GPIO 5`** | Pass Button (TTP-223) `I/O` |
| **`GPIO 6`** | Fail Button (TTP-223) `I/O` |
| **`3.3V`**   | TFT `VCC`, Buttons `VCC` |
| **`GND`**    | TFT `GND`, TFT `BL`, Buttons `GND` |

*(Note: For the charging module, solder it the exact same way as shown in the Legacy version's wiring diagram. The slide switch does not connect to a GPIO pin; wire it inline with the OUT+ of the charging module to act as a physical power switch).*
</details>

<details>
<summary><strong>Legacy Build (ESP32-C3 OLED)</strong></summary>

This is the archived legacy version using a monochrome OLED. You can find its firmware in `firmware/ESP32/PrescriptBeeper_Legacy_OLED`.

**Required Parts:**
- ESP32C3 Super Mini
- 1.3 Inch OLED Screen SH1106 [[AliExpress](https://www.aliexpress.com/item/1005006862867338.html)]
- 2x Touch capacitive switches TTP-223
- Wires (AWG 26 recommended)

(optional)
- Any small slide switch that fits inside the case [[AliExpress](https://www.aliexpress.com/item/1005011902173007.html)]
- TP4056 Type-C Charger [[AliExpress](https://www.aliexpress.com/item/1005006043031985.html)]
- 3.7V Lipo Battery (just make sure it fits in the case)

**Required Arduino Libraries:**
- `Adafruit GFX Library`
- `Adafruit SH110X`

**3D Printing (Case):**
- **For the Legacy Build (ESP32-C3 Supemini):** Use `models/Legacy-Case.3mf` or `models/Legacy-Case.stl`
- I recommend using `.3mf` files. Best results were on a BambuLab A1 with 0.4 nozzle and SUNLU PLA+ 2.0)*

**Wiring Diagram:**
<p align="center">
  <img src="images/Legacy-ESP32-wiring.png" width="50%" alt="Legacy ESP32 Wiring">
</p>

*(Note: The slide switch does not connect to a GPIO pin; wire it inline with the OUT+ of the charging module to act as a physical power switch, as shown in the diagram).*
</details>

<details>
<summary><strong>One-Button Build (LilyGO T-Display-S3)</strong></summary>

This build uses the LilyGO T-Display-S3. The board has the ESP32-S3, 16 MB flash, 8 MB PSRAM, a 1.9 inch 170x320 ST7789 TFT and two push buttons on the PCB. You add one TTP223 touch button and one active buzzer. No screen wiring is necessary.

> **Note:** The screen on this board uses an 8-bit parallel bus, not SPI. The firmware uses the Arduino_GFX library for it. Do not use the `firmware/ESP32` sketch on this board.

**Required Parts:**
- LilyGO T-Display-S3 (non-touch version) [[LilyGO repo](https://github.com/Xinyuan-LilyGO/T-Display-S3)]
- 1x Touch capacitive switch TTP223
- 1x Active buzzer (2-pin part, or 3-pin module)
- Wires
- (optional) 3.7V LiPo battery with a 1.25 mm JST plug for the battery connector on the board

**Wiring Table:**
| T-Display-S3 Pin | Component Pin |
| :--- | :--- |
| **`GPIO 16`** | TTP223 `I/O` |
| **`GPIO 10`** | Buzzer `+` (2-pin part) or buzzer module `I/O` |
| **`3V3`** | TTP223 `VCC`, buzzer module `VCC` |
| **`GND`** | TTP223 `GND`, buzzer `-` or buzzer module `GND` |

*(Notes: The TTP223 module gives a HIGH output while you touch it. This is the factory setting with the A and B solder pads open. If you close pad B, the output is LOW while you touch it. Then set `TOUCH_ACTIVE_LEVEL` to `LOW` in `config.h`. A GPIO pin can drive a small active buzzer that uses 20 mA or less. If your buzzer uses more current, drive it through an NPN transistor. If your buzzer module turns on when the pin is LOW, set `BUZZER_ACTIVE_LEVEL` to `LOW`.)*

**Preloaded Prescripts:**
The firmware carries the prescripts from `prescriptexamples/RienPrescripts.txt`. On the first boot, and after a factory reset, it writes them to the device storage. After that the web app owns the list: add, remove, import and export work as on the other builds. A prescript can have up to about 240 characters. Longer lines are cut when the device reads them back. To ship a different default set, run `firmware/T-Display-S3/gen_default_prescripts.sh yourfile.txt > firmware/T-Display-S3/PrescriptBeeper/default_prescripts.h` and rebuild.

**Screen Insets:**
If the case hides the edge of the screen, set `SCREEN_INSET_TOP`, `SCREEN_INSET_BOTTOM`, `SCREEN_INSET_LEFT` and `SCREEN_INSET_RIGHT` in `config.h`. The values are in pixels. The firmware keeps that area black and lays out the text in the area that is visible.

**Controls:**
| Input | When idle | When a prescript waits for a response |
| :--- | :--- | :--- |
| Touch button, tap | Show a random prescript | **Pass** |
| Touch button, hold 0.7 s | Nothing | **Fail** |
| `KEY` button on the PCB (GPIO 14) | Nothing | **Fail** |
| `BOOT` button on the PCB (GPIO 0) | Cycle the volume: high, mid, low, mute | Cycle the volume, with a confirmation beep |

**Buzzer Sounds:**
- New prescript: one click for each character as the text resolves, then four short beeps and one long beep when the message is complete. This echoes the sound of the web app.
- Pass: one short beep.
- Fail or timeout: one long beep.
- Last three seconds of a timer: one tick each second.
- Connected: two short beeps. Disconnected: one medium beep.
- The `BOOT` button cycles the volume: high, mid, low, mute. The default is mid. The screen shows the new level when idle. The setting stays after a power cycle.
- The buzzer is an active type. Full volume drives it with a steady level. The lower levels chop the drive at 1 kHz with a smaller duty cycle, which this buzzer renders as three clearly different loudness steps. A 20 kHz chop does not work with it.

**Serial Debug Commands:**
Connect to the USB serial port at 115200 baud and send one command per line. `HELP` lists them. Useful ones: `SHOW text` shows a text with sounds and counts as a prescript in the stats, `BEEP on,off,on,off,...` plays a custom pattern in milliseconds, `CLICK ms` sets the click length, `TERM on,off,...` sets the termination pattern, `MUTE 0|1`, `BAT` prints the voltage on the battery pin. Open the port in raw mode, for example with pyserial or `pio device monitor`. A plain `cat` on the port echoes the device output back to it. With USB attached, `BAT` reads the charger rail at about 4.8 V, not the cell.

**Toolchain (Nix):**
The repository has a `flake.nix` built on [arduino-nix](https://github.com/bouk/arduino-nix). Nix fetches `arduino-cli`, the esp32 core 3.3.12 with its compilers, and the `GFX Library for Arduino` 1.6.9 into the Nix store. The versions are pinned in `flake.nix` and `flake.lock`. The first build downloads about 1.5 GB.

```sh
nix develop
beeper-build            # compile the sketch in firmware/T-Display-S3/PrescriptBeeper
beeper-flash            # flash over USB-C, default port /dev/ttyACM0
beeper-monitor          # serial log at 115200 baud
```

The scripts call `arduino-cli` with the board `esp32:esp32:lilygo_t_display_s3`. You can also run `arduino-cli` directly inside the shell.

> **Note:** The Espressif compilers are prebuilt binaries. On NixOS they run through `nix-ld`, so enable `programs.nix-ld` in your system configuration. Your user must also be in the `dialout` group to open the serial port.

If the upload fails, hold `BOOT`, press `RST`, release `BOOT`, and upload again.

**Arduino IDE:** The sketch folder also opens in the Arduino IDE 2.x. Install the esp32 core 3.x and the `GFX Library for Arduino`. Select the board **ESP32S3 Dev Module** with Flash Size 16MB, PSRAM **OPI PSRAM**, USB CDC On Boot **Enabled** and Partition Scheme **16M Flash (3MB APP/9.9MB FATFS)** or any 16 MB scheme. The `partitions.csv` file in the sketch folder replaces the selected scheme and gives LittleFS its partition. The board entry **LilyGo T-Display-S3** also works, but it does not enable PSRAM. The Nix build script passes `-DBOARD_HAS_PSRAM` for that reason.

**3D Printing (Case):**
There is no case model for this build yet. LilyGO sells a shell for the board.

**Sources:** Pin numbers come from `examples/factory/pin_config.h` and the Arduino_GFX example in the [LilyGO T-Display-S3 repository](https://github.com/Xinyuan-LilyGO/T-Display-S3). TTP223 behaviour comes from the [TTP223 datasheet](https://www.sunrom.com/get/396500).
</details>

## Quickstart

### Instructions

> **Note:** some knowledge of soldering is needed. Don't forget you need a soldering iron, solder, and maybe some flux to connect them all

1. Click the `<> Code` button and download the ZIP, or check releases.
2. Open the `firmware/` folder, select your microcontroller's folder, and open the `.ino` file in the Arduino IDE.
3. Solder everything together based on the pins defined in your firmware's `config.h` file or refer to the hardware sections above for wiring diagrams. 
4. Upload the sketch to your board.
5. Open the [Web App](https://quantdrent.github.io/Rien-Prescript-Beeper) in your browser (or open the local `index.html` file to use offline).
6. Click **Pair Device** and connect to your beeper and your device should work now.
7. Click **Prescripts** at the top middle to add or remove your saved prescripts. You can also export/import them (export first to see the formatting template).
8. Click the giant button on the web app to send a random prescript, or physically tap the capacitive buttons on your device to pull one up.
9. You can factory reset the device by pressing the red button at the bottom right of the settings menu.

## Credits
This project and the Web BLE page were inspired by and forked from Kritzkingvoid's Prescript web project.
https://kritzkingvoid.github.io/Prescripts/

Sounds were taken from Limbus Company / Project Moon.
