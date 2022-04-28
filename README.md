# Monitor Brightness

- Requires `xrandr`

A script that uses `xrandr` to control the brightness level of your monitor on a software level, allowing you to lower it past the minimum hardware setting. <br/>
Before compiling it, head over to "brightness.c" and make sure to replace "HDMI-A-0" with your monitor's name on line 11. You can get your active monitors' names by running "xrandr --listactivemonitors" on your terminal. For example, on my computer, the command outputs:
```
Monitors: 1
 0: +*HDMI-A-0 1920/598x1080/336+0+0  HDMI-A-0
```

Here we can see that I am only using ONE monitor, and its name is "HDMI-A-0". So that's the value I'll be defining my MONITORNAME to:
```
// Name of your monitor
#define MONITORNAME "HDMI-A-0"
```

Usage:
```
# Increase brightness by .1
./brightness +.1

# Decrease brightness by .5
./brightness -.5
```
Compile it with `make`:
```
make brightness
```
Brightness level will be capped at 1 or 0.