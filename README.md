# Monitor Brightness

- Requires `xrandr`

A script that uses `xrandr` to control the brightness level of your monitor on a software level, allowing you to lower it past the minimum hardware setting. <br/>
Before compiling it, head over to "brightness.c" and make sure to replace "HDMI-A-0" with your monitor's name on line 11. You can get your primary monitor's name by running:
```
xrandr | grep "connected primary" | cut -f1 -d " "
```
on your terminal. <br/>

## Usage
Usage examples:
```
# Increase brightness by 0.1
./brightness +.1

# Decrease brightness by 0.5
./brightness -.5
```
Brightness level will be capped at 1 or 0.