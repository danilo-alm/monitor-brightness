#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <getopt.h>
#include <errno.h>

// Get user home path
#define HOMEPATH (getpwuid(getuid())->pw_dir)

// Max length of monitorName
#define MONITORNAMELENGTH 20

// File in home directory where brightness and monitor name will be stored
#define FILENAME ".brightness"

// Function prototypes
bool validate_input(char *input, int *inplen);
int get_options(int argc, char **argv);
int get_info(char *monitorName, int *cbrightness, char *path);
bool save_info(char *monitorName, int *cbrightness, char *path, bool getMonitor);
void cap_brightness(int *cbrightness);
void print_usage();

int main(int argc, char **argv)
{
    // Check for bad usage
    if (argc != 2)
    {
        print_usage();
        exit(1);
    }

    // Path to FILENAME
    char path[strlen(HOMEPATH) + strlen(FILENAME) + 1];
    path[0] = '\0';
    strcat(strcat(strcat(path, HOMEPATH), "/"), FILENAME);

    char monitorName[MONITORNAMELENGTH + 1];
    int cbrightness = 10;
    int extatus = 0;

    if ((extatus = get_info(monitorName, &cbrightness, path)) == 1)
    {
        // `get_info()` returned that `path` doesn't exist
        if (!save_info(monitorName, &cbrightness, path, true))
        {
            printf("Could not create file '%s'\n", path);
            exit(1);
        }
    }
    else if (extatus == 2)
    {
        printf("Could not open '%s'\n", path);
        exit(1);
    }

    // Validate argv[1]
    int inplen = 0;
    if (!validate_input(argv[1], &inplen))
    {
        // If input is not a valid digit, check if it's an option
        switch (get_options(argc, argv))
        {
        case 'h':
            print_usage();
            exit(0);
        case 'd':
            if (save_info(monitorName, &cbrightness, path, true))
            {
                printf("Saved to '%s'\n", path);
            }
            exit(0);
        }

        // If argv[1] is not an option, it's just an invalid input
        printf("Invalid value\n");
        print_usage();
        exit(1);
    }
    else if (inplen > 3)
    {
        printf("Value length is too big\n");
        print_usage();
        exit(1);
    }

    // Convert user input to integer
    int inpvalue = strtol(argv[1], (char **)NULL, 10);
    if (errno == ERANGE)
    {
        printf("Could not convert value to integer\n");
        exit(1);
    }

    // Add user value to current brightness
    cbrightness += inpvalue;
    cap_brightness(&cbrightness);

    // Change the brightness
    char command[80];
    float cbrightnessFloat = cbrightness / 10.0;
    snprintf(command, sizeof(command), "xrandr --output %s --brightness %.1f",
             monitorName, cbrightnessFloat);

    int exit_status = system(command);
    if (exit_status != 0)
    {
        printf("Error: error running command \"%s\"\n", command);
        exit(6);
    }
    printf("Current brightness is %f\n", cbrightnessFloat);

    // Save changes to file
    if (!save_info(monitorName, &cbrightness, path, false))
    {
        printf("Could not write to '%s'\n", path);
        exit(1);
    }

    return 0;
}

/* Validates input and puts its length in *INPLEN if INPLEN is not NULL. */
bool validate_input(char *input, int *inplen)
{
    int len = 0;
    bool isValid = true;

    // Check if it's all digits
    char *c;
    for (; *(c = &input[len]) != '\0'; len++)
    {
        if (isdigit(*c) == 0 && *c != '-' && *c != '+')
        {
            isValid = false;
            // Count rest of characters and break
            for (; *c != '\0'; c = &input[len++]);
            break;
        }
    }
    if (inplen != NULL) *inplen = len;
    return isValid;
}

// Returns option in argv if there is one
int get_options(int argc, char **argv)
{
    int optionIndex = 0;
    int opt = 0;
    static struct option long_options[] = {
        {"detectmonitor", no_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}};
    while ((opt = getopt_long(argc, argv, "dh", long_options, &optionIndex)) != -1)
    {
        switch (opt)
        {
        case 'd':
            return opt;
        case 'h':
            return opt;
        }
    }
    return opt;
}

/* Puts monitor name and current brightness in `monitorName` and `cbrightness`.
Returns 1 if path doesn't exist and 2 if could not open path */
int get_info(char *monitorName, int *cbrightness, char *path)
{
    // Check if path already exists
    if (!(access(path, F_OK) == 0))
        return 1;

    // Read values in it
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return 2;

    // Brightness value comes first in the value
    fread(cbrightness, sizeof(int), 1, f);

    // Index in which to add next character
    int index = 0;
    char buffer;

    // Read monitor name
    while (fread(&buffer, sizeof(char), 1, f))
    {
        monitorName[index] = buffer;
        index++;
    }
    fclose(f);
    return 0;
}

/* Saves `monitorName` and `cbrightness` to `path`. If getMonitor is true, gets monitor
name running `command` name and saves it to `monitorName` before saving it to file.
Returns true if information was saved successfully */
bool save_info(char *monitorName, int *cbrightness, char *path, bool getMonitor)
{
    int index = 0;
    if (getMonitor)
    {
        char buff;

        char command[] = "xrandr | grep \"connected primary\" | cut -f1 -d \" \"";
        FILE *p = popen(command, "r");
        if (p == NULL)
            return false;

        // Copy command's output to monitorName
        while ((buff = fgetc(p)) != EOF && index <= MONITORNAMELENGTH)
        {
            monitorName[index] = buff;
            index++;
        }
        pclose(p);

        // Remove newline from output
        monitorName[index - 1] = '\0';
        printf("Monitor name is '%s'\n", monitorName);
    }

    // Create file
    FILE *f = fopen(path, "w");
    if (f == NULL)
        return false;

    // Write brightness to file
    fwrite(cbrightness, sizeof(int), 1, f);

    // Write monitor name to file
    fwrite(monitorName, strlen(monitorName) + 1, 1, f);
    fclose(f);

    return true;
}

// Prints program usage
void print_usage()
{
    printf("Usage: brightness < value > | brightness < -value >\n"
           "Run 'brightness --detectmonitor' to re-detect primary monitor\n");
}

// Caps out currentBrightness at 0 or 10 if needed
void cap_brightness(int *cbrightness)
{
    if (*cbrightness > 10)
    {
        *cbrightness = 10;
    }
    else if (*cbrightness < 0)
    {
        *cbrightness = 0;
    }
}