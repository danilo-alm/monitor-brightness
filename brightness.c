#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <getopt.h>

// Get user home path
#define HOMEPATH (getpwuid(getuid()) -> pw_dir)

// Folder where files will be kept
#define FOLDERNAME ".brightness"

// File in FOLDERNAME where current brightness level will be stored
#define BRIGHTNESSFILE "brightness"

// File in FOLDERNAME where monitor name level will be stored
#define MONITORFILE "monitor"

// Max length of monitorName
#define MONITORNAMELENGTH 20

// currentBrightness (0 to 10)
int currentBrightness = 10;

// Function prototypes
void cap_current_brightness();
char *get_monitor_name(char *monitorFilePath, bool forceOverwrite);
void print_usage();

int main(int argc, char *argv[])
{
    // Check for bad usage
    if (argc != 2)
    {
        print_usage();
        exit(1);
    }

    // Path to FOLDERNAME
    char folderPath[strlen(HOMEPATH) + strlen(FOLDERNAME) + 3];
    strcpy(folderPath, HOMEPATH);
    strcat(strcat(strcat(folderPath, "/"), FOLDERNAME), "/");

    // Path to MONITORFILE
    char monitorFilePath[strlen(folderPath) + strlen(MONITORFILE) + 1];
    strcpy(monitorFilePath, folderPath);
    strcat(monitorFilePath, MONITORFILE);

    char userInput[strlen(argv[1]) + 1];
    strcpy(userInput, argv[1]);

    // Check if userInput is valid
    bool isValid = true;
    int counter = 0;
    for (int i = 0; userInput[i] != '\0'; i++)
    {
        if (isdigit(userInput[i]) == 0 && userInput[i] != '-'
        && userInput[i] != '+')
        {
            isValid = false;
            break;
        }
        else if (isdigit(userInput[i]))
        {
            counter++;
        }
    }
    // Check if value is actually an option
    if (!isValid)
    {
        int option_index = 0;
        int opt = 0;
        static struct option long_options[] = {
            {"detectmonitor", no_argument, 0, 'd'},
            {"help",          no_argument, 0, 'h'},
            {0,               0,           0,   0}
        };
        while ((opt = getopt_long(argc, argv, "dh", long_options, &option_index)) != -1)
        {
            switch (opt)
            {
                case 'd':
                    printf("Detecting monitor...\n");
                    char *monitorName = get_monitor_name(monitorFilePath, true);
                    printf("Monitor: %s. Saved to %s\n", monitorName, monitorFilePath);
                    free(monitorName);
                    exit(0);
                case 'h':
                    print_usage();
                    printf("Run it with '--detectmonitor' to re-detect monitor\n");
                    break;
                    exit(0);
            }
        }
        // Not an option. Just an invalid integer
		print_usage();
        exit(2);
    }
    if (counter > 2)
    {
        printf("Value has too many digits\n");
        print_usage();
        exit(3);
    }

    // Create FOLDERNAME in home directory if it doesn't exist yet
    struct stat st = {0};
    if (stat(folderPath, &st) == -1)
    {
        int status = mkdir(folderPath, 0700);
        if (status == -1)
        {
            printf("Error: could not create directory \"%s\" in \"%s\"\n",
            FOLDERNAME, HOMEPATH);
            printf("Make sure there is not a file with the same name in %s\n", HOMEPATH);
            exit(4);
        }
    }

    // Get monitor name
    char *monitorName = get_monitor_name(monitorFilePath, false);

    // Path to BRIGHTNESSFILE
    char brightnessFilePath[strlen(folderPath) + strlen(BRIGHTNESSFILE) + 1];
    strcpy(brightnessFilePath, folderPath);
    strcat(brightnessFilePath, BRIGHTNESSFILE);

    // If BRIGHTNESSFILE exists, read value in it
    if (access(brightnessFilePath, F_OK) == 0)
    {
        FILE *f = fopen(brightnessFilePath, "r");
        if (f == NULL)
        {
            printf("Error: couldn't open file %s\n", brightnessFilePath);
            exit(5);
        }
        fread(&currentBrightness, sizeof(int), 1, f);
        fclose(f);
    }

    // Add user value to currentBrightness
    int userValue = strtol(userInput, (char **)NULL, 10);
    currentBrightness = currentBrightness + userValue;

    cap_current_brightness();

    // Change the brightness
    char command[80];
    float currentBrightnessFloat = currentBrightness / 10.0;
    snprintf(command, sizeof(command), "xrandr --output %s "
    "--brightness %.2f", monitorName, currentBrightnessFloat);
    free(monitorName);

    int exit_status = system(command);
    if (exit_status != 0)
    {
        printf("Error: error running command \"%s\"\n", command);
        exit(6);
    }
    printf("Current brightness is %i\n", currentBrightness);

    // Write new currentBrightness to file
    FILE *f = fopen(brightnessFilePath, "w");
    if (f == NULL)
    {
        printf("Error: couldn't open file %s\n", brightnessFilePath);
        exit(5);
    }
    fwrite(&currentBrightness, sizeof(int), 1, f);
    fclose(f);

    return 0;
}

void print_usage()
{
    printf("Usage: brightness < value > | brightness < -value >\n"
    "Run 'brightness --detectmonitor' to re-detect primary monitor\n");
    exit(8);
}

// Caps out currentBrightness at 0 or 10 if needed
void cap_current_brightness()
{
    if (currentBrightness > 10)
    {
        currentBrightness = 10;
    }
    else if (currentBrightness < 0)
    {
        currentBrightness = 0;
    }
    return;
}

// Returns primary monitor's name
char *get_monitor_name(char *monitorFilePath, bool forceOverwrite)
{
    char *monitorName = malloc((sizeof(char) * MONITORNAMELENGTH) + 1);
    FILE *f;
    // If monitorFilePath already exists, get monitor name from there
    if ((!forceOverwrite) && access(monitorFilePath, F_OK) == 0)
    {
        f = fopen(monitorFilePath, "r");
        if (f != NULL)
        {
            fgets(monitorName, MONITORNAMELENGTH, f);
            fclose(f);
            return monitorName;
        }
        else
        {
            printf("Warning: could not read monitor name from \"%s\"\n", monitorFilePath);
        }
    }

    // Command that will output the primary monitor's name
    char command[] = "xrandr | grep \"connected primary\" | cut -f1 -d \" \"";

    // Store command's output
    f = popen(command, "r");
    if (f == NULL)
    {
        printf("Error: error running command \"%s\"\n", command);
        exit(7);
    }
    while (fgets(monitorName, MONITORNAMELENGTH + 1, f));
    pclose(f);

    // Remove newline from monitorName
    monitorName[strcspn(monitorName, "\n")] = '\0';

    // Write monitor name to file
    f = fopen(monitorFilePath, "w");
    if (f != NULL)
    {
        fwrite(monitorName, sizeof(char), sizeof(monitorName), f);
        fclose(f);
    }
    else
    {
        printf("Warning: could not write monitor name \"%s\" to file \"%s\"\n", monitorName, monitorFilePath);
    }
    return monitorName;
}
