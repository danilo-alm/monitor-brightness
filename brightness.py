import os
import pickle
import sys
import json

# Check if user is running linux
if sys.platform not in ["linux", "linux2"]:
    print("This script was made for linux")
    sys.exit(1)

# Check if user has xrandr installed
if "xrandr" not in os.listdir("/usr/bin/"):
    print("You must have xrandr installed in order to use this script")
    sys.exit(1)

# Check for bad usage
if len(sys.argv) != 2:
    print("Usage example: brightness.py +0.5")
    sys.exit(1)
elif not sys.argv[1].replace(".", "", 1).replace("-", "", 1) \
                    .replace("+", "", 1).isdigit():
    print("You must provide a valid value")
    sys.exit(1)

# Name of file where we'll store the current brightness
data_file = "brightness.data"

# Check if data file already exists
file_exists = os.path.exists(data_file)
file_size = (os.path.getsize(data_file) if file_exists else 0)

# If file doesn't exist or it's empty, write 1 to it (1 is normal brightness)
if file_size == 0:
    with open(data_file, "wb") as f:
        pickle.dump(1, f)

# Check if value inside file is not too big or too small
with open(data_file, "rb") as fr:
    value = pickle.load(fr)
    if not (0 <= value <= 1):
        with open(data_file, "wb") as fw:
            pickle.dump(1, fw)

# Updated brightness value
updated_value = round(value + float(sys.argv[1]), 2)

# Cap result value if it's too big or too small
if not (0 <= updated_value <= 1):
    if updated_value > 1:
        updated_value = 1
    else:
        updated_value = 0

# Get monitor name
with open("monitor.json", "r") as f:
    monitor_name = json.load(f)["monitor"]

# Update brightness
command = f"xrandr --output {monitor_name} --brightness {updated_value}"
exit_status = os.system(command)

if os.WIFEXITED(exit_status):
    if os.WEXITSTATUS(exit_status) != 0:
        print("An error ocurred")
        exit(1)

print(f"Brightness set to {updated_value}")

# Save new brightness
with open(data_file, "wb") as f:
    pickle.dump(updated_value, f)
