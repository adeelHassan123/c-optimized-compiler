int main() {
    int i = 0;
    int n = 4;
    int k = 0;
    int x = 0;

    while (i < n) {
        k = 10 + 20;   /* loop invariant -> hoisted out */
        x = i * 2;     /* loop strength reduction -> i + i */
        i = i + 1;
    }

    print(k);
    print(x);
    return 0;
}
