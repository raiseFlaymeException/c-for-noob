# C For Noobs

A usefull library that help debuggin memory related bug, helped me multiple time


## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Example](#example)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)

## Install

compile and run the test program:
```cmd
make run

```
or

```cmd
gcc -o main.c main.exe -L src/include
```

the library is in the folder [src/include/c_for_noob](src/include/c_for_noob)

## Usage

I try to comment stuff in the [example](#example) and in the library to make it easier

```c
#define CFN_IMPL 
#include "c_for_noob/cfn.h"

// ...

int main(int argc, char *argv[]) {
    cfn_init_default_config(); // create default config (all field of cfn_config need to be feeled and it's a great we to do it)
    cfn_config.out = fopen("cfn_test.txt", "w"); // the output will go to this file

    // ...

    cfn_quit(); // need to be called to free internal thing and to create some output 
                // like for example a check of memory leak (see cfn_test.txt)
    fclose(cfn_config.out); // cfn_config.out does not closed itself
    return 0;
}

```

## Example

A better example in the file [main.c](main.c)

## Maintainers

[@raiseFlaymeException](https://github.com/raiseFlaymeException).

## Contributing

Feel free to contibute [Open an issue](https://github.com/raiseFlaymeException/c-for-noob/issues/new) or submit PRs.

## License

[ZLIB](LICENSE) © raiseFlaymeException
