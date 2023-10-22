#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char* strToUpper(char*);
void strToUpperASCII(char*);
int main(void) {
    char* firstName = (char*)malloc(40*sizeof(char));
    char* lastName = (char*)malloc(40*sizeof(char));
    printf("What is your first name?\n");
    scanf("%s", firstName);
    printf("What is your last name?\n");
    scanf("%s", lastName);
    char* result = strToUpper(lastName);
    strToUpperASCII(lastName);
    printf("first name is %s, second name is %s\n", firstName, lastName);

    // using strcomp to compare both.
    printf("the value of strcomp for comparison will be: %d\n", strcmp(lastName, result));

    // concatenation of first and second name
    char name[100];
    snprintf(name, sizeof name, "%s %s",firstName, lastName);
    printf("full name: %s\n", name);

    // reading birth year:
    int year;
    printf("please provide me with your date of birth:\n");
    scanf("%d", &year);
    printf("%d\n", year);

    // name includes year now
    snprintf(name, sizeof name, "%s %s %d test", firstName, lastName, year);
    printf("%s\n", name);

    // sscanf used to get to get values from a string, in this case namem which consistof firstname lastname and birthyear.
    year = 20; // just used to test whether the function will actually change store the value in name into year below.
    printf("%d\n", year);
    sscanf(name, "%s %s %d test", firstName, lastName, &year);
    printf("%s %s %d", firstName, lastName, year);
    free(firstName);
    free(lastName);
    free(result);
    return 0;
}

// to upper function via toupper()
char* strToUpper(char* str) {
    int i = 0;
    char *result = (char*)malloc(20*sizeof(char)); // if you were to define result as a string with a pointer only then will it be in ROM!
    while (str[i]) {
        result[i] = toupper(str[i]);
        i++;
    }
    return result;
}

// to upper function via ascii itself.
void strToUpperASCII(char *str) {
    int i = 0;
    while (str[i]) {
        str[i] = str[i] - 32;
        i++;
    }
}
















//
//
//    char* dynamic_name;
//    dynamic_name = (char *) malloc(200 * sizeof(char));
//    printf("Provide me a name please: \n");
//    if (dynamic_name == NULL) {
//        fprintf(stderr, "No name is supplied.");
//    }
//    else {
//        scanf("%s", dynamic_name);
//    }  //This means that the value is 'unknown'
//    printf("My name is: %s.", dynamic_name);
//    free(dynamic_name);
//
//
//    return 0;
//}
