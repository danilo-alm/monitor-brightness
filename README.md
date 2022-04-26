# Monitor Brightness

- Requires `xrandr`

A script that uses `python` and `xrandr` to control the brightness level of your monitor on a software level, allowing you to go past the minimum hardware setting. <br/>
Before using it, head over to "monitor.json" and run "xrandr --listactivemonitors" on your terminal to find out the name of the monitor you're using. For example, on my computer, the command outputs:
```
Monitors: 1
 0: +*HDMI-A-0 1920/598x1080/336+0+0  HDMI-A-0
```
Here we can see that I am only using ONE monitor, and its name is "HDMI-A-0". So that's the value I'll be setting my "monitor" to in the JSON file:
```
{
    "comment": "You can get the name of your monitor by running 'xrandr --listactivemonitors' ",
    "monitor": "HDMI-A-0"
}
```

Usage:
```
# Increase brightness by .1
python brightness.py +.1

# Decrease brightness by .5
python brightness.py -.5
```
Brightness level will be capped at 1.
