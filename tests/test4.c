/* Test 4: Comprehensive test with various optimizations */
main() {
    int x;
    int y;
    int z;
    int result;
    
    // Constant folding
    x = 100;
    y = 50;
    z = x + y;  // 150
    
    // Algebraic simplification
    result = z + 0;  // Will be simplified
    
    // Comparisons
    if (result == 150) {
        print(result);
    } else {
        print(0);
    }
    
    // Loop with constant
    int count;
    count = 0;
    
    while (count < 5) {
        count = count + 1;
    }
    
    print(count);
    
    return 0;
}
// Commit Marker