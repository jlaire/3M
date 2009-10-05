#include <stdlib.h>
#include <stdio.h>

#include "const.h"
#include "ruleset.h"
#include "move.h"
#include "position.h"
#include "indexing.h"
#include "database.h"

#define BUF_SIZE 1024

int main(void) {
	init_adjacent();
	init_neighbours();
	init_positions();
	init_dead_pattern_table();
	init_indexing_tables();

	char buf[BUF_SIZE];

	while (fgets(buf, BUF_SIZE, stdin) != NULL) {
		position_t position;
		if (read_position(&position, buf) < 0) {
			printf("Parse error\n");
			continue;
		}

		result_t result = lookup(position);
		if (!result_ok(result)) {
			printf("lookup failed\n");
			continue;
		}

		if (show_result(result, buf, BUF_SIZE) < 0) {
			printf("wat\n");
			continue;
		}

		printf("%s\n", buf);
	}

	return EXIT_SUCCESS;
}
