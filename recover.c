#include <stdio.h>
#include <stdint.h>

#define BLOCK_SIZE 512

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check command-line argument
    if (argc != 2)
    {
        printf("Usage: ./recover IMAGE\n");
        return 1;
    }

    // Open forensic image
    FILE *forensic_file = fopen(argv[1], "r");
    if (forensic_file == NULL)
    {
        printf("Could not open forensic image.\n");
        return 1;
    }

    // Variables for file handling
    FILE *jpeg_file = NULL;
    BYTE buffer[BLOCK_SIZE];
    int jpeg_count = 0;
    char filename[8];

    // Read forensic image in 512-byte blocks
    while (fread(buffer, sizeof(BYTE), BLOCK_SIZE, forensic_file) == BLOCK_SIZE)
    {
        // Check for JPEG signature
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // Close previous JPEG file if one is already open
            if (jpeg_file != NULL)
            {
                fclose(jpeg_file);
            }

            // Create new JPEG file
            sprintf(filename, "%03i.jpg", jpeg_count);
            jpeg_file = fopen(filename, "w");
            if (jpeg_file == NULL)
            {
                printf("Could not create JPEG file.\n");
                fclose(forensic_file);
                return 1;
            }

            // Increase JPEG count for the next file
            jpeg_count++;
        }

        // Write to JPEG file if one is open
        if (jpeg_file != NULL)
        {
            fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, jpeg_file);
        }
    }

    // Close any open files
    if (jpeg_file != NULL)
    {
        fclose(jpeg_file);
    }
    fclose(forensic_file);

    return 0;
}
