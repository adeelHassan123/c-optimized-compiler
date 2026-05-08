/* Test 7: Extended Grammar with Math Functions
 * Module 3: Extended Grammar with log() and exp()
 * 
 * Note: This tests the parsing of log and exp identifiers
 */
main() {
    int log_val;
    int exp_val;
    int result;
    
    /* Using log and exp as variable names to test lexer */
    log_val = 10;
    exp_val = 20;
    
    /* Arithmetic with exponentiation */
    result = log_val ^ 2 + exp_val;  /* 10^2 + 20 = 120 */
    
    print(log_val);
    print(exp_val);
    print(result);
    
    return 0;
}
