# ComP

**Name: Mitran Andrei-Gabriel**
**Group: 323CA**

## Homework #4 (Web client. Communication with the REST API.)

### Organization

* The project's objective is to implement a client that communicates with a
server and performs commands such as user registration, user login and logout,
accessing the user's library, retrieving one or all books, adding books,
and deleting books. All input data validation is handled in the client.
* The modified files based on the lab 9 skeleton are "requests" and "client".
Additional files include "snd_recv" and "utils," which contain helper functions.
* The "parson" library was chosen for its ease of JSON creation and
conversion/parsing functions, making it useful for this particular case.

#### Helper functions

* The "requests" file follows the TO DOs in the original code and includes an
additional function for a DELETE request, which follows the structure of the GET
request. All functions include the authorization token.
* The "snd_recv" file provides wrappers for the "requests" functions, sending
the message and returning the reply directly to the client. Memory freeing for
the response is done in the client.
* The "utils" file includes basic functionalities such as an error handling
macro and a number validation function.

#### Client

* The client includes two global variables indicating whether the user is logged
in and whether the user has entered the library.
* The client enters a loop, expecting commands from stdin, until it receives
the command "exit."

##### register

* To register, a JSON containing the username and password is created. If the
username or password contains spaces, the client does not send the POST request
and prints an error. (This is due to the fact that spaces are typically replaced
with special characters. However, for safety reasons, the request is not sent).
Empty fields in the JSON are also forbidden.
* If the user JSON is valid, it is sent to the server. Depending on the server's
reply, an error message is printed if the username is already taken. If not, a
success message is printed.

##### login

* Similar to registration, a user JSON is created and checked for validity. If
it is not valid, an error is printed. Otherwise, a POST request is sent.
* Depending on the server's response, an error message is printed if the given
username does not exist or if the password is incorrect. If not, a success
message is printed, and the client's connection state changes to "logged in."
* If the client is already logged in and attempts to log in again without
logging out first, the client simply switches accounts and sets the entered
state to "not entered," informing the user.

##### enter_library

* If the user is logged in, a GET request is sent to obtain a JWT authorization
token. If not, an error is printed, instructing the user to log in first.
* If the token is present, it is stored internally, and a success message is
printed. If not, an error is printed. If the token exists, the entered state
changes to "entered."
* If an attempt to enter the library again is made, the client switches the
token internally and notifies the user that it entered the library again.

##### get_books

* If the user has entered the library, a request is sent to retrieve a JSON
array in character format. If not, an error is printed, requesting the user to
enter the library first.
* The response is parsed, and all books are printed. If no books are found, a
special message is printed.

##### get_book

* If the user has entered the library, a GET request is sent to retrieve a JSON
object in character format. If not, an error is printed, requesting the user to
enter the library first.
* Before sending the request, the user must input the book's ID, which can be
obtained by calling the get_books command. Only positive integers are accepted,
and any other input generates an error, preventing the request from being sent.
* If the ID is valid and the request is sent, the response is parsed into a JSON
object and printed. If no book with the specified ID exists, this is indicated.

##### add_book

* If the user has entered the library, a POST request containing the book JSON
is sent. If not, an error is printed, instructing the user to enter the library
first.
* Before sending the request, the user provides input for the book's title,
author, genre, publisher, and page count. All fields must contain information,
and the page count must be a positive integer. If any of the fields are invalid,
the request is not sent, and an error message is printed.
* If all fields are valid, the request is sent, and a success message is
printed.

##### delete_book

* If the user has entered the library, a DELETE request to delete a book is
sent, including the ID of the book to be deleted. If not, an error is printed,
instructing the user to enter the library first.
* Before sending the request, the user must input the book's ID, which can be
obtained by calling the get_books command. Only positive integers are accepted,
and any other input generates an error, preventing the request from being sent.
* If the ID is valid, the URL is formed (using the book's ID), and the request
is sent. If no book with the specified ID is found, an error message is printed.
Otherwise, a success message is printed.

##### logout

* To log out, the user must be logged in first; otherwise, an error message is
printed.
* A GET request is sent, and the connection and entered states are reset. The
cookies and JWT token are freed, and a success message is printed.

### Implementation

* Every functionality required for this homework was implemented.

### Compilation

* To compile, use:

```bash
make
```

### Resources

* Everything provided by the ComP team
* [Linux Manual](https://www.man7.org/linux/man-pages/index.html)
