// SPDX-License-Identifier: EUPL-1.2
/* Copyright Mitran Andrei-Gabriel 2023 */

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <stdbool.h>    /* bool */
#include <ctype.h>      /* isdigit */
#include <arpa/inet.h>  /* inet_addr */
#include "client.h"
#include "helpers.h"
#include "snd_recv.h"
#include "requests.h"
#include "parson.h"
#include "utils.h"

// Server host and port
char host[] = "34.254.242.81";
int port = 8080;

// Connection and authentication state
bool connected = false;
bool entered = false;

// Socket used for communication with the server
int sock = -1;

// Cookies
char *cookies[COOKIES_MAX];
int cookies_cnt = 0;

// JWT Token
char token[BUFSIZ];

// Command
char command[LINELEN];

char *setup_user() {
	char username[BUFSIZ];
	char password[BUFSIZ];

	bool valid = true;
	bool invalid_fields[USER_FIELDS] = {false};

	// Reads username
	printf("username=");
	fgets(username, BUFSIZ, stdin);

	// Checks for spaces
	if (username[0] == '\n' || strstr(username, " ")) {
		valid = false;
		invalid_fields[0] = true;
	}

	// Removes trailing newline
	username[strlen(username) - 1] = '\0';

	// Reads password
	printf("password=");
	fgets(password, BUFSIZ, stdin);

	// Checks for spaces
	if (password[0] == '\n' || strstr(password, " ")) {
		valid = false;
		invalid_fields[1] = true;
	}

	// Removes trailing newline
	password[strlen(password) - 1] = '\0';

	// Returns NULL if username/password contains spaces or is empty
	if (!valid) {
		printf("Invalid user! Invalid fields:\n");
		if (invalid_fields[0]) {
			printf("username=%s\n", username);
		}
		if (invalid_fields[1]) {
			printf("password=%s\n", password);
		}

		return NULL;
	}

	// Creates JSON object
	JSON_Value *val = json_value_init_object();
	JSON_Object *obj = json_value_get_object(val);
	json_object_set_string(obj, "username", username);
	json_object_set_string(obj, "password", password);

	// Serializes JSON object
	char *user = json_serialize_to_string(val);

	// Frees JSON value
	json_value_free(val);

	return user;
}

void register_cmd() {
	// Sets up the user
	char *user = setup_user();

	// Returns if username/password contains spaces
	if (!user) {
		printf("Registration failed!\n");
		return;
	}

	// Sends POST request and receives the server's response
	char *response = recv_post(host, sock, REGISTER, NULL, user);

	// Checks if registration was successful
	if (!strstr(response, "is taken")) {
		printf("Registered successfully!\n");
	} else {
		printf("Registration failed! Username is already taken!\n");
	}

	free(user);
	free(response);
}

void login_cmd() {
	// Sets up the user
	char *user = setup_user();

	// Returns if username/password contains spaces
	if (!user) {
		printf("Login failed!\n");
		return;
	}

	// Sends POST request and receives the server's response
	char *response = recv_post(host, sock, LOGIN, NULL, user);

	// Gets a pointer to the cookie
	char *cookie_ptr = strstr(response, "Set-Cookie: ");

	// Checks if the cookie exists
	if (!cookie_ptr) {
		// Checks why the login failed
		if (strstr(response, "Credentials are not good!")) {
			printf("Login failed! Wrong password!\n");
		} else if (strstr(response, "No account with this username!")) {
			printf("Login failed! No account with this username exists!\n");
		}

		// Resets the connection state
		connected = false;
		entered = false;

		free(user);
		free(response);
		return;
	}

	// Moves the pointer to the cookie value
	cookie_ptr += 12;
	strtok(cookie_ptr, ";");

	// Checks if the user is already logged in
	if (connected) {
		// Clears the old cookie
		memset(cookies[0], 0, BUFSIZ);

		// Copies the new cookie
		strcpy(cookies[0], cookie_ptr);

		printf("New login successful (without logout)!\n");

		// Sets the user as having not entered the library
		entered = false;

		free(user);
		free(response);
		return;
	}

	// Allocates memory for the cookie
	cookies[0] = (char *)malloc(BUFSIZ * sizeof(char));
	DIE(!cookies[0], "cookies malloc() failed!");

	// Copies the cookie
	strcpy(cookies[0], cookie_ptr);

	// Sets the cookie count
	cookies_cnt = 1;

	printf("Login successful!\n");

	// Sets the connection state to being logged in
	connected = true;

	free(user);
	free(response);
}

void enter_lib() {
	// Checks if the user is logged in
	if (connected) {
		// Sends GET request and receives the server's response
		char *response = recv_get(host, sock, ACCESS, NULL, cookies,
									cookies_cnt);

		// Gets a pointer to the token
		char *tok = strstr(response, "token");

		// Checks if the token exists
		if (tok) {
			// Moves the pointer to the token value
			tok += 8;

			// Clears the old token
			memset(token, 0, BUFSIZ);

			// Copies the new token
			strcpy(token, tok);

			// Removes the trailing characters
			token[strlen(token) - 2] = '\0';

			// Checks if the user has already entered the library
			if (entered) {
				printf("Entered the library successfully (again)!\n");

				free(response);
				return;
			}

			printf("Entered the library successfully!\n");

			// Sets the user as having entered the library
			entered = true;

			free(response);
		} else {
			printf("Entering the library failed! Missing token!\n");
		}
	} else {
		printf("Entering the library failed! Please login first!\n");
	}
}

void get_books() {
	// Checks if the user has entered the library
	if (entered) {
		// Sends GET request and receives the server's response
		char *response = recv_get(host, sock, BOOKS, token, cookies,
									cookies_cnt);

		// Parses the response
		JSON_Value *val = json_parse_string(strstr(response, "["));

		// Gets the array of books
		JSON_Array *arr = json_value_get_array(val);
		int len = json_array_get_count(arr);

		// Prints the books
		for (int i = 0; i < len; ++i) {
			JSON_Object *obj = json_array_get_object(arr, i);
			printf("Book %d:\n", i + 1);
			printf("id=%d\n", (int)json_object_get_number(obj, "id"));
			printf("title=%s\n", json_object_get_string(obj, "title"));
		}

		// Frees the JSON value
		json_value_free(val);

		if (!len) {
			printf("No books found!\n");
		}

		free(response);
	} else {
		printf("Access denied! Please enter the library first!\n");
	}
}

void get_book() {
	// Checks if the user has entered the library
	if (entered) {
		char url[BUFSIZ];
		char id[MAX_DIGITS];

		// Reads the book's id
		printf("id=");
		fgets(id, MAX_DIGITS, stdin);

		// Checks if the id is valid
		if (id[0] == '\n') {
			printf("Getting the book failed! Invalid id!\n");
			return;
		}

		// Removes the trailing newline
		id[strlen(id) - 1] = '\0';	

		// Checks if the id is a number
		if (!is_number(id)) {
			printf("Getting the book failed! Invalid id!\n");
			return;
		}

		// Builds the url
		sprintf(url, "%s/%s", BOOKS, id);

		// Sends GET request and receives the server's response
		char *response = recv_get(host, sock, url, token, cookies, cookies_cnt);

		// Checks if the book was found
		if (strstr(response, "No book was found!")) {
			printf("Getting the book failed! Book not found!\n");
		} else {
			// Parses the response
			JSON_Value *val = json_parse_string(strstr(response, "{"));
			JSON_Object *obj = json_value_get_object(val);

			// Prints the book
			printf("Book found successfuly:\n");
			printf("title=%s\n", json_object_get_string(obj, "title"));
			printf("author=%s\n", json_object_get_string(obj, "author"));
			printf("genre=%s\n", json_object_get_string(obj, "genre"));
			printf("publisher=%s\n", json_object_get_string(obj, "publisher"));
			printf("page_count=%d\n",
					(int)json_object_get_number(obj, "page_count"));

			// Frees the JSON value
			json_value_free(val);
		}

		free(response);
	} else {
		printf("Getting the book failed! Please enter the library first!\n");
	}
}

char *create_book() {
	char title[BUFSIZ];
	char author[BUFSIZ];
	char genre[BUFSIZ];
	char page_count[MAX_DIGITS];
	char publisher[BUFSIZ];

	bool valid = true;
	bool invalid_fields[BOOK_FIELDS] = {false};

	// Reads the book's title
	printf("title=");
	fgets(title, BUFSIZ, stdin);

	// Checks if the title is valid
	if (title[0] == '\n') {
		valid = false;
		invalid_fields[0] = true;
	}

	// Removes the trailing newline
	title[strlen(title) - 1] = '\0';

	// Reads the book's author
	printf("author=");
	fgets(author, BUFSIZ, stdin);

	// Checks if the author is valid
	if (author[0] == '\n') {
		valid = false;
		invalid_fields[1] = true;
	}

	// Removes the trailing newline
	author[strlen(author) - 1] = '\0';

	// Reads the book's genre
	printf("genre=");
	fgets(genre, BUFSIZ, stdin);

	// Checks if the genre is valid
	if (genre[0] == '\n') {
		valid = false;
		invalid_fields[2] = true;
	}

	// Removes the trailing newline
	genre[strlen(genre) - 1] = '\0';

	// Reads the book's publisher
	printf("publisher=");
	fgets(publisher, BUFSIZ, stdin);

	// Checks if the publisher is valid
	if (publisher[0] == '\n') {
		valid = false;
		invalid_fields[3] = true;
	}

	// Removes the trailing newline
	publisher[strlen(publisher) - 1] = '\0';

	// Reads the book's page count
	printf("page_count=");
	fgets(page_count, MAX_DIGITS, stdin);

	// Checks if the page count is valid
	if (page_count[0] == '\n') {
		valid = false;
		invalid_fields[4] = true;
	}

	// Removes the trailing newline
	page_count[strlen(page_count) - 1] = '\0';

	// Checks if the page count is a positive integer (number)
	if (!is_number(page_count)) {
		valid = false;
		invalid_fields[4] = true;
	}

	// Checks if the book is valid
	if (!valid) {
		// Prints the invalid fields
		printf("Adding the book failed! Invalid book fields:\n");
		if (invalid_fields[0]) {
			printf("title=%s\n", title);
		}
		if (invalid_fields[1]) {
			printf("author=%s\n", author);
		}
		if (invalid_fields[2]) {
			printf("genre=%s\n", genre);
		}
		if (invalid_fields[3]) {
			printf("publisher=%s\n", publisher);
		}
		if (invalid_fields[4]) {
			printf("page_count=%s\n", page_count);
		}
		return NULL;
	}

	// Creates the book JSON object
	JSON_Value *val = json_value_init_object();
	JSON_Object *obj = json_value_get_object(val);
	json_object_set_string(obj, "title", title);
	json_object_set_string(obj, "author", author);
	json_object_set_string(obj, "genre", genre);
	json_object_set_number(obj, "page_count", atoi(page_count));
	json_object_set_string(obj, "publisher", publisher);

	// Serializes the JSON object
	char *book = json_serialize_to_string(val);

	// Frees the JSON value
	json_value_free(val);

	return book;
}

void add_book() {
	// Checks if the user has entered the library
	if (entered) {
		// Creates the book
		char *book = create_book();
		
		// Checks if the book is valid
		if (!book) {
			return;
		}

		// Sends the POST request and receives the server's response
		char *response = recv_post(host, sock, BOOKS, token, book);

		printf("Book added successfully!\n");

		free(book);
		free(response);
	} else {
		printf("Adding the book failed! Please enter the library first!\n");
	}
}

void delete_book() {
	// Checks if the user has entered the library
	if (entered) {
		char url[BUFSIZ];
		char id[MAX_DIGITS];

		// Reads the book's id
		printf("id=");
		fgets(id, MAX_DIGITS, stdin);

		// Checks if the id is valid
		if (id[0] == '\n') {
			printf("Deleting the book failed! Invalid id!\n");
			return;
		}

		// Removes the trailing newline
		id[strlen(id) - 1] = '\0';


		// Checks if the id is a positive integer (number)
		if (!is_number(id)) {
			printf("Deleting the book failed! Invalid id!\n");
			return;
		}

		// Builds the URL
		sprintf(url, "%s/%s", BOOKS, id);

		// Sends the DELETE request and receives the server's response
		char *response = recv_delete(host, sock, url, token, cookies,
										cookies_cnt);

		// Checks if the book was deleted
		if (strstr(response, "No book was deleted!")) {
			printf("Deleting the book failed! Book not found!\n");
		} else {
			printf("Book deleted successfully!\n");
		}

		free(response);
	} else {
		printf("Access denied! Please enter the library first!\n");
	}
}

void logout_cmd() {
	// Checks if the user is logged in
	if (connected) {
		// Sends the GET request and receives the server's response
		char *response = recv_get(host, sock, LOGOUT, token, cookies,
									cookies_cnt);

		// Resets the connection state and entered state
		connected = false;
		entered = false;

		// Frees the cookies
		for (int i = 0; i < cookies_cnt; ++i) {
			free(cookies[i]);
		}
		// Resets the cookies count
		cookies_cnt = 0;

		// Clears the token
		memset(token, 0, BUFSIZ);

		printf("Logged out successfully!\n");

		free(response);
	} else {
		printf("Logged out failed! You are not logged in!\n");
	}
}

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, BUFSIZ);

	// The client loop, which stops when "exit" is entered
	while (fgets(command, LINELEN, stdin)) {
		if (!strncmp(command, "exit", 4)) {
			break;
		} else {
			// Opens the socket connection to the server
			sock = open_connection(host, port, AF_INET, SOCK_STREAM, 0);

			if (!strncmp(command, "register", 8)) {
				register_cmd();
			} else if (!strncmp(command, "login", 5)) {
				login_cmd();
			} else if (!strncmp(command, "enter_library", 13)) {
				enter_lib();
			} else if (!strncmp(command, "get_books", 9)) {
				get_books();
			} else if (!strncmp(command, "get_book", 8)) {
				get_book();
			} else if (!strncmp(command, "add_book", 8)) {
				add_book();
			} else if (!strncmp(command, "delete_book", 11)) {
				delete_book();
			} else if (!strncmp(command, "logout", 6)) {
				logout_cmd();
			} else {
				printf("Invalid command!\n");
			}

			// Fixes the scanf bug (reads the extra newline character)
			scanf(" ");

			// Closes the socket connection to the server
			close_connection(sock);
		}
	}

	// Frees the cookies
	for (int i = 0; i < cookies_cnt; ++i) {
		free(cookies[i]);
	}

	return 0;
}
