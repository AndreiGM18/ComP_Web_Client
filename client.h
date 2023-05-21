/* SPDX-License-Identifier: EUPL-1.2 */
/* Copyright Mitran Andrei-Gabriel 2023 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

// URLs
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define ACCESS "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"

#define COOKIES_MAX 1
#define MAX_DIGITS 32
#define USER_FIELDS 2
#define BOOK_FIELDS 5

/**
 * @brief Sets up the user.
 * @return The user (as a string in JSON format)
 */
char *setup_user();

/**
 * @brief Handles the register command.
 */
void register_cmd();

/**
 * @brief Handles the login command.
 */
void login_cmd();

/**
 * @brief Handles the enter_library command.
 */
void enter_lib();

/**
 * @brief Handles the get_books command.
 */
void get_books();

/**
 * @brief Handles the get_book command.
 */
void get_book();

/**
 * @brief Creates a book.
 * @return The book (as a string in JSON format) 
*/
char *create_book();

/**
 * @brief Handles the add_book command.
 */
void add_book();

/**
 * @brief Handles the delete_book command.
 */
void delete_book();

/**
 * @brief Handles the logout command.
 */
void logout_cmd();

#endif // _CLIENT_H_
