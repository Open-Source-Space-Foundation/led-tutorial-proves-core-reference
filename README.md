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

When you are done with this tutorial, you may want to try modifying your led-blinker component to be like the [Watchdog Component](https://github.com/Open-Source-Space-Foundation/proves-core-reference/blob/main/FprimeZephyrReference/Components/Watchdog/docs/sdd.md) that we have in the main repository. Think about how you can modify the led blinker component to comply with the requirements and standards in the Watchdog Software Design Document

## Let's Get Started!

Since most of what is being done in this tutorial is covered by the FPrime tutorial, this readme will link the relevent parts of the FPrime tutorial, and explain between the tutorial what changes you need to make to have the LED blink on the proves core reference.

1. The first thing we want to do it set up the hardware. In order to do so we need to edit the device tree. In Zephyr, the device tree is a structured data format used to describe the hardware layout to the operating system at compile time. It tells Zephyr which peripherals are available (e.g., UART, I2C, SPI), what their addresses are, how they are connected, and how they should be configured. [TO DO ADD LINKS]

In order to support various different boards in the proves-core-reference we have varients of boards. What this means is that in the v5 folder, everything that both the c and the d have in common are there.This allows us to reuse shared configuration while also customizing behavior for specific boards, like the v5c vs the v5d

the stucture of the board files is the following:

boards/bronco_space/
├── proves_flight_control_board_v5      # Common files shared by all v5 variants
├── proves_flight_control_board_v5c     # Variant C-specific files
└── proves_flight_control_board_v5d     # Variant D-specific files


Shared Base: proves_flight_control_board_v5

This folder contains everything common to both the v5c and v5d variants. These shared files include:

Kconfig.defconfig: Default Kconfig settings shared across variants.
proves_flight_control_board_v5.dtsi: The shared devicetree include file.
proves_flight_control_board_v5-pinctrl.dtsi: Common pin configuration.

This shared configuration ensures consistency and avoids code duplication. In general, when adding components to the board files, you will be editing the dtsi file.






## Running Integration Tests

First, start GDS with:
```sh
make gds
```

Then, in another terminal, run the following command to execute the integration tests:
```sh
make test-integration
```
