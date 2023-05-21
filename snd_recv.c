// SPDX-License-Identifier: EUPL-1.2
/* Copyright Mitran Andrei-Gabriel 2023 */

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include "helpers.h"
#include "requests.h"
#include "snd_recv.h"

char *recv_get(char *host, int sock, char *url, char *token, char **cookies,
				int cookies_cnt) {
	char *message =
		compute_get_request(host, url, NULL, cookies, cookies_cnt, token);

	send_to_server(sock, message);

	free(message);

	return receive_from_server(sock);
}

char *recv_post(char *host, int sock, char *url, char *token, char *data)  {
    // The body data for the POST request
	char *body_data[1];
	body_data[0] = data;
	
	char *message =
		compute_post_request(host, url, APP_JSON, body_data, 1, NULL, 0, token);

	send_to_server(sock, message);

	free(message);

	return receive_from_server(sock);
}

char *recv_delete(char *host, int sock, char *url, char *token, char **cookies,
					int cookies_cnt) {
	char *message =
		compute_delete_request(host, url, NULL, cookies, cookies_cnt, token);

	send_to_server(sock, message);

	free(message);

	return receive_from_server(sock);
}
