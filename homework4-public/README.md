  The skeleton and helper functions andfiles are taken from lab 9,the json parser used is 
https://github.com/kgabis/parson.git.The program start with an infinte loop that breaks only when we use exit,then for each commandit has its own functions.I didnt feel the need to comment much for this program,
becouse all the functions follow basiclly the same structure,they check their respective flags,(for example 
: we cant do things unlesss we are logged in),then it reads the data from stdin if needed and how its needed
opens the connection,form the request either post or get or delete with the needed json formatting,send it,
recive a respond,and see if we succed or not,and finally clean up.THese is the structure of all my functions,
but just in case i am adding a very short description for them.

register_user :

Reads username and password.
Validates inputs.(checks for " ",can also check for non-alphanumberic if needed)
Sends registration request to the server.
Prints success or error message.

login_user :

Checks if already logged in.
Reads username and password.
Validates inputs. (" ")
Sends login request to the server.
Prints success or error message.
Stores session cookie.

enter_library :

Checks if logged in and not already accessed.
Sends access library request to the server.
Prints success or error message.
Stores JWT token.

get_books :

Checks if logged in and library access.
Sends request to get books from the server.
Prints books or error message.

add_book :

Checks if logged in and library access.
Reads book details. (page count is made sure to be number,sorry if i forgot any other flags)
Sends request to add a book to the server.
Prints success or error message.

delete_book :

Checks if logged in and library access.
Reads book ID.
Validates book ID.(needs to be number)
Sends request to delete the book from the server.
Prints success or error message.

get_book :

Checks if logged in and library access.
Reads book ID.
Validates book ID.
Sends request to get book details from the server.
Prints book details or error message.

logout :

Checks if logged in.
Sends logout request to the server.
Prints success or error message.
Clears session and access tokens.

Why use that json parser?
Well obviously becouse it is the simplest one and it was in the web page,
by this question i thought you meant what functions so here is a discription of them

json_value_init_object():
Used when creating a new JSON object to store user data, such as in the register_user() and login_user() functions.

json_value_get_object(JSON_Value *value):
Used to get the JSON object after initializing it to add key-value pairs.

json_object_set_string(JSON_Object *object, const char *name, const char *string):
Used to add user information (like username and password) to the JSON object.

json_object_set_number(JSON_Object *object, const char *name, double number):
Used to add numerical data, such as the page count of a book in the add_book() function.

json_serialize_to_string(JSON_Value *value):
Converts the JSON object to a string format for sending in HTTP requests.

json_free_serialized_string(char *string):
Ensures there are no memory leaks by freeing the serialized string after use.

json_value_free(JSON_Value *value):
Ensures proper memory management by freeing JSON values after they are no longer needed.