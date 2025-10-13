# LED Tutorial Proves Core Reference Project

Welcome to the tutorial reposiotory! This will walk you through making your first componet on the PROVES Core Reference on the [Proves Kit](https://docs.proveskit.space/en/latest/)!

For actual hardware testing, you need a flight controller board and a USBC cable to plug into the board. Currently v5c's and v5d's are supported

## System Requirements
- F Prime System Requirements listed [here](https://fprime.jpl.nasa.gov/latest/docs/getting-started/installing-fprime/#system-requirements)
- Zephyr dependencies listed [here](https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-dependencies) (NOTE: Only complete the install dependencies step, as we run through the rest of the steps in this readme.)

## Installation

First, clone the Proves Core Reference repository. If you want to push changes, you might want [to make a fork] and clone that!(https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo)

```shell
git clone https://github.com/Open-Source-Space-Foundation/proves-core-reference
```

Next, navigate to the `proves-core-reference` directory and run `make` to set up the project.

```shell
cd proves-core-reference
make
```

## Running the code

Run generate from the `proves-core-reference` directory. This generates the build cache for FPrime. You only need to do generate if something in the core FPrime package has changed
```shell
make generate
```

Then, and every time you change code, run

```shell
make build
```

### Find the path to your board

Next, plug in your board! If you have previously installed a firmware on your board you may not see it show up as a drive. In that case you'll want to put the board into boot loader mode. Then you'll be able to find the location of the board on your computer. It should be called something like RP2350 but you want to find the path to it

For Mac:
```shell
ls -lah /Volumes
```

For Windows:
Check the letter said to be the mount (ex /d/) and then the name of the removable drive (ex /d/RP2350)

For Linux:
```shell
findmnt
```

Now you want to install the firmware to the board.
```shell
cp build-artifacts/zephyr.uf2 [path-to-your-board]
```

Finally, run the fprime-gds.
```shell
make gds
```

## Answers 
If you want to check out the answers, they are on the answers branch

```shell
git checkout answers
```

or

```shell
git diff answers <your_branch>
```

## Let's Get Started!

# Part 1: Board Definition
The first thing we want to do it set up the hardware. In order to do so we need to edit the device tree. In Zephyr, the device tree is a structured data format used to describe the hardware layout to the operating system at compile time. It tells Zephyr which peripherals are available (e.g., UART, I2C, SPI), what their addresses are, how they are connected, and how they should be configured. It is a good idea to give a [quick read of the zephyr device tree docs and familiize yourself with the Zephyr terminalogy](https://docs.zephyrproject.org/latest/build/dts/howtos.html)

In order to support various different boards in the proves-core-reference we have varients of boards. What this means is that in the v5 folder, everything that both the c and the d have in common are there.This allows us to reuse shared configuration while also customizing behavior for specific boards, like the v5c vs the v5d

the stucture of the board files is the following:

boards/bronco_space/
├── proves_flight_control_board_v5      # Common files shared by all v5 variants
├── proves_flight_control_board_v5c     # Variant C-specific files
└── proves_flight_control_board_v5d     # Variant D-specific files
└── ... as we add more varients of the v5 boards there will be more folders here

# Shared Base: proves_flight_control_board_v5

This folder contains everything common to both the v5c and v5d variants. For Instance:

Kconfig.defconfig: Default Kconfig settings shared across variants.
proves_flight_control_board_v5.dtsi: The shared devicetree include file.
proves_flight_control_board_v5-pinctrl.dtsi: Common pin configuration.

This shared configuration ensures consistency and avoids code duplication. 

# Variant-Specific: proves_flight_control_board_v5d
This folder contains everything specific to the v5d board variant. It extends the shared configuration from proves_flight_control_board_v5 and defines settings, hardware layout, and metadata unique to the v5d.

For instance:

**board.cmake:** CMake script for defining board-specific build behavior such as custom flags, source files, or toolchain tweaks.
**board.yml:** Metadata about the board, including architecture, SoC, and supported features. Used by Zephyr tools and documentation.
**Kconfig.proves_flight_control_board_v5d:** Kconfig fragment that defines or overrides configuration options specific to v5d.
**proves_flight_control_board_v5d_rp2350a_m33.dts:** Device Tree Source file describing the v5d hardware layout—peripherals, memory, pin mappings, etc.
**proves_flight_control_board_v5d_rp2350a_m33_defconfig:** Default Kconfig settings loaded when building for this board. Enables necessary drivers and features for v5d.
**proves_flight_control_board_v5d_rp2350a_m33.yaml:** Board metadata used by Zephyr's tooling to describe memory layout, CPU, and supported features.

This structure allows us to keep hardware-specific settings isolated per variant while still benefiting from the shared configuration in the base folder.

In general, when adding components to the board files, you will be adding the most updated version of the board to the proves_flight_control_board_v5.dtsi file, and updating the other dts with any differences! For this board, you need to add the LED GPIO pins.

1. Add GPIO pins for the v5 device tree. Under

leds {
		compatible = "gpio-leds";

add 
    
led0: led0 {
			gpios = <&gpio0 23 GPIO_ACTIVE_HIGH>;
			label = "Watchdog LED";
		};

next to burnwire 0 and 1. This sets the GPIO pin 23 to be the one associated with the led. But the LED pin is different from the c's and the d's! 

Add

// Configure the Watchdog LED
&led0 {
	gpios = <&gpio0 24 GPIO_ACTIVE_HIGH>;
};

This doesnt have to be nested in anything. For boards where the LED is connected to a different pin—such as the C and D variants—we override just the gpios property using a node reference (&led0) and assign it to GPIO pin 24. This approach avoids duplication and keeps the device tree organized by centralizing common settings while allowing for easy customization.


2. In ReferenceDelpoymentTopology.cpp, you want to add the following line:

   static const struct gpio_dt_spec ledGpio = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);

This retrieves the GPIO configuration for the led0 device (that you added to the device tree in the previous step) directly from the device tree using its node label. GPIO_DT_SPEC_GET extracts the GPIO controller, pin number, and flags (like active high/low) from the gpios property of the led0 node. 


3.

# Part 2: Follow the F Prime Tutorial

Since most of what is being done in this tutorial is covered by the FPrime tutorial, this readme will link the relevent parts of the FPrime tutorial, and explain between the tutorial what changes you need to make to have the LED blink on the proves core reference. Something to note is that instead of running fprime-util generate -f and fprime-util build and fprime-gds you can run make generate, make build and make gds


# Part 3: Make it into a watchdog

Now, try to create something for the flight model by using the understanding of the watchdog component

1. add ```-watchdog``` to the bottom of the supported
supported:
  -  ...
  - watchdog

You also want to change

	aliases {
		watchdog0 = &wdt0;
	};

Now, try and modify the actual led-blinker component to be like the [Watchdog Component](https://github.com/Open-Source-Space-Foundation/proves-core-reference/blob/main/FprimeZephyrReference/Components/Watchdog/docs/sdd.md) that we have in the main repository. Think about how you can modify the led blinker component to comply with the requirements and standards in the Watchdog Software Design Document



## Running Integration Tests

First, start GDS with:
```sh
make gds
```

Then, in another terminal, run the following command to execute the integration tests:
```sh
make test-integration
```
