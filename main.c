#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <curl/curl.h>

#define PROGRAM_NAME "notifymeplz"
#define AUTHORS "Leonardo de Requeséns"
#define VERSION 0.1

#define BUFF_BYTES 1000
#define MAX_TELEGRAM_CHARACTERS 4096 // https://limits.tginfo.me/en


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

int concat_array(char* dest, char* src, int start, int chars_amount) {
	int position = 0;
	int special_chars = 0;
	for (int i = 0; i < chars_amount; ++i) {
		position = start + i;
		switch (src[i]){
			case '\n':
			dest[position] = '%';
			dest[position + 1] = '0';
			dest[position + 2] = 'A';
			i = i + 2;
			++special_chars;
			break;

			case ' ':
			dest[position] = '%';
			dest[position + 1] = '2';
			dest[position + 2] = '0';
			i = i + 2;
			++special_chars;
			break;

			default:
			dest[position] = src[i];
		}
	}
	return special_chars;
}

void clear(CURL* curl, char* buffer) {
	curl_easy_cleanup(curl);
	free(buffer);
}


int main(int argc, char** argv) {
	int optc;
	bool print_stdout = false;
	char buffer[BUFF_BYTES];
	char* message = (char*)calloc(BUFF_BYTES, sizeof(char));
	int lenght = 0;
	int max_lenght = BUFF_BYTES;
	char* token;
	char* chat_id;
	char* url;
	
	CURL* curl = curl_easy_init();
	if (curl == NULL) {
		fputs("Can not initialize curl\n", stderr);
		free(message);
		return EXIT_FAILURE;
	}

	if (message == NULL) {
		fputs("Cannot allocate memory for the message\n", stderr);
		clear(curl, message);
		exit(EXIT_FAILURE);
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

	token = getenv("TELEGRAM_BOT_TOKEN");
	chat_id = getenv("TELEGRAM_CHAT_ID");

	if (print_stdout) {
		printf("[TEST] Telegram Token: %s\n", token);
		printf("[TEST] Telegram chat id: %s\n", chat_id);
	}

	struct stat st;
	if (fstat(fileno(stdin), &st) != 0) {
		// To be honest I don't know if this will ever trigger
		fputs("stdin not in good state, what?\n", stderr);
		curl_easy_cleanup(curl);
		exit(EXIT_FAILURE);
	}

	
	char* line;
	char* temporal_realloc;
	int line_lenght = 0;
	while ((line = fgets(buffer, BUFF_BYTES, stdin))) {
		line_lenght = strlen(line);

		if (line_lenght + lenght >= max_lenght) {
			if (line_lenght + lenght > MAX_TELEGRAM_CHARACTERS){
				fputs("Stderr has too many characters, I will send a partial message\n", stderr);
				// Todo: Send partial message
				clear(curl, message);
				return EXIT_FAILURE;
			}

			max_lenght *= 2;
			temporal_realloc = realloc(message, max_lenght);
			if (temporal_realloc == NULL) {
				fputs("Reallocation of message not working when reading stdin\n", stderr);
				// Here we could send a partial message
				clear(curl, message);
				exit(EXIT_FAILURE);
			} else {
				free(message);
				message = temporal_realloc;
			}

		}

		lenght += line_lenght + 2 * concat_array(message, line, lenght, line_lenght);
	}

	while ((line = fgets(buffer, BUFF_BYTES, stderr))) {
		printf("Inside stderr\n");
		line_lenght = strlen(line);

		if (line_lenght + lenght >= max_lenght ) {
			if (line_lenght + lenght > MAX_TELEGRAM_CHARACTERS){
				fputs("Stderr has too many characters, I will send a partial message\n", stderr);
				// Todo: send partial message
				clear(curl, message);
				return EXIT_FAILURE;
			}
			max_lenght *= 2;
			temporal_realloc = realloc(message, max_lenght);
			if (temporal_realloc == NULL) {
				fputs("Reallocation of message not working when reading stderr\n", stderr);
				// Here we could send a partial message
				clear(curl, message);
				exit(EXIT_FAILURE);
			} else {
				free(message);
				message = temporal_realloc;
			}
		}

		lenght += line_lenght + 2 * concat_array(message, line, lenght, line_lenght);
	}

	url = (char*)malloc(sizeof(char) * 90 + lenght); // 90 chars is a little bit more for the base url

	sprintf(url, "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s", token, chat_id, message);
	if (print_stdout) {
		printf("[TEST] Full link: %s\n", url);
	} else {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		if (curl_easy_perform(curl) != 0) {
			fputs("Failing on sending the message\n", stderr);
			clear(curl,message);
			free(url);
			return EXIT_FAILURE;
		}
	}

	free(url);
	clear(curl, message);
	return EXIT_SUCCESS;
}
