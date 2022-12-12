# Monitor Brightness

- Requires `xrandr`

A script that uses `xrandr` to control the brightness level of your primary monitor on a software level, allowing you to lower it past the minimum hardware setting. <br/>

## Usage
You use the program by running it along with an integer (From 0 to 10. Positive or negative) to add to the current brightness value. For example:
```
# Increase brightness by 1
./brightness 1

# Decrease brightness by 2
./brightness -2
```

## How it works
On the first time you run it, it runs `xrandr | grep "connected primary" | cut -f1 -d " "` to get the name of your primary monitor and saves it in "$HOME/.brightness" along with the current brightness (represented as an int from 0 to 10) <br/>
The program will now fetch the name of the monitor as well as the current brightness from "$HOME/.brightness" to save time in the next executions.<br/><br/>

If you've changed your monitor and need to detect your new one, run `./brightness --detectmonitor` or `./brightness -d` and it'll overwrite "$HOME/.brightness" with your new primary monitor's name.
