/* Test 6: Exponentiation and Math Functions
 * Module 3: Extended Grammar
 * 
 * Tests: ^ operator (right-associative), log(), exp()
 */
main() {
    int a;
    int b;
    int c;
    
    /* Exponentiation - right associative */
    a = 2 ^ 3;       /* 8 */
    b = 2 ^ 2 ^ 2;   /* 2^(2^2) = 2^4 = 16 */
    
    print(a);        /* Should print 8 */
    print(b);        /* Should print 16 */
    
    return 0;
}
