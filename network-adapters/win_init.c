//this macto determine the proper version of the windows headers are included
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#include <WinSock2.h>
#include <iphlpapi.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

/*
tells Microsoft Visual C which libraries to link with the executable 
Visual Studio compiler feature, other compilers must import on the commend line
*/
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib") 

int main() {
    WSADATA d;

    if(WSAStartup(MAKEWORD(2, 2), &d)) {
        printf("Failed to initialize.\n");
        return -1;
    }

    // store the adapters' address buffer
    DWORD asize = 20000;
    PIP_ADAPTER_ADDRESSES adapters;

    do {
        adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);

        if(!adapters) {
            printf("Couldn't allocate %ld bytes for adapters.\n", asize);
            WSACleanup();
            return -1;
        }

        /*
        AF_UNSPEC tells windows that we want both IPv4 and IPv6 addresses (AF_INET for IPv4 - AF_INET6 for IPv6)
        GAA_FLAG_INCLUDE_PREFIX required to include a list of addresses
        */
        int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize);

        if(r == ERROR_BUFFER_OVERFLOW) {
            printf("GetAdaptersAddresses wants %ld bytes.\n", asize);
            free(adapters);
        } else if(r == ERROR_SUCCESS) {
            break;
        } else {
            printf("Error from GetAdaptersAddresses: %d\n", r);
            free(adapters);
            WSACleanup();
            return -1;
        }
    } while(!adapters);

    PIP_ADAPTER_ADDRESSES adapter = adapters;

    while(adapter) {
        printf("\nAdapter name: %S\n", adapter->FriendlyName);

        PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
        while(address) {
            printf("\t%s", address->Address.lpSockaddr->sa_family == AF_INET ? "IPv4" : "IPv6");

            char ap[100];

            getnameinfo(address->Address.lpSockaddr, address->Address.iSockaddrLength, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);

            address = address->Next;
        }

        adapter = adapter->Next;
    }

    free(adapters);
    WSACleanup();
    printf("Ok.\n");
    return 0;
}