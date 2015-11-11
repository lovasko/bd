#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define BUFFER_SIZE 65536

static uintmax_t group_size;
static uintmax_t groups_per_line;
static uintmax_t skip;
static uintmax_t counter;

static void
print_bytes(char* buffer, ssize_t n_bytes)
{
	ssize_t i;
	int bit;

	for (i = 0; i < n_bytes * CHAR_BIT; i++, counter++) {
		if (counter < skip)
			continue;

		bit = !!(buffer[i/8] & (1 << (i % CHAR_BIT)));
		write(STDOUT_FILENO, bit ? "1" : "0", 1);

		if (((counter+1-skip) % (group_size * groups_per_line)) == 0)
			write(STDOUT_FILENO, "\n", 1);
		else if ((counter+1-skip) % group_size == 0)
			write(STDOUT_FILENO, " ", 1);
	}
}

static uintmax_t 
read_number(void)
{
	uintmax_t number;

	errno = 0;
	number = strtoumax(optarg, NULL, 10);
	if ((number == 0 || number == UINTMAX_MAX) && errno != 0) {
		perror("strtoumax");
		exit(EXIT_FAILURE);
	}

	return number;
}

static void
print_usage(void)
{
	printf("bd [-g N] [-l N] [-s N] [file]\n"
	       "  -g N    group size\n"
	       "  -n N    groups per line\n"
	       "  -s N    skip bits from the head\n");
}

int
main(int argc, char** argv)
{
	char buffer[BUFFER_SIZE];
	int fd;
	int option;
	ssize_t n_bytes;
	
	group_size = CHAR_BIT;
	groups_per_line = 8;
	counter = 0;

	while ((option = getopt(argc, argv, "g:hn:s:")) != -1) {
		switch(option) {
			case 'g': group_size = read_number(); break;
			case 'n': groups_per_line = read_number(); break;
			case 's': skip = read_number(); break;

			case 'h':
			case '?':
				print_usage();
			return EXIT_FAILURE;

			default: 
				fprintf(stderr, "ERROR: Unknown error while parsing arguments\n");
			return EXIT_FAILURE;
		}
	}

	if (argc - optind < 1)
		fd = STDIN_FILENO;
	else {
		errno = 0;
		fd = open(argv[optind], O_RDONLY);
		if (fd < 0) {
			perror("open");
			return EXIT_FAILURE;
		}
	}

	while (1) {
		errno = 0;
		n_bytes = read(fd, buffer, BUFFER_SIZE);

		if (n_bytes == 0)
			break;

		if (n_bytes == -1) {
			perror("read");
			return EXIT_FAILURE;
		}

		print_bytes(buffer, n_bytes);
	}

	if (fd != STDIN_FILENO)
		close(fd);

	return EXIT_SUCCESS;
}

