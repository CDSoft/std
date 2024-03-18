/* This file is part of std.
 *
 * std is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * std is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with std.  If not, see <https://www.gnu.org/licenses/>.
 *
 * For further information about std you can visit
 * http://cdelord.fr/std
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

static size_t nb_checks = 0;
static size_t nb_errors = 0;

#define CHECK(cond, msg) report_error(__FILE__, __LINE__, (cond), (msg))

static void report_error(const char *file, int line, int cond, const char *msg) {
    nb_checks++;
    if (!cond) {
        nb_errors++;
        fprintf(stderr, "%s:%d: %s\n", file, line, msg);
    }
}

static int test_status(void) {
    fprintf(stdout, "Tests : %zu\n", nb_checks);
    fprintf(stdout, "Errors: %zu\n", nb_errors);
    return (nb_errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
