#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

#define BUFFER_SIZE 65536

#define LSB 1
#define MSB 2

static uintmax_t group_size;
static uintmax_t groups_per_line;
static uintmax_t offset;
static uintmax_t count;
static uintmax_t counter;
static int numbering;

static int
print_bytes(char* buffer, ssize_t n_bytes)
{
	ssize_t i;
	int bit;
	char mask;

	for (i = 0; i < n_bytes * CHAR_BIT; i++, counter++) {
		if (counter < offset)
			continue;

		if (counter - offset == count)
			return 1;

		if (numbering == LSB)
			mask = (char)(1 << (i % CHAR_BIT));
		else
			mask = (char)(1 << (CHAR_BIT - (i % CHAR_BIT) - 1));

		bit = !!(buffer[i/8] & mask);
		write(STDOUT_FILENO, bit ? "1" : "0", 1);

		if (((counter+1-offset) % (group_size * groups_per_line)) == 0)
			write(STDOUT_FILENO, "\n", 1);
		else if ((counter+1-offset) % group_size == 0)
			write(STDOUT_FILENO, " ", 1);
	}

	return 0;
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
	printf("bd [-c N] [-g N] [-n N] [-o N] [-m|-l] [file]\n"
	       "  -c N    number of bits\n"
	       "  -g N    group size\n"
	       "  -n N    groups per line\n"
	       "  -o N    offset from the head\n"
	       "  -l      least significant bit first\n"
	       "  -m      most significant bit first\n");
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
	offset = 0;
	count = UINT_MAX;
	numbering = LSB;

	counter = 0;

	while ((option = getopt(argc, argv, "c:g:hlmn:o:")) != -1) {
		switch(option) {
			case 'g': group_size = read_number(); break;
			case 'n': groups_per_line = read_number(); break;

			case 'c': count = read_number(); break;
			case 'o': offset = read_number(); break;

			case 'l': numbering = LSB; break;
			case 'm': numbering = MSB; break;

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

		if (print_bytes(buffer, n_bytes) == 1)
			break;
	}

	if (((counter+1-offset) % (group_size * groups_per_line)) != 0)
		write(STDOUT_FILENO, "\n", 1);

	if (fd != STDIN_FILENO)
		close(fd);

	return EXIT_SUCCESS;
}

