#include <stdio.h>
#include <string.h>
#include "view.h"

Status read_and_validation_for_view(char *argv[])
{
    if (argv[2] == NULL)
    {
        printf("\nERROR: MP3 file name is missing\n");
        return failure;
    }

    if (strstr(argv[2], ".mp3") == NULL)
    {
        printf("\nERROR: File is not an MP3 file\n");
        return failure;
    }

    return success;
}

Status to_view(char *filename)
{
    FILE *fptr;

    char header[4];
    char version[2];
    char tag[5];

    unsigned char size_str[4];
    unsigned int size;

    char content[100];

    fptr = fopen(filename, "rb");

    if (fptr == NULL)
    {
        printf("\nERROR: Unable to open file %s\n", filename);
        return failure;
    }

    /* Read ID3 identifier */
    fread(header, 1, 3, fptr);
    header[3] = '\0';

    if (strcmp(header, "ID3") != 0)
    {
        printf("\nERROR: ID3 tag not found\n");
        fclose(fptr);
        return failure;
    }

    /* Read ID3 version */
    fread(version, 1, 2, fptr);

    if (version[0] != 3 && version[0] != 4)
    {
        printf("\nERROR: Unsupported ID3 version\n");
        fclose(fptr);
        return failure;
    }

    /* Move to first frame */
    fseek(fptr, 10, SEEK_SET);

    printf("\n");
    printf("==================================================\n");
    printf("              MP3 TAG INFORMATION\n");
    printf("==================================================\n");

    printf("File Name    : %s\n", filename);
    printf("ID3 Version  : 2.%d\n", version[0]);

    printf("--------------------------------------------------\n");

    /* Read 6 frames */
    for (int i = 0; i < 6; i++)
    {
        /* Read frame ID */
        fread(tag, 1, 4, fptr);
        tag[4] = '\0';

        /* Read frame size */
        fread(size_str, 1, 4, fptr);

        /* Convert big-endian bytes to integer */
        size = ((unsigned int)size_str[0] << 24) |
               ((unsigned int)size_str[1] << 16) |
               ((unsigned int)size_str[2] << 8) |
               size_str[3];

        /* Skip flag bytes */
        fseek(fptr, 2, SEEK_CUR);

        /* Read frame content */
        if (size > 0 && size < sizeof(content))
        {
            fread(content, 1, size, fptr);
            content[size] = '\0';

            if (strcmp(tag, "TIT2") == 0)
                printf("Title        : %s\n", content + 1);

            else if (strcmp(tag, "TPE1") == 0)
                printf("Artist       : %s\n", content + 1);

            else if (strcmp(tag, "TALB") == 0)
                printf("Album        : %s\n", content + 1);

            else if (strcmp(tag, "TYER") == 0)
                printf("Year         : %s\n", content + 1);

            else if (strcmp(tag, "TCON") == 0)
                printf("Genre        : %s\n", content + 1);

            else if (strcmp(tag, "COMM") == 0)
                printf("Comment      : %s\n", content + 1);
        }
    }

    printf("==================================================\n");
    printf("             MP3 TAG READ SUCCESSFULLY\n");
    printf("==================================================\n\n");

    fclose(fptr);

    return success;
}