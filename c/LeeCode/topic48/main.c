#include<stdio.h>

void rotate(int** matrix, int matrixSize, int* matrixColSize)
{
    int i = 0,j = 0;
    int nMatrixRowSize = *matrixColSize;
    int nTemp = 0;

    for(i = 0; i < nMatrixRowSize/2; i++)
    {
        for(j = 0; j < nMatrixRowSize; j++)
        {
            nTemp = *((int*)matrix + i*nMatrixRowSize + j);
            *((int*)matrix + i*nMatrixRowSize + j) = *((int*)matrix + j*nMatrixRowSize + i);
            *((int*)matrix + j*nMatrixRowSize + i) = nTemp;
        }
    }

    for(i = 0; i < nMatrixRowSize; i++)
    {
        for(j = 0; j < nMatrixRowSize / 2; j++)
        {
            nTemp = *((int*)matrix + i*nMatrixRowSize + j);
            *((int*)matrix + i*nMatrixRowSize + j) = *((int*)matrix + i*nMatrixRowSize + nMatrixRowSize - j - 1);
            *((int*)matrix + i*nMatrixRowSize + nMatrixRowSize - j - 1) = nTemp;
        }
    }
}

int main(void)
{
    int matrix[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
    int nColSize = 3;
    int i = 0,j = 0;
    int *pmatrix = matrix;

    rotate(matrix, sizeof(matrix), &nColSize);
    
    for(i = 0; i < nColSize; i++)
    {
        for(j = 0; j < nColSize; j++)
        {
            printf("%d\t",matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}

