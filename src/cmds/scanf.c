/**
 * @file
 * @brief Test scanf function.
 *
 * @date 22.04.17
 * @author Victor Y. Fadeev
 */

#include <stdio.h>

int main(int argc, char **argv) {
	char string[256];

    printf("---Enter your string---\n");
    scanf("%s", string);
    printf("Your sting - %s" ,string);

	return 0;
}
