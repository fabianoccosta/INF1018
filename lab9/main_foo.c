#include <stdio.h>

void foo(int a[], int n);

int main(void) {
    int a[] = {1, 2, 0, 4, 5, 0, 3};
    int n = 7;

    printf("Array original: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    foo(a, n);

    printf("Array alterado:  ");
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");

    return 0;
}