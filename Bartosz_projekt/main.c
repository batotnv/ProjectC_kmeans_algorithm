#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//the defines which helps in getting length of an array

#define rowLength(array) (sizeof((array))/sizeof((array)[0]))
#define colLength(array) sizeof((array)[0])/sizeof((array)[0][0])

#define MAX_VARIABLES 10000

//kmeans parameters

#define number_clusters 3
#define MAX_ITERATIONS 5




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
 //   printf("obliczony dystans w funkcji: %f \n", dist);
    return dist;
}


// funkcja losująca liczby całkowite z przedziału <min, max>
int losowa(int min, int max) {

    int zakres = max - min + 1;
    int dzielnik = RAND_MAX / zakres;
    return min + (rand() / dzielnik);
}

//funkcja liczaca centroidy dla poszczegolnych klastrow

struct myArray calculate_centroid(int length, int column,struct myArray *array, int cluster){

    int i, j, k;
    int members = 0;
    struct myArray *c = malloc(sizeof(*c)*length);
    struct myArray *centroid = malloc(sizeof(*centroid));
    double mean = 0;

 //wybieranie wszystkich obserwacji nalezacych do danego klastra

    for (k = 0; k < length; k++){
        if(array[k].cluster == cluster){
            c[members] = array[k];
            members++;
        }
    }

//jesli nie ma zadnych obserwacji w tym klastrze, zwracam wektor z samymi zerami

    if(members == 0){
            for(i = 0; i < column; i++){
                centroid -> values[i] = 0;
            }

        return *centroid;
    }


//obliczanie centroidy dla danego klastra, srednia poszczegolnych obserwacji (srednia z wartosci po kolumnach)

    for (i = 0; i < column; i++){
        for(j = 0; j < members; j++){
            mean += c[j].values[i];
   //         printf("Wartosc: %f \n", c[j].values[i]);
            }

   //     printf("Suma: %f \n", mean);
        mean = mean/members;
        centroid -> values[i] = mean;
        mean = 0;
    }

    return *centroid;
}

//funkcja zwracaja najblizsza centroide dla danej obserwacji

int closest_centroid(struct myArray *centroids, struct myArray observation, int column){

    int i;
    double *distances = malloc(sizeof(*distances)*number_clusters);


    //obliczanie odleglosci euklidesowej pomiedzy obserwacja, a kazda z centroid

    for (i = 0; i < number_clusters; i++){
            distances[i] = euclidean_distance(observation, centroids[i], column);
     //       printf("obliczony dystans: %f \n", distances[i]);
    //        printf("Odleglosci pomiedzy centroida, a obserwacjami \n\n");
    //        printf("Odleglosc: %f \n", distances[i]);
    }

    //wybor klastra o najmniejszej odleglosci do obserwacji

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
 //   printf("NABLIZSZY KLASTER TO: %d \n\n",observation.cluster);

   return observation.cluster;
}



//wariancja wewnatrz klastrowa, czyli zsumowane odeglosci pomiedzy wszystkimi obserwacjami w danym klastrze
//i podzielone przez ilosc wszystkich obserwacji znajdujacych sie w nim

double within_cluster_variance(int columns, int length, struct myArray *observations, int cluster){

    printf("\nNOWE LICZENIE WITHIN CLUSTER \n\n");
     int i, j, k;
   // double distances[number_clusters];

 //   double *variance = malloc(sizeof(*variance));
 //   *variance = 0;
    double variance = 0;
    double sum = 0;
//    double *sum = malloc(sizeof(*sum));
 //   *sum = 0;
    int members = 0;
    double temp = 0;
    struct myArray *c = malloc(sizeof(*c)*length);

     //wybieranie wszystkich obserwacji nalezacych do danego klastra

    for (k = 0; k < length; k++){
        if(observations[k].cluster == cluster){
            c[members] = observations[k];
            members++;
        }
    }

 //   printf("MEMBERS %d \n", members);


//       //printing gotten data
//     for(k = 0; k < members; k++){
//        printf("Member nr %d \n",k);
//        for(j = 0; j < 5; j++){
//         //   printf("%f ", testArray[k][j]);
//
//            printf("%lf\n", c[k].values[j]);
//        }
//        printf("\n");
//    }
    //jesli w danym klastrze nie ma zadnych obserwacji to wariancja 0

    if(members == 0 || members == 1){
        return variance;
    }
    else{
        // petla liczaca odleglosci pomiedzy kazda z obserwacji

        for(i = 0; i < (members - 1); i++){
            for(j = i + 1 ; j < members; j++){
                temp = euclidean_distance(c[i], c[j], columns);
           //     printf("dodaje dystans miedzy czlonkami %d i %d %f \n", i, j, temp);
                sum += temp;
            }
        }

    //obliczenie wariancji
    //    printf("SUM OF MEMBERS DISTANCES: %f \n", sum);

        variance = sum/(double)members;
     //   printf("SUM AFTER DIVIDE: %f \n", variance);

        return variance;

    }

}




//funkcja zwracajaca ilosc wierszy w danym pliku

int count_rows(FILE *fp){
    int rows = 0;
    char line[MAX_VARIABLES];

        //counting rows in file
    if (!fp){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }
    else{
       // while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp,"%*c")))
        while(fgets(line, sizeof(line), fp) != NULL){
        rows++;
    }
    }

    printf("THIS TXT HAS %d ROWS \n", rows);

    return rows;
}

//funkcja zwracajaca ilosc kolumn w danym pliku

int count_columns(FILE *fp){
    int cols = 0;
    int ch;

        //counting lines in file

    if (!fp){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }
    else{

         while((ch= fgetc(fp))!= EOF) {
            if(ch == '\n')
                break;
            if(ch == ' ')
                ++cols;
                continue;
        }
    }

    printf("THIS TXT HAS %d COLS \n", cols+1);
    return cols+1;
}





int main()
{
    srand(time(NULL));
    int rows = 1;
    struct myArray *newArray = malloc(sizeof(*newArray)*rows);
    int i, j, k, columns;
    int a = 1;
    FILE *fp1, *fp2, *fpointer;
    double sum_variance_guard = 1000;

    fp1 = fopen("data.txt", "r");
    fp2 = fopen("data.txt", "r");
    fpointer = fopen("data.txt", "r");

    //int columns;
    rows = count_rows(fp1);
    columns = count_columns(fp2);

    //rows = 5;
    //columns = 5;
    //printf("length col: %d \n", columns);
    //printf("length rows: %d \n", rows);


    //returning error when program cannot open a file
    if (!fpointer){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }

    //getting data from file

    printf("POBIERANIE DANYCH Z PLIKU... \n \n");

    for( i = 0; i < rows; i++){
        for(j = 0; j < columns; j++){
            //fscanf(fpointer,"%lf", &testArray[i][j]);
            fscanf(fpointer,"%lf", &newArray[i].values[j]);
        }
    }


    printf("POBRANE DANE: \n");

     //printing gotten data
     for(k = 0; k < rows; k++){
        printf("Obserwacja nr %d \n",k);
        for(j = 0; j < columns; j++){
         //   printf("%f ", testArray[k][j]);

            printf("%lf\n", newArray[k].values[j]);
        }
        printf("\n");
    }

     printf("\n\n");
    printf("LOSOWE PRZYDZIELANIE KLASTROW \n");

        //test przydzielania
//    newArray[0].cluster = 1;
//    newArray[1].cluster = 2;
//    newArray[2].cluster = 3;
//    newArray[3].cluster = 1;
//    newArray[4].cluster = 2;
//    newArray[5].cluster = 3;


    //przydzielanie i printowanie losowych klastrow
    for(i = 0; i < rows; i++){
        newArray[i].cluster = losowa(1, number_clusters);
        printf("Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
    }

    printf("\n");




    //TUTAJ ZACZYNAŁABY SIĘ PĘTLA ITERACYJNA

    while(a <= MAX_ITERATIONS){

        printf("\n \n");
        printf("ZACZYNAM ITERACJE NR %d \n", a);
             //losowe przydzielanie klastrow



        printf("OBLICZANIE CENTROID DLA POSZCZEGOLNYCH KLASTROW... \n");

        //obliczanie centroid
        struct myArray *centroids = malloc(sizeof(*centroids)*number_clusters);

        for(i = 0; i < rows; i++){
            centroids[i] = calculate_centroid(rows, columns, newArray, i+1);
        }

        //printowanie centroid



        for(j = 0; j < number_clusters; j++){
            printf("CENTROIDA NR %d \n", j+1);
            for (i = 0; i < columns; i++){
                printf("%f \n", centroids[j].values[i]);
            }
            printf("\n");
        }

        //obliczanie odleglosci pomiedzt centroidami, a obserwacjami

        printf("OBLICZANIE ODLEGLOSCI POMIEDZY OBSERWACJAMI, A CENTROIDAMI... \n");
        printf("PRZYDZIELANIE DO NOWYCH, BLIZSZYCH KLASTROW \n \n");


        //przydzielenie nowego klastra danej obserwacji
        for(i = 0; i < rows; i++){
            newArray[i].cluster = closest_centroid(centroids, newArray[i], columns);
            printf("UPDATE Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
        }

    //    for(i = 0; i < rows; i++){
    //
    // //       newArray[i].cluster = losowa(1, number_clusters);
    //        printf("Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
    //    }

        //obliczanie wariancji wewnatrzklastrowych
        //within cluster raczej dziala dobrze
        printf("\n");
        printf("OBLICZANIE WARIANCJI WEWNATRZKLASTROWYCH... \n\n");

        double sum_variance = 0;
        double temp = 0;

        for(i = 1; i <= number_clusters; i++){

           temp = within_cluster_variance(columns, rows, newArray, i);
           printf("Wariancja wewnatrzklastrowa klastra %d wynosi: %f \n", i, temp);
           sum_variance += temp;
           printf("Suma wszystkich warianacji wewnatrzklastrowych wynosi: %f \n", sum_variance);

        }

    //kod sprawdzajacy czy jest minimum lokalne



        if((a == 1) || (sum_variance < sum_variance_guard)){

            sum_variance_guard = sum_variance;
            a++;
            printf("WARIANCJA NIZSZA! LECIMY DALEJ!\n");
            continue;
        }
        else if (sum_variance >= sum_variance_guard) {

            printf("MINIMUM LOKALNE! KONIEC ALGORYTMU! \n");
            break;
        }



    }

    //zrobic jakas przechowywalnie na lepszy wynik z poprzedniej iteracji
    // bo gdy sprawdzamy nowy to tracimy poprzednie?

    //PRINT ROZWIAZANIA
    printf("\n");
    printf("WYNIKI ALGORYTMU \n \n");

//         printing results
    for(k = 1; k <= number_clusters; k++){

        printf("KLASTER %d \n", k);
        for(j = 0; j < rows; j++){

            if(newArray[j].cluster == k){
                printf("Obserwacja nr %d \n", j);
            }
        }
        printf("\n");
    }


    fclose(fpointer);
    fclose(fp1);
    fclose(fp2);


    FILE *fpw;
    fpw = fopen("results.txt","w");

    for(k = 1; k <= number_clusters; k++){

        fprintf(fpw,"KLASTER %d \n", k);
        for(j = 0; j < rows; j++){

            if(newArray[j].cluster == k){

                fprintf(fpw, "Obserwacja nr %d \n", j);
            }
        }
        fprintf(fpw,"\n");
    }



    fclose(fpw);


    return 0;
}






























