
RobotLights: 
  Contains software that drives an addressable
  LED light strip using various patterns useful 
  for signalling various functionality to a driver
  in an FTC competition.  The strip itself is 
  driven by a barebones ATmega328 with the Arduino
  bootloader burned onto it.  This allows the microcontroller
  to run at about 4ma.

  The mC is being driven by a Hitechnic 
  protoboard that drives digital out into into digital
  in pins on the atmel mC.  The arduino sketch reads
  the digital inputs to determine which pattern to
  display on the light strip.  An interrupt pin is
  used to signal changes in input state.

  The protoboard is controlled by software running on
  a Lego Mindstorms NXT brick (using RobotC).
