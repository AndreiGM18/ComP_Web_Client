// SPDX-License-Identifier: EUPL-1.2
/* Copyright Mitran Andrei-Gabriel 2023 */

#include <stdbool.h>    /* bool */
#include <ctype.h>      /* isdigit */
#include "utils.h"

bool is_number(char *s) {
	for (int i = 0; s[i] != '\0'; ++i) {
		if (!isdigit(s[i])) {
			return false;
		}
	}

	return true;
}
