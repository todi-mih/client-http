#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"
#include <stdbool.h>
#include <ctype.h>

#define HOST "34.246.184.49"
#define PORT 8080

//Start with README please...

//Hope i dont have to explain these
//loged_in is a flag to check if we are loged in
//and access to see if we have entered the library

char *session_cookie = NULL;
char *jwt_token = NULL;
bool loged_in = false;
bool access = false;

//helper fn to check if username and password are in the correct format

int is_valid_username_password(const char *str) {
    if (strlen(str) <= 1) return 0;
    while (*str) {
        if (isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

void register_user() {
    char username[50], password[50];

    printf("username=");
    fflush(stdout);
    if (fgets(username, sizeof(username), stdin) == NULL) {
        printf("ERROR reading username\n");
        return;
    }
    username[strcspn(username, "\n")] = '\0'; 
    
    //read README
   // if(contains_non_alphanumeric(username)) {
     //   printf("ERROR ,the username contains non-alphanumberic chars!\n");
     //   return;
   // }
    printf("password=");
    fflush(stdout);
    if (fgets(password, sizeof(password), stdin) == NULL) {
        printf("ERROR reading password\n");
        return;
    }
    password[strcspn(password, "\n")] = '\0'; 

    if (!is_valid_username_password(username)) {
        printf("ERROR. Invalid username\n");
        return;
    }

    if (!is_valid_username_password(password)) {
        printf("ERROR. Invalid password\n");
        return;
    }

     int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
   
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    char *payload = json_serialize_to_string(root_value);

    char *message = compute_post_request(HOST, "/api/v1/tema/auth/register", "application/json",
                    &payload, sizeof(payload), NULL, 0);
    
    send_to_server(sockfd, message);
   
    char *response = receive_from_server(sockfd);
  
    close_connection(sockfd);

    if(strstr(response, "error")) {
      printf("ERROR. %s\n",response);
    }
    else {
      printf("SUCCESS . Client registered %s\n",response);
    }

    json_value_free(root_value);
    free(message);
    free(response);
}

void login_user() {
    if(loged_in){
        printf("ERROR, already loged in!\n");
        return;
    }
    char username[50], password[50];

    printf("username=");
    fflush(stdout);
    if (fgets(username, sizeof(username), stdin) == NULL) {
        printf("ERROR reading username\n");
        return;
    }
    username[strcspn(username, "\n")] = '\0'; 

    printf("password=");
    fflush(stdout);
    if (fgets(password, sizeof(password), stdin) == NULL) {
        printf("ERROR reading password\n");
        return;
    }
    password[strcspn(password, "\n")] = '\0'; 

    if (!is_valid_username_password(username)) {
        printf("ERROR. Invalid username\n");
        return;
    }

    if (!is_valid_username_password(password)) {
        printf("ERROR. Invalid password\n");
        return;
    }

   
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    char *payload = json_serialize_to_string(root_value);

    char *message;
    char *response;
    int sockfd;

    
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_post_request(HOST, "/api/v1/tema/auth/login", "application/json", &payload, 1, NULL, 0);
   // printf("Request:\n%s\n", message);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
   // printf("Response:\n%s\n", response);

    if (strstr(response, "error") != NULL) {
        printf("ERROR: %s\n", basic_extract_json_response(response));
    } else {
        printf("SUCCESS: User logged in successfully!\n");
        loged_in = true;
        
        char *cookie_start = strstr(response, "Set-Cookie: ");
        if (cookie_start) {
            cookie_start += strlen("Set-Cookie: ");
            char *cookie_end = strstr(cookie_start, ";");
            session_cookie = strndup(cookie_start, cookie_end - cookie_start);
        }
    }

    close_connection(sockfd);
    free(message);
    free(response);
    json_free_serialized_string(payload);
    json_value_free(root_value);
}

void enter_library() {
    if(!loged_in){
        printf("ERROR, where you going buddy, you are not loged in!\n");
        return;
    }

    if(access){
        printf("ERROR, you alredy have access\n");
        return;
    }

    char *message;
    char *response;
    int sockfd;

    
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char *cookies[] = { session_cookie };
    message = compute_get_request(HOST, "/api/v1/tema/library/access", NULL, cookies, 1,NULL);
    //printf("Request:\n%s\n", message);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    //printf("Response:\n%s\n", response);

    if (strstr(response, "ERROR") != NULL) {
        printf("ERROR: %s\n", basic_extract_json_response(response));
    } else {
        printf("SUCCESS: Entered the library!\n");
        access = true;
        char *token_start = strstr(response, "{\"token\":\"");
        if (token_start) {
            token_start += strlen("{\"token\":\"");
            char *token_end = strstr(token_start, "\"}");
            jwt_token = strndup(token_start, token_end - token_start);
        }
    }

    close_connection(sockfd);
    free(message);
    free(response);
}

void get_books() {
    if(!loged_in){
        printf("ERROR, where you going buddy, you are not loged in!\n");
        return;
    }
    
    if(!access){
        printf("ERROR, you dont have access to library\n");
        return;
    }

    char *message;
    char *response;
    int sockfd;

    
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    message = compute_get_request(HOST, "/api/v1/tema/library/books",  NULL, NULL, 0,
                    jwt_token);
    //printf("Request:\n%s\n", message);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    if(strstr(response, "error")) {
      printf("Error. %s\n",response);
    }
    else {
      printf("Success \n%s\n", strstr(response, "{"));
    }

    close_connection(sockfd);
    free(message);
    free(response);
}

void add_book() {
    if(!loged_in){
        printf("ERROR, where you going buddy, you are not loged in!\n");
        return;
    }

    if(!access){
        printf("ERROR, you dont have access to library\n");
        return;
    }

    char title[100], author[100], genre[100], publisher[100];
    int page_count;
    char buffer[10];

    printf("title=");
    fflush(stdout);
    if (fgets(title, sizeof(title), stdin) == NULL) {
        printf("ERROR reading title\n");
        return;
    }
    title[strcspn(title, "\n")] = '\0'; 

    printf("author=");
    fflush(stdout);
    if (fgets(author, sizeof(author), stdin) == NULL) {
        printf("ERROR reading author\n");
        return;
    }
    author[strcspn(author, "\n")] = '\0'; 

    printf("genre=");
    fflush(stdout);
    if (fgets(genre, sizeof(genre), stdin) == NULL) {
        printf("ERROR reading genre\n");
        return;
    }
    genre[strcspn(genre, "\n")] = '\0'; 

    printf("publisher=");
    fflush(stdout);
    if (fgets(publisher, sizeof(publisher), stdin) == NULL) {
        printf("ERROR reading publisher\n");
        return;
    }
    publisher[strcspn(publisher, "\n")] = '\0'; 

    printf("page_count=");
    fflush(stdout);
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("ERROR reading page_count\n");
        return;
    }
    if (sscanf(buffer, "%d", &page_count) != 1) {
        printf("ERROR parsing page_count\n");
        return;
    }

   
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_string(root_object, "publisher", publisher);
    json_object_set_number(root_object, "page_count", page_count);
    char *payload = json_serialize_to_string(root_value);

    char *message;
    char *response;
    int sockfd;

  
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char auth_header[400];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", jwt_token);
    char *headers[] = {auth_header};

    message = compute_post_request(HOST, "/api/v1/tema/library/books", "application/json", &payload, 1, headers, 1);
    //printf("Request:\n%s\n", message);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    ///printf("Response:\n%s\n", response);

    if (strstr(response, "ERROR") != NULL) {
        printf("ERROR: %s\n", basic_extract_json_response(response));
    } else {
        printf("SUCCESS: Book added successfully!\n");
    }

    close_connection(sockfd);
    free(message);
    free(response);
    json_free_serialized_string(payload);
    json_value_free(root_value);
}

void delete_book(){
    if(!loged_in){
        printf("ERROR, where you going buddy, you are not loged in!\n");
        return;
    }

    if(!access){
        printf("ERROR, you dont have access to library\n");
        return;
    }

    char book_id[50];

    printf("book_id=");
    fflush(stdout);
    if (fgets(book_id, sizeof(book_id), stdin) == NULL) {
        printf("ERROR reading book ID\n");
        return;
    }
    
     book_id[strcspn(book_id, "\n")] = '\0'; 

   if(!is_number(book_id)){
    printf("ERROR , enter an actual number!\n");
    return;
   }

    char *message;
    char *response;
    int sockfd;

    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char url[100];
    snprintf(url, sizeof(url), "/api/v1/tema/library/books/%s", book_id);
    message = compute_delete_request(HOST, url, jwt_token);
    //printf("Request:\n%s\n", message);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    //printf("Response:\n%s\n", response);

    if (strstr(response, "ERROR") != NULL) {
        printf("ERROR: %s\n", basic_extract_json_response(response));
    } else {
        printf("SUCCESS: Book deleted successfully!\n");
    }

    close_connection(sockfd);
    free(message);
    free(response);
}

void get_book() { 
    if(!loged_in){
        printf("ERROR, where you going buddy, you are not loged in!\n");
        return;
    }

    if(!access){
        printf("ERROR, you dont have access to library\n");
        return;
    }

    char book_id[50];

    printf("book_id=");
    fflush(stdout);
    if (fgets(book_id, sizeof(book_id), stdin) == NULL) {
        printf("ERROR reading book ID\n");
        return;
    }
    book_id[strcspn(book_id, "\n")] = '\0'; 
    
    if(!is_number(book_id)){
    printf("ERROR , enter an actual number!\n");
    return;
    }

    char *message;
    char *response;
    int sockfd;

    
    sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char url[100];
    snprintf(url, sizeof(url), "/api/v1/tema/library/books/%s", book_id);
     message = compute_get_request(HOST, url,NULL,NULL, 0, jwt_token);
    //printf("Request:\n%s\n", message);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    //printf("Response:\n%s\n", response);

    if (strstr(response, "ERROR") != NULL) {
        printf("ERROR: %s\n", basic_extract_json_response(response));
    } else {
        printf("SUCCESS \n%s\n", strstr(response, "{"));
    }

    close_connection(sockfd);
    free(message);
    free(response);
}

void logout() {

    if(!loged_in){
        printf("ERROR, where you going buddy, you are not loged in!\n");
        return;
    }

    int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    char logout_url[] = "/api/v1/tema/auth/logout";
    char *cookies[] = {session_cookie};

    char *message = compute_get_request(HOST, logout_url, NULL, cookies,sizeof(cookies) / sizeof(char ** ), "");
    send_to_server(sockfd, message);
   
    char *response = receive_from_server(sockfd);
    close_connection(sockfd);

    if(strstr(response, "error")) {
        printf("ERROR. You are not logged out\n");
      }
      else {
        printf("SUCCESS. Client logged out\n");
        loged_in = false;
        access = false;
        
        if (session_cookie) free(session_cookie);
        if (jwt_token) free(jwt_token);
        
      }

    free(message);
    free(response);
}

int main() {
    char command[50];

    while (1) {
       
        fflush(stdout);
        if (fgets(command, sizeof(command), stdin) == NULL) {
            printf("ERROR reading command\n");
            continue;
        }
        command[strcspn(command, "\n")] = '\0'; 

        if (strcmp(command, "register") == 0) {
            register_user();
        } else if (strcmp(command, "login") == 0) {
            login_user();
        } else if (strcmp(command, "enter_library") == 0) {
            enter_library();
        } else if (strcmp(command, "get_books") == 0) {
            get_books();
        } else if (strcmp(command, "add_book") == 0) {
            add_book();
        } else if (strcmp(command,"delete_book") == 0) {
            delete_book();
        } else if (strcmp(command,"get_book") == 0) {
            get_book();
        }else if (strcmp(command,"logout") == 0) {
            logout();
        }else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Unknown command: %s\n", command);
        }
    }

    if (session_cookie) free(session_cookie);
    if (jwt_token) free(jwt_token);

    return 0;
}
