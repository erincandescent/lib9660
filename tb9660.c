/* lib9660: a simple ISO9660 reader library especially suited to embedded 
 *          systems
 *
 * Copyright © 2014, Owen Shepherd
 * 
 * Permission to use, copy, modify, and/or distribute this software for any 
 * purpose with or without fee is hereby granted without restriction.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, 
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lib9660.h"

FILE *isof;

l9660_fs   fs;
l9660_dir  dir;
l9660_file file;

static bool readsect(l9660_fs *fs, void *buf, uint32_t sector)
{
	if (fseek(isof, 2048 * sector, SEEK_SET)) {
		fprintf(stderr, "Reading sector %u\n", sector);
		perror("fseek");
		return false;
	}

	if (fread(buf, 2048, 1, isof) != 1) {
		fprintf(stderr, "Reading sector %u\n", sector);
		perror("fread");
		return false;
	}

	return true;
}

static void check(l9660_status stat, const char *ctx)
{
	if (stat) {
		fprintf(stderr, "%s: error %d\n", ctx, stat);
		exit(1);
	}
}

int main(int argc, char **argv)
{
	const char *progname = "tb9660";
	if (argc >= 1) progname = argv[0];

	if (argc < 4) {
		fprintf(stderr, "Usage: %s file.iso [ls|cat] path\n", progname);
		return 1;
	}

	isof = fopen(argv[1], "rb");
	if (!isof) {
		fprintf(stderr, "Unable to open '%s'\n", argv[1]);
		return 1;
	}

	check(l9660_openfs(&fs, readsect), "openfs");
	check(l9660_fs_open_root(&dir, &fs), "fs_open_root");

	if (strcmp(argv[2], "ls") == 0) {
		if (strlen(argv[3]))
			check(l9660_opendirat(&dir, &dir, argv[3]), "opendirat");

		for (;;) {
			l9660_dirent *dent;
			check(l9660_readdir(&dir, &dent), "readdir");

			if (dent == 0)
				break;

			printf("%.*s\n", dent->name_len, dent->name);
		}
	} else if (strcmp(argv[2], "cat") == 0) {
		check(l9660_openat(&file, &dir, argv[3]), "openat");

		for(;;) {
			char buf[128];
			size_t read;
			check(l9660_read(&file, buf, 128, &read), "read");
			if (read == 0)
				break;
			fwrite(buf, read, 1, stdout);
		}
	} else {
		fprintf(stderr, "Unknown command %s\n", argv[2]);
		return 1;
	}

	return 0;
}