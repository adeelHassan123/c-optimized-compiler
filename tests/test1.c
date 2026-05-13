/* Test 1: Simple constant folding and arithmetic */
main() {
    int x;
    int y;
    
    x = 5 + 3;      // Will be folded to 8
    y = 10 * 2;     // Will be folded to 20
    
    print(x);
    print(y);
    
    return 0;
}
// Commit Marker

