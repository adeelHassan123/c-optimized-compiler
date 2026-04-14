/* Test 2: Algebraic simplification and strength reduction */
main() {
    int a;
    int b;
    int c;
    
    a = 100;
    b = a + 0;      // Will be simplified to a
    c = a * 1;      // Will be simplified to a
    
    print(a);
    print(b);
    print(c);
    
    return 0;
}
// Commit Marker