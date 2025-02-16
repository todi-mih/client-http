#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;

        
    // Ex 1.1: GET dummy from main server
    sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("34.254.242.81:8080", "/api/v1/dummy", NULL, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    //close_connection(sockfd);

    // Ex 1.2: POST dummy and print response from main server
    //sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    char *params_dummy[] = {"nume=Ana", "are=mere"};

    message = compute_post_request("34.254.242.81:8080", "/api/v1/dummy", "application/x-www-form-urlencoded",
        params_dummy, 2, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    //close_connection(sockfd);

    // Ex 2: Login into main server
    //sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    char *params_login[] = {"username=student", "password=student"};

    message = compute_post_request("34.254.242.81:8080", "/api/v1/auth/login", "application/x-www-form-urlencoded",
        params_login, 2, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    //close_connection(sockfd);

    char* cookie_inex1 = strstr(response, "Set-Cookie:") + strlen("Set-Cookie: ");
    char* cookie_inex2 = strstr(cookie_inex1, "\r\n");

    int cookie_len = strlen(cookie_inex1) - strlen(cookie_inex2);

    char* cookie = (char*)calloc(300, sizeof(char));
    strncpy(cookie, cookie_inex1, cookie_len);
    //strcat(cookie, "\0");

    char* cookies[100];
    char* delimiter = "; ";
    int cookies_nr = 0;

    char* token = strtok(cookie, delimiter);
    while (token != NULL && cookies_nr < 100) {
        cookies[cookies_nr] = strdup(token);
        cookies_nr++;
        token = strtok(NULL, delimiter);
    }

    printf("cookie-TEST\n");
    for (int i = 0; i < cookies_nr; i++)
    {
        printf("%s\n", cookies[i]);
    }
    printf("cookie-TEST\n");

    // Ex 3: GET weather key from main server

    //sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request("34.254.242.81:8080", "/api/v1/weather/key", NULL, cookies, cookies_nr);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    //close_connection(sockfd);
    free(cookie);

    char* full_key = strstr(response, "{");
    printf("key-TEST\n");
    printf("%s\n", full_key);
    printf("key-TEST\n");

    char* key_start = strstr(response, "key") + 6;
    char* key_end = strstr(key_start, "\"");
    int key_len = strlen(key_start) - strlen(key_end);

    char* key = (char*)calloc(300, sizeof(char));;
    strncpy(key, key_start, key_len);
    strcat(key, "\0");

    printf("key-TEST\n");
    printf("%s\n", key);
    printf("key-TEST\n");

    // Ex 4: GET weather data from OpenWeather API

    //sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);
    char* params = (char*)calloc(500, sizeof(char));
    strcpy(params, "lat=44.7398&lon=22.2767&appid=");
    strcat(params, key);

    message = compute_get_request("api.openweathermap.org", "/data/2.5/history/city", params, NULL, 0);
    puts(message);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);
    close_connection(sockfd);

    free(key);
    free(params);

    // Ex 5: POST weather data for verification to main server
    // Ex 6: Logout from main server

    // BONUS: make the main server return "Already logged in!"

    // free the allocated data at the end!

    return 0;
}
