#include <stdio.h>
#include <string.h>

#include "common.h"
#include "view.h"
#include "edit.h"

int main(int argc, char *argv[])
{
    OperationType operation;

    if (argc < 2)
    {
        printf("\nERROR: Please pass the command line arguments\n");
        printf("Use --help for more information\n");
        return failure;
    }

    /* Identify operation */

    if (strcmp(argv[1], "-v") == 0)
    {
        operation = view_mp3;
    }
    else if (strcmp(argv[1], "-e") == 0)
    {
        operation = edit_mp3;
    }
    else if (strcmp(argv[1], "--help") == 0)
    {
        operation = helpmenu;
    }
    else
    {
        operation = Invalid_operator;
    }

    /* Perform operation */

    switch (operation)
    {
        case view_mp3:

            if (read_and_validation_for_view(argv) == success)
            {
                to_view(argv[2]);
            }

            break;


        case edit_mp3:

            if (read_and_validation_for_edit(argv) == success)
            {
                to_edit(argv[4], argv[2], argv[3]);
            }

            break;


        case helpmenu:

            printf("\n==================================================\n");
            printf("              MP3 TAG READER AND EDITOR\n");
            printf("==================================================\n");

            printf("\nVIEW OPERATION\n");
            printf("./a.out -v <file.mp3>\n");

            printf("\nEDIT OPERATION\n");
            printf("./a.out -e <option> <value> <file.mp3>\n");

            printf("\nEDIT OPTIONS\n");
            printf("-t  : Edit Title\n");
            printf("-a  : Edit Artist\n");
            printf("-A  : Edit Album\n");
            printf("-y  : Edit Year\n");
            printf("-g  : Edit Genre\n");
            printf("-c  : Edit Comment\n");

            printf("\nHELP\n");
            printf("./a.out --help\n");

            printf("\n==================================================\n");

            break;


        case Invalid_operator:

            printf("\nERROR: Invalid option\n");
            printf("Use --help for more information\n");

            break;
    }

    return success;
}