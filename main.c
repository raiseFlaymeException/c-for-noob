#define CFN_IMPL
#include "c_for_noob/cfn.h"

#include <stdlib.h>
#include <stdio.h>


int main() {
    cfn_init_default_config();
    cfn_config.out = fopen("cfn_test.txt", "w");

    size_t size;
    
    printf("Enter a size: ");
    scanf("%d", &size);
    
    char *data = (char *)malloc(sizeof(char)*size);
    data = (char *)realloc(data, sizeof(char)*size*2);

    printf("Enter your name (max %d characters): ", size);
    scanf("%s", data);
    printf("your name is: %s\n", data);

    // no free
    // free(data);
    cfn_print_capture_ptr();

    cfn_quit();
    fclose(cfn_config.out);
    return 0;
}
