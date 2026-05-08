/* Test 8: Extended Grammar - log() and exp() functions
 * Module 3: Extended Grammar
 * Tests math function calls with constant folding
 */
main() {
    float a;
    float b;
    float c;
    
    /* log(100) should be constant folded to ~4.605 */
    a = log(100);
    
    /* exp(1) should be constant folded to ~2.718 */
    b = exp(1);
    
    /* Exponentiation with ^ */
    c = 2 ^ 10;  /* 1024 */
    
    print(a);
    print(b);
    print(c);
    
    return 0;
}
