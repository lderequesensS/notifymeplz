#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>

#define PROGRAM_NAME "notifymeplz"
#define AUTHORS "Leonardo de Requeséns"
#define VERSION 0.1

#define BUFF_BYTES 1000

static struct option const longopts[] =
{
	{"test", no_argument, 0, 't'},
	{"help", no_argument, 0, 'h'},
	{"version", no_argument, 0, 'v'},
	{0, 0, 0, 0}
};

void usage (int status) {
	printf((
"Usage: <command> |  %s [OPTION]...\n\
"), PROGRAM_NAME );
	fputs(("\
Options:\n\
    -t, --test        write to stdout\n\
    -v, --version     output version information and exit\n\
    -h, --help        show this\n\
") , stdout);
	exit(status);
}

void version() {
	printf("%s %.1f\n\n", PROGRAM_NAME, VERSION);
	printf("Vomited by %s\n", AUTHORS);
	exit(EXIT_SUCCESS);
}

void concat_array(char* dest, char* src, int start, int chars_amount) {
	for (int i = 0; i < chars_amount; ++i) {
		dest[start + i] = src[i];
	}
}

int main(int argc, char** argv) {
	int optc;
	bool print_stdout = false;
	char buffer_stdin[BUFF_BYTES];
	char* message = (char*)calloc(BUFF_BYTES, sizeof(char));
	int lenght = 0;
	int max_lenght = BUFF_BYTES;
	
	if (message == NULL) {
		fputs("Cannot allocate memory for the message\n", stderr);
		exit(1);
	}

	while ((optc = getopt_long (argc, argv, "htv", longopts, 0)) != -1)
		switch (optc){
			case 't':
				print_stdout = true;
				break;
			case 'v':
				version();
			case 'h':
			default:
				usage(EXIT_SUCCESS);
	}

	if (print_stdout) {
		printf("[TEST] Telegram Token: %s\n", getenv("TELEGRAM_BOT_TOKEN"));
	}

	struct stat st;
	if (fstat(fileno(stdin), &st) != 0) {
		// To be honest I don't know if this will ever trigger
		fputs("stdin not in good state, what?\n", stderr);
		exit(EXIT_FAILURE);
	}

	
	char* line;
	char* temporal_realloc;
	int line_lenght = 0;
	while ((line = fgets(buffer_stdin, BUFF_BYTES, stdin))) {
		printf("Lenght of line %zu\nAnd this was read: %s\n", strlen(line), line);
		line_lenght = strlen(line);

		if (line_lenght + lenght >= max_lenght) {
			max_lenght *= 2;
			temporal_realloc = realloc(message, max_lenght);
			if (temporal_realloc == NULL) {
				fputs("Reallocation of message not working\n", stderr);
				// Here we could send a partial message
				free(message);
				exit(EXIT_FAILURE);
			} else {
				free(message);
				message = temporal_realloc;
			}

		}

		concat_array(message, line, lenght, line_lenght);
		lenght += line_lenght;
	}

	if (print_stdout) {
		printf("[TEST] Message Here:\n");
		printf("%s\n", message);
	} else {
		printf("Not implemented yet\n");
	}

	free(message);
	return EXIT_SUCCESS;
}
