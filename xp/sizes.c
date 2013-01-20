#include <stdio.h>
int main() {
    printf("long: %ld float: %ld double: %ld ldouble: %ld\n",
            sizeof(long),
            sizeof(float),
            sizeof(double),
            sizeof(long double));
    return 0;
}
