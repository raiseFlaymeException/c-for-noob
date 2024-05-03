#define CFN_IMPL
#include "c_for_noob/cfn.h"

#include <stdlib.h>
#include <stdio.h>


int main() {
    cfn_init_default_config(); // create default config (all field of cfn_config need to be feeled and it's a great we to do it)
    cfn_config.out = fopen("cfn_test.txt", "w"); // the output will go to this file
    
    /////////////////////////////////////////////////////////
    // a random program that ask the size of your name, get the name of your name and print it
    size_t size;
    
    printf("Enter a size: ");
    scanf("%d", &size);
    
    char *data = (char *)malloc(sizeof(char)*size);

    printf("Enter your name (max %d characters): ", size);
    scanf("%s", data); // (it's an example I know it's unsafe)
    printf("your name is: %s\n", data);

    // forgot to free
    // free(data);
    /////////////////////////////////////////////////////////

    cfn_print_capture_ptr(); // print all pointers allocated in a table see (cfn_test.txt) and if they have been freed

    cfn_quit(); // need to be called to free internal thing and to create some output 
                // like for example a check of memory leak (see cfn_test.txt)
    fclose(cfn_config.out); // cfn_config.out does not closed itself
    return 0;
}
