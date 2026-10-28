#include "copy.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(
            stderr,
            "Usage: %s <source> <destination>\n",
            argv[0]
        );
        return EXIT_FAILURE;
    }

    if (copy_path(argv[1], argv[2]) == -1) {
        perror("Copy failed");
        return EXIT_FAILURE;
    }

    printf("Copy completed successfully.\n");
    printf("Source      : %s\n", argv[1]);
    printf("Destination : %s\n", argv[2]);

    return EXIT_SUCCESS;
}