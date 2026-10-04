#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "edit.h"


Status read_and_validation_for_edit(char *argv[])
{
    if (argv[2] == NULL)
    {
        printf("\nERROR: Edit option is missing\n");
        return failure;
    }

    if (argv[3] == NULL)
    {
        printf("\nERROR: Tag value is missing\n");
        return failure;
    }

    if (argv[4] == NULL)
    {
        printf("\nERROR: MP3 file name is missing\n");
        return failure;
    }

    if (strstr(argv[4], ".mp3") == NULL)
    {
        printf("\nERROR: File is not an MP3 file\n");
        return failure;
    }

    return success;
}


Status to_edit(char *filename, char *option, char *value)
{
    FILE *src;
    FILE *temp;

    unsigned char header[10];

    char tag[5];
    char frame_tag[5];

    unsigned char size_str[4];
    unsigned int size;

    unsigned char flags[2];

    char content[100];
    char new_content[100];

    unsigned int new_size;

    unsigned char new_size_str[4];

    unsigned char buffer[1024];

    size_t bytes;


    /* Convert command-line option to frame ID */

    if (strcmp(option, "-t") == 0)
    {
        strcpy(frame_tag, "TIT2");
    }
    else if (strcmp(option, "-a") == 0)
    {
        strcpy(frame_tag, "TPE1");
    }
    else if (strcmp(option, "-A") == 0)
    {
        strcpy(frame_tag, "TALB");
    }
    else if (strcmp(option, "-y") == 0)
    {
        strcpy(frame_tag, "TYER");
    }
    else if (strcmp(option, "-g") == 0)
    {
        strcpy(frame_tag, "TCON");
    }
    else if (strcmp(option, "-c") == 0)
    {
        strcpy(frame_tag, "COMM");
    }
    else
    {
        printf("\nERROR: Invalid edit option\n");
        return failure;
    }


    /* Open original MP3 */

    src = fopen(filename, "rb");

    if (src == NULL)
    {
        printf("\nERROR: Unable to open file %s\n", filename);
        return failure;
    }


    /* Create temporary file */

    temp = fopen("temp.mp3", "wb");

    if (temp == NULL)
    {
        printf("\nERROR: Unable to create temporary file\n");

        fclose(src);

        return failure;
    }


    /* Read and copy ID3 header */

    fread(header, 1, 10, src);

    fwrite(header, 1, 10, temp);


    /*
     * Read all frames
     */

    while (1)
    {
        /* Read frame ID */

        if (fread(tag, 1, 4, src) != 4)
        {
            break;
        }

        tag[4] = '\0';


        /* Check for empty frame */

        if (tag[0] == '\0')
        {
            break;
        }


        /* Read frame size */

        if (fread(size_str, 1, 4, src) != 4)
        {
            break;
        }


        /* Convert big-endian size to integer */

        size = ((unsigned int)size_str[0] << 24) |
               ((unsigned int)size_str[1] << 16) |
               ((unsigned int)size_str[2] << 8) |
               size_str[3];


        /* Read flags */

        if (fread(flags, 1, 2, src) != 2)
        {
            break;
        }


        /*
         * If frame is too large for our buffer,
         * copy it without editing.
         */

        if (size >= sizeof(content))
        {
            fwrite(tag, 1, 4, temp);
            fwrite(size_str, 1, 4, temp);
            fwrite(flags, 1, 2, temp);

            while (size > 0)
            {
                unsigned int chunk;

                if (size > sizeof(buffer))
                {
                    chunk = sizeof(buffer);
                }
                else
                {
                    chunk = size;
                }

                bytes = fread(buffer, 1, chunk, src);

                if (bytes == 0)
                {
                    break;
                }

                fwrite(buffer, 1, bytes, temp);

                size = size - bytes;
            }

            continue;
        }


        /* Read frame content */

        if (fread(content, 1, size, src) != size)
        {
            break;
        }


        /*
         * Check whether this is the frame
         * that needs to be edited.
         */

        if (strcmp(tag, frame_tag) == 0)
        {
            /*
             * First byte is the encoding byte.
             * Keep it unchanged.
             */

            new_content[0] = content[0];

            strcpy(new_content + 1, value);

            new_size = strlen(value) + 1;


            /* Write frame ID */

            fwrite(tag, 1, 4, temp);


            /*
             * Convert new size to big-endian
             */

            new_size_str[0] = (new_size >> 24) & 0xFF;
            new_size_str[1] = (new_size >> 16) & 0xFF;
            new_size_str[2] = (new_size >> 8) & 0xFF;
            new_size_str[3] = new_size & 0xFF;


            /* Write new frame size */

            fwrite(new_size_str, 1, 4, temp);


            /* Write original flags */

            fwrite(flags, 1, 2, temp);


            /* Write new content */

            fwrite(new_content, 1, new_size, temp);


            printf("\n==================================================\n");
            printf("              MP3 TAG EDITOR\n");
            printf("==================================================\n");

            printf("Frame       : %s\n", tag);
            printf("Old value   : %s\n", content + 1);
            printf("New value   : %s\n", value);

            printf("--------------------------------------------------\n");
            printf("Tag edited successfully!\n");

            printf("==================================================\n");
        }
        else
        {
            /*
             * Copy unchanged frame
             */

            fwrite(tag, 1, 4, temp);
            fwrite(size_str, 1, 4, temp);
            fwrite(flags, 1, 2, temp);
            fwrite(content, 1, size, temp);
        }
    }


    /*
     * Copy remaining MP3 audio data
     */

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0)
    {
        fwrite(buffer, 1, bytes, temp);
    }


    fclose(src);
    fclose(temp);


    /*
     * Replace original file
     */

    remove(filename);

    if (rename("temp.mp3", filename) != 0)
    {
        printf("\nERROR: Unable to replace original MP3 file\n");
        return failure;
    }


    return success;
}