#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "poker.h"


User users[MAX_USERS];
int user_count =0;


static void test_hash_password(void **state) {
    (void)state;
    
    char password1[] = "test123";
    char hashed1[MAX_PASSWORD_LEN];
    char expected1[] = "uftu234";
    
    hash_password(password1, hashed1);
    assert_string_equal(hashed1, expected1);
    
}

static void test_load_users(void **state) {
    (void)state;
    
    FILE* file = fopen("users.txt", "w");
    assert_non_null(file);
    
    fprintf(file, "user1 pass1 1000\n");
    fprintf(file, "user2 pass2 2000\n");
    fprintf(file, "user3 pass3 3000\n");
    fclose(file);
    
    user_count = 0;
    memset(users, 0, sizeof(users));
    
    load_users();
    
    assert_int_equal(user_count, 3);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_hash_password),
        cmocka_unit_test(test_load_users),
    };
    
    return cmocka_run_group_tests(tests, NULL, NULL);
}