#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../includes/assLib.h"

void newRecord(FILE *file) {
    struct contact con;
    unsigned long phone = 0;
    char *fName = malloc(sizeof(char) * 10000);
    char *lName = malloc(sizeof(char) * 10000);
    char *cName = malloc(sizeof(char) * 10000);
    char *email = malloc(sizeof(char) * 10000);
    char strPhone[20];
    int pFlag = 0, i;

    /*Read data into buffers*/
    printf("First Name: ");
    fgets(fName, 9990, stdin);
    fName[strlen(fName)-1] = '\0';

    printf("Last Name: ");
    fgets(lName, 9990, stdin);
    lName[strlen(lName)-1] = '\0';

    printf("Company Name: ");
    fgets(cName, 9990, stdin);
    cName[strlen(cName)-1] = '\0';

    while(1) {
        pFlag = 0;
        printf("Phone Number (enter only numbers): ");
        fgets(strPhone, 15, stdin);
        strPhone[strlen(strPhone)-1] = '\0';
        for(i = 0; i < strlen(strPhone); i++) {
            if(isdigit(strPhone[i]) == 0) {pFlag = 1;}
        }
        if(strlen(strPhone) == 0) {pFlag = 1;}
        if(pFlag == 0) {sscanf(strPhone, "%lu", &phone); break;}
    }

    printf("Email: ");
    fgets(email, 9990, stdin);
    email[strlen(email)-1] = '\0';

    /*Calculate posn values BEFORE writing*/
    fseek(file, 0, SEEK_END);
    con.first_name_posn = ftell(file) + (sizeof(struct contact));
    if(strlen(fName) != 0) {con.last_name_posn = con.first_name_posn + strlen(fName) + 1;} else {con.last_name_posn = con.first_name_posn;}
    if(strlen(lName) != 0) {con.company_name_posn = con.last_name_posn + strlen(lName) + 1;} else {con.company_name_posn = con.last_name_posn;}
    if(strlen(cName) != 0) {con.email_posn = con.company_name_posn + strlen(cName) + 1;} else {con.email_posn = con.company_name_posn;}
    if(strlen(email) != 0) {con.next = con.email_posn + strlen(email) + 1;} else {con.next = con.email_posn;}

    /*set missing data posn to zero*/
    if (strlen(fName) == 0) {con.first_name_posn = 0;}
    if (strlen(lName) == 0) {con.last_name_posn = 0;}
    if (strlen(cName) == 0) {con.company_name_posn = 0;}
    if (strlen(email) == 0) {con.email_posn = 0;}

    /*Write struct to file*/
    con.phone_number = phone;
    fwrite(&con, sizeof(struct contact), 1, file);

    /*Write data to file*/
    fseek(file, con.first_name_posn, SEEK_SET);
    if(con.first_name_posn != 0) {fwrite(fName, sizeof(char), strlen(fName) + 1, file);}
    fseek(file, con.last_name_posn, SEEK_SET);
    if(con.last_name_posn != 0) {fwrite(lName, sizeof(char), strlen(lName) + 1, file);}
    fseek(file, con.company_name_posn, SEEK_SET);
    if(con.company_name_posn != 0) {fwrite(cName, sizeof(char), strlen(cName) + 1, file);}
    fseek(file, con.email_posn, SEEK_SET);
    if(con.email_posn != 0) {
        fwrite(email, sizeof(char), strlen(email) + 1, file);
    }

    free(fName); free(lName); free(cName); free(email);
}

int readRecord(FILE *file, unsigned long sPhone) {
    struct contact con;
    unsigned long num = 0;
    char *fName = calloc(sizeof(char), 10000);
    char *lName = calloc(sizeof(char), 10000);
    char *cName = calloc(sizeof(char), 10000);
    char *email = calloc(sizeof(char), 10000);
    long next = 0;
    char *c = malloc(sizeof(char));

    /*Read first phone number in file and it's next posn value*/
    fseek(file, 0, SEEK_SET);
    fread(&num, sizeof(long), 1, file);
    fseek(file, sizeof(long) * 4, SEEK_CUR);
    fread(&next, sizeof(long), 1, file);

    /*If it is not the same, continue searching until it is found or EOF*/
    while(num != sPhone) {
        fseek(file, next, SEEK_SET);
        fread(&num, sizeof(long), 1, file);
        fseek(file, sizeof(long) * 4, SEEK_CUR);
        fread(&next, sizeof(long), 1, file);
        if(feof(file) != 0) {
            printf("No match found.\n");
            free(fName); free(lName); free(cName); free(email); free(c);
            return 0;
        }
    }

    /*Move cursor back to the first name posn value in struct and read posn values into struct*/
    fseek(file, -sizeof(long) * 5, SEEK_CUR);
    fread(&con.first_name_posn, sizeof(long), 1, file);
    fread(&con.last_name_posn, sizeof(long), 1, file);
    fread(&con.company_name_posn, sizeof(long), 1, file);
    fread(&con.email_posn, sizeof(long), 1, file);

    /*Read contact data into strings, checking for missing data with posn == 0*/
    if(con.first_name_posn != 0) {
        fseek(file, con.first_name_posn, SEEK_SET);
        while(1) {
            fread(c, 1, 1, file);
            if(*c == '\0') {
                strcat(fName, c);
                break;
            }
            strcat(fName, c);
        }
    }

    if(con.last_name_posn != 0) {
        fseek(file, con.last_name_posn, SEEK_SET);
        while(1) {
            fread(c, 1, 1, file);
            if(*c == '\0') {
                strcat(lName, c);
                break;
            }
            strcat(lName, c);
        }
    }

    if(con.company_name_posn != 0) {
        fseek(file, con.company_name_posn, SEEK_SET);
        while(1) {
            fread(c, 1, 1, file);
            if(*c == '\0') {
                strcat(cName, c);
                break;
            }
            strcat(cName, c);
        }
    }

    if(con.email_posn != 0) {
        fseek(file, con.email_posn, SEEK_SET);
        while(1) {
            fread(c, 1, 1, file);
            if(*c == '\0') {
                strcat(email, c);
                break;
            }
            strcat(email, c);
        }
    }

    printf("First Name: %s\nLast Name: %s\nCompany Name: %s\nPhone Number: %lu\nEmail: %s\n", fName, lName, cName, num, email);

    free(fName); free(lName); free(cName); free(email); free(c);

    return 1;
}
