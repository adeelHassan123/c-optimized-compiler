#include <stdio.h>
#include <stdlib.h>

int main() {
    int x;
    int y;
    int z;
    int result;
    x = 100;
    y = 50;
    z = (x + y);
    result = z;
    if ((result == 150)) {
        printf("%d\n", result);
    }
    else {
        printf("%d\n", 0);
    }
    int count;
    count = 0;
    while ((count < 5)) {
        count = (count + 1);
    }
    printf("%d\n", count);
    return 0;
}
// Commit Marker