# Monitor Brightness

This program allows you to control the brightness of your primary monitor on a software level from the command line. It uses the `xrandr` command to adjust the brightness and stores the current brightness level in a file for future reference.

## Why?

`xrandr` allows you to set a brightness level with `xrandr --output <outputname> --brightness <value>` but doesn't allow you to simply increase or decrease it by a value. This script simply stores the brightness level so it can access it and increase/decrease it by whatever desired.

## Options

The program also supports the following options:

- `--detectmonitor` or `-d`: This option detects the primary monitor and saves its name. Use this option if you change your monitor and want to update the program with the new monitor name.
- `--help` or `-h`: Displays the program's usage information.

## Configuration

The program stores the brightness level and monitor name in a file located in the user's home directory. The default file name is `.brightness`. You can change the file name by modifying the `FILENAME` constant in the program.

## Usage

To use the program, run the following command:

```
./brightness <value>
```

Replace `<value>` with the desired brightness level. The program accepts both positive and negative values. Positive values increase the brightness, while negative values decrease it. The program caps the brightness level at 0 and 10, ensuring that it doesn't go beyond the minimum or maximum limits.

## How it works
On the first time you run it, it runs `xrandr | grep "connected primary" | cut -f1 -d " "` to get the name of your primary monitor and saves it in "$HOME/.brightness" along with the current brightness (represented as an int from 0 to 10). The program will now fetch the name of the monitor as well as the current brightness from "$HOME/.brightness" to save time in the next executions.

The script sets the brightness by running `xrandr --output <outputname> --brightness <value>`.

## Notes

- Since I have switched to Wayland, I no longer use the script and will not be updating it.

## License

This project is licensed under the [GNU General Public License](LICENSE). Feel free to modify and use it according to your needs.
