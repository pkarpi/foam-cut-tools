# Foam cutting tools

## Getting Started
These programs are for a hotwire cutter, which consist of two moving carriages and a steel wire
between them. Four stepper motors move the carriages of the hotwire. As an output you can get
a foam wing blank cut out of a foam block.

This project consist of windows uwp programs **styroXcut and styroXjog** and arduino program
**runsteppers.ino**. Windows programs are also available in microsoft store if you want
to install them without compiling. With styroXjog you can move the wire vertically and horizontally.
With styroXcut you can cut wing blanks using profile files from for example uiuc airfoil database
(https://m-selig.ae.illinois.edu/ads/coord_database.html)

### Prerequisites
You need a windows machine, **arduino uno** and **cnc shield** for it.
Connect stepper motors and suitable power input to your cnc-shield. 
Install arduino windows driver, connect usb cable between windows machine and arduino.

### Installing
With arduino ide, send **runsteppers.ino** to arduino. Load styroXjog from microsoft
store and connect and run.