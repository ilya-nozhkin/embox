/**
 * @file
 * @brief Test scanf function.
 *
 * @date 22.04.17
 * @author Victor Y. Fadeev
 */

#include <stdio.h>

int main(int argc, char **argv) {
	char line_1[256];
    char line_2[256];
    
    printf("---Enter your 2 lines---\n");
    scanf("%s %s", line_1, line_2);
    printf("Your lines - %s, %s" ,line_1, line_2);

	return 0;
}
