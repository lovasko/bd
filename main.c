#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 65536

unsigned int group_size;
unsigned int groups_per_line;
unsigned int skip;
unsigned int counter;

char* usage_string = "bd [-g N] [-l N] [-s N] [file]\n";
char* unknown_error_string = "ERROR: unknown error during option parsing\n";
char* unknown_opt_string = "ERROR: unknown option ";

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

		if (((counter+1) % (group_size * groups_per_line)) == 0)
			write(STDOUT_FILENO, "\n", 1);
		else if ((counter+1) % group_size == 0)
			write(STDOUT_FILENO, " ", 1);
	}
}

static void
print_error(int error)
{
	char* string;
	string = strerror(error);
	write(STDERR_FILENO, string, strlen(string));
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

	while ((option = getopt(argc, argv, "g:l:s:")) != -1) {
		switch(option) {
			case 'g': 
				group_size = atoi(optarg);
			break;

			case 'l': 
				groups_per_line = atoi(optarg);
			break;

			case 's': 
				skip = atoi(optarg);
			break;

			case 'h':
				write(STDOUT_FILENO, usage_string, strlen(usage_string));
			return EXIT_FAILURE;

			case '?':
				write(STDERR_FILENO, unknown_opt_string, strlen(unknown_opt_string));
				/* write(STDERR_FILENO, (char)optopt, 1); */
				write(STDERR_FILENO, "\n", 1);
			return EXIT_FAILURE;

			default: 
				write(STDERR_FILENO, unknown_error_string, strlen(unknown_error_string));
			return EXIT_FAILURE;
		}
	}

	if (argc - optind < 1)
		fd = STDIN_FILENO;
	else {
		errno = 0;
		fd = open(argv[optind], O_RDONLY);
		if (fd < 0) {
			print_error(errno);
			return EXIT_FAILURE;
		}
	}

	while (1) {
		errno = 0;
		n_bytes = read(fd, buffer, BUFFER_SIZE);

		if (n_bytes == 0)
			break;

		if (n_bytes == -1) {
			print_error(errno);
			return EXIT_FAILURE;
		}

		print_bytes(buffer, n_bytes);
	}

	return EXIT_SUCCESS;
}

