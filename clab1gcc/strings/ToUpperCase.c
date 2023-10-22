#include <stdio.h>
#include <stdlib.h>

char ToUpperCase(char ch){
//    int ascii_ch = *ch; //stores converts to ascii code.
//    char upper_ch = ascii_ch + 32;
//    return upper_ch;
    char* ascii_ch = *ch;
    printf("%p", ascii_ch);
//    char upper_ch = ascii_ch + 32;
//    return upper_ch;
      return 'c';
    }

}