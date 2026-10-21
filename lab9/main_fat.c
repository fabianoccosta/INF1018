#include <stdio.h>

int fat(int n);

int main(void) {
    int valores[] = {0, 1, 5, 7};
    int n = sizeof(valores) / sizeof(valores[0]);

    for (int i = 0; i < n; i++) {
        printf("fat(%d) = %d\n", valores[i], fat(valores[i]));
    }

    return 0;
}