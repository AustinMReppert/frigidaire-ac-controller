# About
An example of how to control a Frigidaire window ac unit. Most features except the timer and remote sensing work.

# Monitor
Monitors the temperature from a shelly temperature and humidity sensor and controls the ac accordingly. Contains the API that forms the IR signals and passes them to the controller.

# Controller
The ir controller was built for esp12e, but code should work or be easily portable to other devices. It was built using PlatformIO, but can easily be ported to Arduino.
The sole purpose of the controller is to wait for commands from the monitor and transmit them over IR signals.