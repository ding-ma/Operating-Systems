#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    int result;
    result = gethostname(hostname, HOST_NAME_MAX);
    if (result)
    {
        perror("gethostname");
        return EXIT_FAILURE;
    }
    result = getlogin_r(username, LOGIN_NAME_MAX);
    if (result)
    {
        perror("getlogin_r");
        return EXIT_FAILURE;
    }
    result = printf("Hello %s, you are logged in to %s.\n",
                    username, hostname);
    if (result < 0)
    {
        perror("printf");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}