#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//the defines which helps in getting length of an array
#define rowLength(array) (sizeof((array))/sizeof((array)[0]))
#define colLength(array) sizeof((array)[0])/sizeof((array)[0][0])


//kmeans parameters

#define number_clusters 2
#define MAX_ITERATIONS 100


typedef struct
{
  double *values;
  int capacity;
} myArray;


//function that computes euclidean distance
double euclidean_distance(double *p, double *q, int n){

    double dist;
    int i;

    for(i = 0; i < n; i++){
        dist += (p[i]-q[i])*(p[i]-q[i]);
        printf("%lf \n", dist);
    }
    dist = sqrt(dist);
    return dist;
}


// funkcja losująca liczby całkowite z przedziału <min, max>
int losowa(int min, int max) {

    int zakres = max - min + 1;
    int dzielnik = RAND_MAX / zakres;
    return min + (rand() / dzielnik);
}

//funkcja przypisujaca kazdy wektor z matriksa do danego klastra
// tutaj cos trzeba pokombinowac zeby zwracalo macierz


//double **assign_to_clusters(int row, int col, double matrix[row][col]){
//
//    int i;
//  //  int **board = (int **)malloc(Rows * sizeof(int *));
//
//    for(i = 0; i < row; i++){
//
//        matrix[i][col+1] = losowa(1,2);
//    }
//
//    return matrix;
//}

void print_array(int row, int col, double matrix[row][col]){

    int i,j;

     for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

//double calculate_centroid()


int main()
{
    srand(time(NULL));
    int lineCount = 0;
    double testArray[3][5];
  //  char ch_arr[3][20];
    int i, j, k;

    FILE *fpointer = fopen("data.txt", "r");

    //returning error when program cannot open a file
    if (!fpointer){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }

    while( !feof(fpointer))
    {
        for( i = 0; i < 3; i++){
    //        fscanf(fpointer,"%ls,", &ch_arr[i]);
            for(j = 0; j < 5; j++){
                    fscanf(fpointer,"%lf,", &testArray[i][j]);
            }
        }
     }

    fclose(fpointer);

    for(k = 0; k < 3; k++){
        for(j = 0; j < 5; j++){
            printf("%f ", testArray[k][j]);
        }
        printf("\n");
    }


    //calculating lengths of matrix

    int length = rowLength(testArray);
    int column = colLength(testArray);

    printf("length col: %d \n", column);
    printf("length rows: %d \n", length);


    //double test1[] = {3, 4, 5, 7, 8};
    //double test2[] = {1, 3, 5, 6, 9};

    //przekazywanie poszczegolnych wierszy dziala dobrze
    printf("Euclidean dist: %lf", euclidean_distance(testArray[0],testArray[1],column));



//    double przydzielone[][] = assign_to_clusters(length, column, testArray)


 //   print_array(length, column, przydzielone);


    //printf("asdad: %f", elo[0]);

    return 0;
}






























