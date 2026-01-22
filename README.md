# SpiLoopback
Simple STM32F303RE Nucleo board showing off SPI master loopback &amp; SPI master with slave
This was in part because somebody down-counted my reply proposing that this was a means
to check code.

Note: working NOW !!

A picture of my NUCLEO-F303RE setup is included. Waveforms now included!! Saleae Logic Pro files included!!

Note:

The Makefile expects the environment variable PLATFORM to be set to the PLATFORM of the
target system: STM32F303

The build directory will then be "build.STM32F303". This allows multiple PLATFORMs
(targets) to be built within the same project directory, when appropriate.

