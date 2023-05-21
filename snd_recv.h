/* SPDX-License-Identifier: EUPL-1.2 */
/* Copyright Mitran Andrei-Gabriel 2023 */

#ifndef _SND_RECV_H_
#define _SND_RECV_H_

#define APP_JSON "application/json"

/**
 * @brief Sends a GET request to the server and receives the reply.
 * @param host The host
 * @param sock The server socket
 * @param url The url
 * @param token The JWT token
 * @param cookies The cookies
 * @param cookies_cnt The number of cookies
 * @return The server reply
*/
char *recv_get(char* host, int sock, char *url, char *token, char **cookies,
				int cookies_cnt);

/**
 * @brief Sends a POST request to the server and receives the reply.
 * @param host The host
 * @param sock The server socket
 * @param url The url
 * @param token The JWT token
 * @param data The data (user or book, in JSON format)
 * @return The server reply
 */
char *recv_post(char *host, int sock, char *url, char *token, char *data);

/**
 * @brief Sends a DELETE request to the server and receives the reply.
 * @param host The host
 * @param sock The server socket
 * @param url The url
 * @param token The JWT token
 * @param cookies The cookies
 * @param cookies_cnt The number of cookies
 * @return The server reply
 */
char *recv_delete(char* host, int sock, char *url, char *token, char **cookies,
                    int cookies_cnt);

#endif // _SND_RECV_H_