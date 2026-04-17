#include <stdio.h>
#include <stdlib.h>

int main() {
    int sum;
    int i;
    sum = 0;
    i = 1;
    while ((i <= 10)) {
        sum = (sum + i);
        i = (i + 1);
    }
    if ((sum > 50)) {
        printf("%d\n", sum);
    }
    return 0;
}
// Commit Marker