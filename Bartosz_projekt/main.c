#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//the defines which helps in getting length of an array
#define rowLength(array) (sizeof((array))/sizeof((array)[0]))
#define colLength(array) sizeof((array)[0])/sizeof((array)[0][0])


//kmeans parameters

#define number_clusters 3
#define MAX_ITERATIONS 1

struct myArray{
  char observation_name[50];//nazwa obserwacji
  double values[50];//miejsce na dane
  int cluster; //przyporzadkowany klaster
};



//function that computes euclidean distance
//p - 1st vector
//q - 2nd vector
//n - number of variables (dimensions)

double euclidean_distance(struct myArray p, struct myArray q, int n){

    double dist = 0;
    int i;

    for(i = 0; i < n; i++){
        dist += (p.values[i] - q.values[i]) * (p.values[i] - q.values[i]);
  //      printf("%lf \n", dist);
    }
    dist = sqrt(dist);
    printf("obliczony dystans w funkcji: %f \n", dist);
    return dist;
}


// funkcja losująca liczby całkowite z przedziału <min, max>
int losowa(int min, int max) {

    int zakres = max - min + 1;
    int dzielnik = RAND_MAX / zakres;
    return min + (rand() / dzielnik);
}



void print_array(int row, int col, double matrix[row][col]){

    int i,j;

     for(i = 0; i < row; i++){
        for(j = 0; j < col; j++){
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

//funkcja liczaca centroidy dla poszczegolnych klastrow

struct myArray calculate_centroid(int length, int column,struct myArray *array, int cluster){

    int i, j, k;
    int members = 0;
    struct myArray *c = malloc(sizeof(*c)*length);
 //jak to zadeklerowac dynamicznie?
  //  struct myArray c[50];
    struct myArray centroid;
    double mean = 0;

 //wybieranie wszystkich obserwacji nalezacych do danego klastra

    for (k = 0; k < length; k++){
        if(array[k].cluster == cluster){
            c[members] = array[k];
            members++;
        }
    }
//obliczanie centroidy dla danego klastra

    for (i = 0; i < column; i++){
        for(j = 0; j < members; j++){
            mean += c[j].values[i];
            printf("Wartosc: %f \n", c[j].values[i]);
            }

        printf("Suma: %f \n", mean);
        mean = mean/members;
        centroid.values[i] = mean;
        mean = 0;
    }
//funkcja zwracajaca index najmniejszej wartosci z danej listy
//int index_minimum(struct vector *v){
//
//
//
//
//}

////length = 3
////column = 5
//
//    for (i = 0; i < column; i++){
//        for(j = 0; j < length; j++){
//            mean += array[j].values[i];
//            printf("Wartosc: %f \n", array[j].values[i]);
//            }
//
//        printf("Suma: %f \n", mean);
//        mean = mean/length;
//        centroid.values[i] = mean;
//        mean = 0;
//    }
    return centroid;
}

int closest_centroid(struct myArray *centroids, struct myArray observation, int column){

    int i;
   // double distances[number_clusters];
    double *distances = malloc(sizeof(*distances)*number_clusters);

    for (i = 0; i < number_clusters; i++){
            distances[i] = euclidean_distance(observation, centroids[i], column);
            printf("obliczony dystans: %f \n", distances[i]);
    //        printf("Odleglosci pomiedzy centroida, a obserwacjami \n\n");
    //        printf("Odleglosc: %f \n", distances[i]);
    }

    double minimum = distances[0];
    int index_min = 0;

    for (int i = 0; i < number_clusters; i++) {
 //       printf("pierwsze minimum: %f \n", minimum);
        if (distances[i] < minimum)
        {
            minimum = distances[i];
            index_min = i;
        }
 //       printf("drugie minimum: %f \n", minimum);
  //      printf("Nowy numer klastra to: %d, \n", index_min);
    }

// +1 bo indeksowanie od 0, a klastry od 1
    observation.cluster = index_min + 1;
    printf("NABLIZSZY KLASTER TO: %d \n\n",observation.cluster);

   return observation.cluster;
}



//wariancja wewnatrz klastrowa, czyli zsumowane odeglosci pomiedzy wszystkimi obserwacjami w danym klastrze
//i podzielone przez ilosc wszystkich obserwacji znajdujacych sie w nim

double within_cluster_variance(int length, struct myArray *observations, int cluster){

     int i, j, k;
   // double distances[number_clusters];
    double variance = 0;
    double sum = 0;
    int members = 0;

    struct myArray *c = malloc(sizeof(*c)*length);

     //wybieranie wszystkich obserwacji nalezacych do danego klastra

    for (k = 0; k < length; k++){
        if(observations[k].cluster == cluster){
            c[members] = observations[k];
            members++;
        }
    }

    //jesli w danym klastrze nie ma zadnych obserwacji to wariancja 0

    if(members == 0){
        return variance;
    }
// petla liczaca odleglosci pomiedzy kazda z obserwacji

    for(i = 0; i < members; i++){
        for(j = 1; j < (members - i); j++){

            sum += euclidean_distance(c[i], c[i+j], members);
        }
    }

    //obliczenie wariancji

    variance = sum/members;

    return variance;

};


int count_rows(FILE *fp){
    int rows = 0;
    int ch;
        //counting rows in file

    if (!fp){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }
    else{
        while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
            ++rows;
    }

    printf("THIS TXT HAS %d ROWS \n", rows);

//    fclose(fp);
    return rows;
}

int count_columns(FILE *fp){
    int cols = 0;
    int ch;
        //counting lines in file

    if (!fp){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }
    else{
        //while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
         //   ++lines;

         while((ch=fgetc(fp))!=EOF) {
            if(ch=='\n')
                break;
            if(ch==' ')
                continue;
            ++cols;

        }
    }

    printf("THIS TXT HAS %d COLS \n", cols);

//    fclose(fp);
    return cols;
}





int main()
{
    srand(time(NULL));
    int lineCount = 1;
    double testArray[3][5];
  //  struct myArray newArray[lineCount];
    struct myArray *newArray = malloc(sizeof(*newArray)*lineCount);
    int i, j, k;
    double value = 0;
    int a = 0;
    FILE *fp1, *fp2, *fpointer;

    fp1 = fopen("data.txt", "r");
    fp2 = fopen("data.txt", "r");
    fpointer = fopen("data.txt", "r");

    int rows, columns;
    rows = count_rows(fp1);
    columns = count_columns(fp2);

    //rows = 6;
    //columns = 5;
    printf("length col: %d \n", columns);
    printf("length rows: %d \n", rows);


    //returning error when program cannot open a file
    if (!fpointer){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }

    //getting data from file

    for( i = 0; i < rows; i++){
        for(j = 0; j < columns; j++){
            //fscanf(fpointer,"%lf", &testArray[i][j]);
            fscanf(fpointer,"%lf", &newArray[i].values[j]);
        }
    }




     //printing gotten data
     for(k = 0; k < rows; k++){
        for(j = 0; j < columns; j++){
         //   printf("%f ", testArray[k][j]);

            printf("%lf\n", newArray[k].values[j]);
        }
        printf("\n");
    }

 //   fclose(fpointer);
//    fclose(fp1);
 //  fclose(fp2);

   // do{



    //calculating lengths of matrix

 //   int length = rowLength(myArray.values);
    //int length = 6;
    //int column = 5;
//    int column = colLength(myArray[0].values);




    //double test1[] = {3, 4, 5, 7, 8};
    //double test2[] = {1, 3, 5, 6, 9};

    //przekazywanie poszczegolnych wierszy dziala dobrze
    //printf("Euclidean dist: %lf", euclidean_distance(testArray[0],testArray[1],column));

    printf("Euclidean dist: %lf", euclidean_distance(newArray[0],newArray[1],columns));

//    double przydzielone[][] = assign_to_clusters(length, column, testArray)


 //   print_array(length, column, przydzielone);


    //printf("asdad: %f", elo[0]);

    //losowe przydzielanie klastrow

    printf("\n\n");
    newArray[0].cluster = 1;
    newArray[1].cluster = 2;
    newArray[2].cluster = 3;
    newArray[3].cluster = 1;
    newArray[4].cluster = 2;
    newArray[5].cluster = 3;

//    //przydzielanie i printowanie klastrow
//    for(i = 0; i < rows; i++){
//
//        newArray[i].cluster = losowa(1, number_clusters);
//        printf("Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
//    }



    //obliczanie centroid
    struct myArray *centroids = malloc(sizeof(*centroids)*number_clusters);

    for(i = 0; i < rows; i++){
        centroids[i] = calculate_centroid(rows, columns, newArray, i+1);
    }

    //printowanie centroid

    for(j = 0; j < number_clusters; j++){
        for (i = 0; i < columns; i++){
            printf("%f \n", centroids[j].values[i]);
        }
        printf("\n");
    }

    //obliczanie odleglosci pomiedzt centroidami, a obserwacjami

//    for(i = 0; i < length; i++){
//
//        closest_centroid(centroids, newArray[i], column);
//        printf("Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
//
//    }


    printf("Nowy klaster dla obserwacji: %d \n", closest_centroid(centroids, newArray[0], columns));
    printf("Obserwacja nr %d nalezy do klastra %d \n", 0, newArray[0].cluster);
    printf("Obserwacja nr %d nalezy do klastra %d \n", 3, newArray[3].cluster);

    //przydzielenie nowego klastra danej obserwacji
    for(i = 0; i < rows; i++){
       newArray[i].cluster = closest_centroid(centroids, newArray[i], columns);
       printf("UPDATE Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
    }

    for(i = 0; i < rows; i++){

 //       newArray[i].cluster = losowa(1, number_clusters);
        printf("Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
    }

    //within cluster raczej dziala dobrze
    double sum_variance = 0;

     for(i = 1; i <= number_clusters; i++){
       printf("Wariancja wewnatrzklastrowa klastra %d wynosi: %f \n", i, within_cluster_variance(rows, newArray, i));

       sum_variance += within_cluster_variance(rows, newArray, i);
       printf("Suma wszystkich warianacji wewnatrzklastrowych wynosi: %f \n", sum_variance);

    }





//napisac kod, ktory pozwoli na przechowywanie najlepszego dotychczasowego wyniku i po zakonczeniu petli bedzie trzeba go wyprintowac

//    double minimum = cluster[0];
//    int index_min = 0;
//
//    for (int i = 0; i < number_clusters; i++) {
//        printf("pierwsze minimum: %f \n", minimum);
//        if (distances[i] < minimum)
//        {
//            minimum = distances[i];
//            index_min = i;
//        }
//        printf("drugie minimum: %f \n", minimum);
//        printf("Nowy numer klastra to: %d, \n", index_min);


//    }
 //       a++;
//    }
 //   while(a < MAX_ITERATIONS);



    return 0;
}






























