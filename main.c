#include "utils.h"
#include <stdio.h>

int main(void) {
    save_all_outputs();
    printf("Done. CSV files are in ./output and ROS2/FastDDS configs are in ./config.\n");
    return 0;
}
