#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

// Name of your monitor
#define MONITORNAME "HDMI-A-0"

// Get user home path
#define HOMEPATH (getpwuid(getuid()) -> pw_dir)

// File in home directory where we will store the current brightness level
#define BRIGHTNESSFILE ".brightness"

// Number of decimal places tolerated in the float that the user will provide as argument
#define NUMOFDECIMALS 4

float currentBrightness = 1;

// Function prototypes
void cap_current_brightness(void);

int main(int argc, char *argv[])
{
    // Append BRIGHTNESSFILE to user home path
    char brightnessFilePath[strlen(BRIGHTNESSFILE) + strlen(HOMEPATH) + 2];
    strcpy(brightnessFilePath, HOMEPATH);
    strcat(strcat(brightnessFilePath, "/"), BRIGHTNESSFILE);

    // Check for bad usage
    if (argc != 2)
    {
        printf("Usage example: ./brightness -.1\n");
        return 1;
    }

    char userInput[strlen(argv[1]) + 1];
    strcpy(userInput, argv[1]);

    // Check if argv[1] a is valid number
    bool isValid = true;
    for (int i = 0; userInput[i] != '\0'; i++)
    {
        if (isdigit(userInput[i]) == 0 && userInput[i] != '.'
        && userInput[i] != '-' && userInput[i] != '+')
        {
            isValid = false;
            break;
        }
    }
    if (!isValid)
    {
        printf("Error: value is not valid\n");
        return 2;
    }

    // Count number of digits after decimal point
    int counter = 0;
    int decimalPointIndex = strcspn(userInput, ".");
    for (int i = decimalPointIndex + 1; userInput[i] != '\0'; i++)
    {
        counter++;
    }
    if (counter > NUMOFDECIMALS)
    {
        printf("Error: value has too many decimal places\n");
        return 3;
    }

    // Check if argv[1] has more than one digit before decimal point
    counter = 0;
    for (int i = 0; userInput[i] != '.' && userInput[i] != '\0'; i++)
    {
        if (userInput[i] != '+' && userInput[i] != '-')
        {
            counter++;
            if (counter > 1)
            {
                printf("Error: value is too big\n");
                return 4;
            }
        }
    }

    // If file exists, read value in it
    if (access(brightnessFilePath, F_OK) == 0)
    {
        FILE *f = fopen(brightnessFilePath, "r");
        if (f == NULL)
        {
            printf("Error: couldn't open file %s\n", brightnessFilePath);
            return 5;
        }
        fread(&currentBrightness, sizeof(float), 1, f);
        fclose(f);
    }
    else
    {
        // File will be created later on
        currentBrightness = 1;
    }

    cap_current_brightness();

    // Add user value to currentBrightness
    float userValue = strtod(userInput, NULL);
    currentBrightness += userValue;

    cap_current_brightness();

    // Round value to two decimal places
    currentBrightness = ( (float) ((int) (currentBrightness * 100) ) / 100);

    // Change the brightness
    char command[80];
    snprintf(command, sizeof(command), "xrandr --output %s --brightness %.2f", MONITORNAME, currentBrightness);
    int exit_status = system(command);
    if (exit_status != 0)
    {
        printf("Error: error running command \"%s\"\n", command);
        return 6;
    }
    printf("Current brightness is %.2f\n", currentBrightness);

    // Write new currentBrightness to file
    FILE *f = fopen(brightnessFilePath, "w");
    if (f == NULL)
    {
        printf("Error: couldn't open file %s\n", brightnessFilePath);
        return 5;
    }
    fwrite(&currentBrightness, sizeof(currentBrightness), 1, f);
    fclose(f);

    return 0;
}

// Caps out currentBrightness at 0 or 1 if needed
void cap_current_brightness(void)
{
    if (!(currentBrightness >= 0 && currentBrightness <= 1))
    {
        if (currentBrightness > 1)
        {
            currentBrightness = 1.0;
        }
        else
        {
            currentBrightness = 0.0;
        }
    }

    return;
}