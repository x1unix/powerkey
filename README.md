# PowerKey

> - Your password is too long to remember?
> - Try PowerKey instead!

![Preview](./docs/demo.gif)

## Brief

PowerKey is a simple single-key device which acts as a USB-keyboard and automatically types password, or any another preprogrammed string.

## Building

### Ingredients

* 1x Arduino Pro Micro
* 2x LEDs + 220Ω resistors
* 1x Cherry MX or another switch
* 1x Button
* 3D Printer (optionally, to print [housing](./docs/3d-print/PowerKeyCase.stl))

> [!NOTE]
> See [include/config.h](include/config.h) for pinout config.

## Firmware

Use `make build` and `make upload` to flash a board.

> [!IMPORTANT]
> PlatformIO is required to build the project. Install [PlatformIO CLI](https://docs.platformio.org/en/latest/core/index.html) or [VSCode extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide).

## Housing

You can find a Fusion 360 project [here](./docs/3d-print/PowerKeyCase.f3d).

Final STL file is available [here](./docs/3d-print/PowerKeyCase.stl).

## Configuration

### Firmware

See [include/config.h](include/config.h) for pinout configuration and firmware options.

### Programming

Use [PowerKey Programmer](https://x1unix.github.io/powerkey/flasher) web tool to re-program a device and update stored password.

