#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
// struct person with 3 fields
struct person {
    int id;
    char fname[20];
    char lname[20];
};

// Driver program
int main()
{
    FILE* infile;

    // Open person.dat for reading
    infile = fopen("person1.dat", "rr+");
    if (infile == NULL) {
        fprintf(stderr, "\nError opening file\n");
        perror("the error message is: ");
        exit(1);
    }

    struct person write_struct = { 1, "Rohan", "Sharma" };

    // writing to file
    fwrite(&write_struct, sizeof(write_struct), 1, infile);

    struct person read_struct;

    // setting pointer to start of the file
    rewind(infile);

    // reading to read_struct
    fread(&read_struct, sizeof(read_struct), 1, infile);

    printf("Name: %s %s \nID: %d", read_struct.fname,
           read_struct.lname, read_struct.id);

    // close file
    fclose(infile);

    return 0;
}