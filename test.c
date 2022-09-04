#include <stdio.h>
#include "fs.h"



int
main()
{
    // create_fs();
    // sync_fs();
    mount_fs();
    // allocate_file("Zitha  ");

    // set_filesize(0, 5000);

    // char data = 'b';

    // int i;
    // for(i=0; i<49; i++){
    //     write_byte(0, i*100, &data);
    // }

    // sync_fs();
    print_fs();
    printf("Done. \n");

    return 0;
}
