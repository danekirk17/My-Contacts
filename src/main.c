#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../includes/assLib.h"

int main() {
    FILE *file = fopen("myContactList.db", "ab+");
    unsigned long phone = 0;
    char *ans = malloc(sizeof(char) * 10);

    while(1) {

        printf("Do you wish to enter a new contact? ");
        fgets(ans, 8, stdin);

        if(strcmp(ans, "Yes\n") == 0) {
            newRecord(file);
            memset(ans, 0, strlen(ans));

        } else if(strcmp(ans, "No\n") == 0) {
            memset(ans, 0, strlen(ans));
            printf("Do you wish to retrieve a contact? (Yes or No)? ");
            fgets(ans, 8, stdin);
            if(strcmp(ans, "Yes\n") == 0) {
                memset(ans, 0, strlen(ans));
                printf("Phone Number: ");
                scanf("%lu", &phone);
                getchar();
                readRecord(file, phone);
                phone = 0;
            } else if(strcmp(ans, "No\n") == 0) {free(ans); free(file); break;}
        }
    }
    return 0;
}
