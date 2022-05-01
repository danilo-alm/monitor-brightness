# Monitor Brightness

- Requires `xrandr`

A script that uses `xrandr` to control the brightness level of your monitor on a software level, allowing you to lower it past the minimum hardware setting. <br/>

## Usage
Usage examples:
```
# Increase brightness by 1
./brightness 1

# Decrease brightness by 2
./brightness -2
```

## How it works
On the first time you run it, it runs `xrandr | grep "connected primary" | cut -f1 -d " "` to get the name of your primary monitor and saves it in "$HOME/.brightness/monitor". The current brightness will be stored in "$HOME/.brightness/brightness" as a int from 0 to 10. <br/>
The program will now fetch the name of the monitor as well as the current brightness from the files stored under "$HOME/.brightness/" to save time. <br/>
If you've changed your monitor and need to detect your new one, run `brightness --detectmonitor` and it'll overwrite "$HOME/.brightness/monitor" with your new primary monitor's name.