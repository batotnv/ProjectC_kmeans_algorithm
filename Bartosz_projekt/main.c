#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

//the defines which helps in getting length of an array
//#define rowLength(array) (sizeof((array))/sizeof((array)[0]))
//#define colLength(array) sizeof((array)[0])/sizeof((array)[0][0])

#define MAX_VARIABLES 100
#define MAX_OBSERVATIONS 100

//kmeans parameters
#define number_clusters 4
#define MAX_ITERATIONS 100
#define RANDOM_DRAWS 20

struct myArray{
  char observation_name[50];//nazwa obserwacji
  double values[MAX_VARIABLES];//miejsce na dane
  int cluster; //przyporzadkowany klaster
};

//function that computes euclidean distance
//p - 1st vector
//q - 2nd vector
//n - number of variables (dimensions)




double euclidean_distance(struct myArray p, struct myArray q, int columns){
    int i;
    double dist = 0;
    for(i = 0; i < columns; i++){
        dist += (p.values[i] - q.values[i]) * (p.values[i] - q.values[i]);
    }
    dist = sqrt(dist);
    return dist;
}

// funkcja losująca liczby całkowite z przedziału <min, max>
int random(int min, int max) {
    int range = max - min + 1;
    int divider = RAND_MAX / range;
    return min + (rand() / divider);
}

//funkcja liczaca centroidy dla poszczegolnych klastrow
struct myArray calculate_centroid(int rows, int columns,struct myArray *array, int cluster){
    int i, j, k;
    int members = 0;
    struct myArray *c = malloc(sizeof(*c)*rows);
    struct myArray centroid;
    double mean = 0;

 //wybieranie wszystkich obserwacji nalezacych do danego klastra
    for (k = 0; k < rows; k++){
        if(array[k].cluster == cluster){
            c[members] = array[k];
            members++;
        }
    }
//jesli nie ma zadnych obserwacji w tym klastrze, zwracam wektor z samymi zerami
    if(members == 0){
            for(i = 0; i < columns; i++){
                centroid.values[i] = 0;
            }
        return centroid;
    }
//obliczanie centroidy dla danego klastra, srednia poszczegolnych obserwacji (srednia z wartosci po kolumnach)
    for (i = 0; i < columns; i++){
        for(j = 0; j < members; j++){
            mean += c[j].values[i];
            }
        mean = mean/members;
        centroid.values[i] = mean;
        mean = 0;
    }
    return centroid;
}

//funkcja zwracaja najblizsza centroide dla danej obserwacji
int closest_centroid(struct myArray *centroids, struct myArray observation, int columns){

    int i;
    double *distances = malloc(sizeof(*distances)*number_clusters);

    //obliczanie odleglosci euklidesowej pomiedzy obserwacja, a kazda z centroid
    for (i = 0; i < number_clusters; i++){
            distances[i] = euclidean_distance(observation, centroids[i], columns);
    }

    //wybor klastra o najmniejszej odleglosci do obserwacji
    double minimum = distances[0];
    int index_min = 0;

    for (int i = 0; i < number_clusters; i++) {

        if (distances[i] < minimum){
            minimum = distances[i];
            index_min = i;
        }
    }
// +1 bo indeksowanie od 0, a klastry od 1
    observation.cluster = index_min + 1;

   return observation.cluster;
}

//wariancja wewnatrz klastrowa, czyli zsumowane odeglosci pomiedzy wszystkimi obserwacjami w danym klastrze
//i podzielone przez ilosc wszystkich obserwacji znajdujacych sie w nim

double within_cluster_variance(int columns, int length, struct myArray *observations, int cluster){

    int i, j, k;
    double variance = 0;
    double sum = 0;
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
    //jesli w danym klastrze nie ma zadnych obserwacji to wariancja 0
    if(members == 0 || members == 1){
        return variance;
    }
    else{
        // petla liczaca odleglosci pomiedzy kazda z obserwacji
        for(i = 0; i < (members - 1); i++){
            for(j = i + 1 ; j < members; j++){
                temp = euclidean_distance(c[i], c[j], columns);
                sum += temp;
            }
        }
    //obliczenie wariancji wewnatrzklastrowej
        variance = sum/(double)members;

        return variance;
    }

}

//funkcja zwracajaca ilosc wierszy w danym pliku
int count_rows(FILE *fp){

    int rows = 0;
    char line[MAX_OBSERVATIONS];

    if (!fp){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }
    else{
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

    if (!fp){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }
    else{
         while((ch = fgetc(fp))!= EOF) {
            if(ch == '\n')
                break;
            else if(ch == ' '){
                ++cols;
                continue;
            }
        }
    }
    printf("THIS TXT HAS %d COLS \n", cols+1);
    return cols+1;
}

//zapis do pliku
void write_to_file(FILE *fpw, struct myArray *array, int rows){

    int k, j;

    for(k = 1; k <= number_clusters; k++){
        fprintf(fpw,"KLASTER %d \n", k);
        for(j = 0; j < rows; j++){
            if(array[j].cluster == k){
                fprintf(fpw, "Obserwacja nr %d \n", j);
            }
        }
        fprintf(fpw,"\n");
    }
    printf("WYNIKI ZAPISANO DO PLIKU! \n\n");
}

//print do konsoli
void print_to_console(struct myArray *array, int rows){
    int k, j;

    for(k = 1; k <= number_clusters; k++){
        printf("KLASTER %d \n", k);
        for(j = 0; j < rows; j++){
            if(array[j].cluster == k){
                printf("Obserwacja nr %d \n", j);
            }
        }
        printf("\n");
    }
}

int main()
{
    srand(time(NULL));

    int i, j, k, columns, rows;
    int b = 1;
    int a = 1;

    FILE *fp1, *fp2, *fpointer;
    fp1 = fopen("data.txt", "r");
    fp2 = fopen("data.txt", "r");
    fpointer = fopen("data.txt", "r");

    rows = count_rows(fp1);
    columns = count_columns(fp2);

    fclose(fp1);
    fclose(fp2);

    struct myArray *newArray = malloc(sizeof(*newArray)*rows);
    struct myArray *backupArray = malloc(sizeof(*backupArray)*rows);
    struct myArray *resultsArray = malloc(sizeof(*resultsArray)*rows);

    //returning error when program cannot open a file
    if (!fpointer){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }

    //getting data from file
    printf("POBIERANIE DANYCH Z PLIKU... \n \n");

    for( i = 0; i < rows; i++){
        for(j = 0; j < columns; j++){
            fscanf(fpointer,"%lf", &newArray[i].values[j]);
        }
    }
    fclose(fpointer);

    printf("POBRANE DANE: \n");

     //printing gotten data
     for(k = 0; k < rows; k++){
        printf("Obserwacja nr %d \n",k);
        for(j = 0; j < columns; j++){
            printf("%lf\n", newArray[k].values[j]);
        }
        printf("\n");
    }

    //zadeklarowanie zmiennych, ktore beda kontrolowac wykonywanie algorytmu (sprawdzac czy wewnetrzna wariancja sie zmniejsza)
    double iterations_guard;
    double random_guard;

    for(i = 0; i < rows; i++){
        newArray[i].cluster = 1;
        }

    //wartoscia kontrolna bedzie wariancja wewnatrzklastrowa w przpypadku, gdy wszystkie obserwacje bylyby w jednym klastrze
    random_guard = within_cluster_variance(columns, rows, newArray, 1);
    iterations_guard = random_guard;

    //tutaj zaczyna sie petla, dzieki ktorej mozliwe bedzie kilkukrotnie losowanie klastrow
    while(b <= RANDOM_DRAWS){

    //printf("LOSOWANIE PRZYDZIELANIE KLASTRÓW nr %d\n\n", b);
    //pomocnicza do wewnetrznej petli iteracyjnej

        for(i = 0; i < rows; i++){
            newArray[i].cluster = random(1, number_clusters);
  //          printf("Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
        }

    //TUTAJ ZACZYNA SIĘ PĘTLA ITERACYJNA

        while(a <= MAX_ITERATIONS){

   //     printf("\n \n");
  //      printf("ZACZYNAM ITERACJE NR %d \n", a);
   //     printf("OBLICZANIE CENTROID DLA POSZCZEGOLNYCH KLASTROW... \n");

        //obliczanie centroid
        struct myArray *centroids = malloc(sizeof(*centroids)*rows);

        for(i = 0; i < rows; i++){
            centroids[i] = calculate_centroid(rows, columns, newArray, i+1);
        }

        //printowanie centroid
//        for(j = 0; j < number_clusters; j++){
//            printf("CENTROIDA NR %d \n", j+1);
//            for (i = 0; i < columns; i++){
//                printf("%f \n", centroids[j].values[i]);
//            }
//            printf("\n");
//        }

        //obliczanie odleglosci pomiedzt centroidami, a obserwacjami
   //     printf("OBLICZANIE ODLEGLOSCI POMIEDZY OBSERWACJAMI, A CENTROIDAMI... \n");
   //     printf("PRZYDZIELANIE DO NOWYCH, BLIZSZYCH KLASTROW \n \n");

        //przydzielenie nowego klastra danej obserwacji
        for(i = 0; i < rows; i++){
            newArray[i].cluster = closest_centroid(centroids, newArray[i], columns);
     //       printf("UPDATE Obserwacja nr %d nalezy do klastra %d \n", i, newArray[i].cluster);
        }

        //obliczanie wariancji wewnatrzklastrowych
  //      printf("\n");
    //    printf("OBLICZANIE WARIANCJI WEWNATRZKLASTROWYCH... \n\n");

        double sum_variance = 0;
        double temp = 0;

        for(i = 1; i <= number_clusters; i++){
           temp = within_cluster_variance(columns, rows, newArray, i);
       //    printf("Wariancja wewnatrzklastrowa klastra %d wynosi: %f \n", i, temp);
           sum_variance += temp;
      //     printf("Suma wszystkich warianacji wewnatrzklastrowych wynosi: %f \n", sum_variance);

        }

    //kod sprawdzajacy czy jest minimum lokalne
    //iterations guard - zmienna pomocnicza w sprawdzaniu wariancji w petli iteracyjnej

        if((a == 1) || ((sum_variance < iterations_guard) == true) ){
            iterations_guard = sum_variance;
            a++;
            backupArray = newArray;
   //         printf("WARIANCJA NIZSZA! LECIMY DALEJ!\n");
            continue;
        }
        else if (sum_variance >= iterations_guard) {
    //        printf("MINIMUM LOKALNE! KONIEC ALGORYTMU! \n");
            a = 1;
            break;
        }
    }

    if((b == 1) || ((iterations_guard < random_guard))){

        random_guard = iterations_guard;
        resultsArray = backupArray;
        printf("\n############################## \n");
        printf("NOWE NAJLEPSZE ROZWIAZANIE!");
        printf("\n############################## \n");
        printf("\nNOWA NAJLEPSZA SUMA WARIANCJI! : %f \n \n", random_guard);
        printf("WYNIKI ALGORYTMU \n \n");
        print_to_console(resultsArray, rows);

        //to file
        FILE *fpw;
        fpw = fopen("results.txt","w");
        write_to_file(fpw, resultsArray, rows);
        fclose(fpw);
       }

    b++;
    }

    return 0;
}






























