#include <stdio.h>

int main(int argc, char *argv[]){
    int i, j, m, n, d;
    if(argc < 3){
        printf("./a.out rows cols density \n");
        return 0;
    }
    srand(NULL);
    m = atoi(argv[1]);
    n = atoi(argv[2]);
    d = atoi(argv[3]);
    printf("%d %d\n", m, n);
    for(i=0;i<m;i++)
    {
        for(j=0;j<n;j++){
            if(rand()%100 < d)
                printf("*");
            else
                printf(".");
        }
        printf("\n");
    }
    return 0;
}
