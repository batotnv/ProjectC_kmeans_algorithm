#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_VARIABLES 100
#define MAX_OBSERVATIONS 100
#define FILENAME "data.txt"
#define number_clusters 6
#define MAX_ITERATIONS 100
#define RANDOM_DRAWS 50

/**
* @file main.c
* @author Bartosz Nguyen Van
* @date 04.11.2020
* @brief Glowny plik projektu
*
*\mainpage Opis projektu:
* Implementacja algorytmu k-srednich w jezyku C.
* Skrotowy opis programu:
* Program pozwala przeprowadzic analize skupien dla n-obserwacji p-wymiarowych przy uzyciu algorytmu k-srednich. Stosowana odlegloscia w napisanym algorytmie jest odleglosc euklidesowa. Przy uruchamianiu algorytmu mozliwe jest
* ustalenie nastepujacych parametrow: liczby klastrow, liczby iteracji oraz liczby wykonan algorytmu (pozwala to na zmniejszenie ryzyka otrzymania wyniku minimum lokalnego, gdyz obserwacje do klastrow przydzielane sa losowo).
* Program obsługuje wczytywanie danych oraz zapisywanie wynikoww w plikach o rozszerzeniu .txt.
*/

/** struktura przechowujaca dana obserwacje oraz numer klastru do ktorego ona nalezy */
struct myArray{
  double values[MAX_VARIABLES];
  int cluster;
};

/**
 @brief euclidean_distance
  Funkcja to oblicza odleglosc euklidesowa pomiedzy dwoma obserwacjami (wektorami, gdy mamy wiele wymiarow).
 \param p - pierwsza obserwacja
 \param q - druga obserwacja
 \param columns - liczba wymiarow (zmiennych)
 */

double euclidean_distance(struct myArray p, struct myArray q, int columns){
    int i;
    double dist = 0;
    for(i = 0; i < columns; i++){
        dist += (p.values[i] - q.values[i]) * (p.values[i] - q.values[i]);
    }
    dist = sqrt(dist);
    return dist;
}

/**
 @brief random
  Funkcja losujaca liczby calkowite z przedzialu domknietego <min, max>.
 \param min - wartosc minimalna dla przedzialu
 \param max - wartosc maksymalna dla przedzialu
 */
int random(int min, int max) {
    int range = max - min + 1;
    int divider = RAND_MAX / range;
    return min + (rand() / divider);
}

/**
 @brief calculate_centroid
  Funkcja obliczajaca centroide dla obserwacji znajdujacych sie w danym klastrze. Centroida to srednia arytmetyczna dla poszczegolnych zmiennych tych obserwacji (wektor, jesli zmiennych jest wiecej niz 1)
 \param rows - liczba obserwacji
 \param columns - liczba wymiarow
 \param array - tablica z obserwacjami
 \param cluster - numer klastra dla ktorego liczymy centroide
 */

struct myArray calculate_centroid(int rows, int columns,struct myArray *array, int cluster){
    int i, j, k;
    int members = 0;
    struct myArray *c = malloc(sizeof(*c)*rows);
    struct myArray centroid;
    double mean = 0;

/** wybor obserwacji nalezacych do klastra */
    for (k = 0; k < rows; k++){
        if(array[k].cluster == cluster){
            c[members] = array[k];
            members++;
        }
    }
/** jesli w danym klastrze nie ma obserwacji to zwracany jest wektor z samymi zerami */
    if(members == 0){
        for(i = 0; i < columns; i++){
            centroid.values[i] = 0;
        }
        return centroid;
    }
/**obliczanie centroidy*/
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

/**
 @brief closest_centroid
  Funkcja zwracajaca numer najblizszej centroidy dla danej obserwacji.
 \param centroids - tablica ze wszystkimi centroidami
 \param observation - obserwacja
 \param columns - liczba wymiarow
 */
int closest_centroid(struct myArray *centroids, struct myArray observation, int columns){

    int i;
    double *distances = malloc(sizeof(*distances)*number_clusters);

/** obliczanie odleglosci euklidesowej pomiedzy obserwacja, a kazda z centroid */
    for (i = 0; i < number_clusters; i++){
        distances[i] = euclidean_distance(observation, centroids[i], columns);
    }

/** wybor klastra o najmniejszej odleglosci do obserwacji */
    double minimum = distances[0];
    int index_min = 0;

    for (int i = 0; i < number_clusters; i++) {
        if (distances[i] < minimum){
            minimum = distances[i];
            index_min = i;
        }
    }
/** +1 bo indeksowanie od 0, a klastry numerowane sa od 1*/
    observation.cluster = index_min + 1;
    return observation.cluster;
}

/**
 @brief within_cluster_variation
  Funkcja obliczajaca zmiennosc wewnatrzklastrowa w danym klastrze, tj. zsumowane odleglosci pomiedzy wszystkimi obserwacjami i podzielone przez ich liczbe.
 \param columns - liczba wymiarow
 \param rows - liczba obserwacji
 \param observations - macierz z obserwacjami
 \param cluster - numer klastra
 */

double within_cluster_variation(int columns, int rows, struct myArray *observations, int cluster){

    int i, j, k;
    double variation = 0;
    double sum = 0;
    int members = 0;
    double temp = 0;
    struct myArray *c = malloc(sizeof(*c)*rows);

/** wybieranie wszystkich obserwacji nalezacych do danego klastra*/
    for (k = 0; k < rows; k++){
        if(observations[k].cluster == cluster){
            c[members] = observations[k];
            members++;
        }
    }
/**jesli w danym klastrze nie ma zadnych obserwacji to wariancja 0*/
    if(members == 0 || members == 1){
        return variation;
    }
    else{
/**petla liczaca odleglosci pomiedzy kazda z obserwacji*/
        for(i = 0; i < (members - 1); i++){
            for(j = i + 1 ; j < members; j++){
                temp = euclidean_distance(c[i], c[j], columns);
                sum += temp;
            }
        }
/**obliczenie zmiennosci wewnatrzklastrowej*/
        variation = sum/(double)members;
        return variation;
    }
}

/**
 @brief count_rows
  Funkcja zwracajaca ile wierszy (obserwacji) znajduje sie w danym pliku.
 \param fp - sprawdzany plik
 */
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
    return rows;
}

/**
 @brief count_columns
  Funkcja zwracajaca ile kolumn (zmiennych) znajduje sie w danym pliku.
 \param fp - sprawdzany plik
 */
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
            else if(ch == ','){
                ++cols;
                continue;
            }
        }
    }
    return cols+1;
}

/**
 @brief write_to_file
  Funkcja zapisujaca wyniki do pliku.
 \param fpw - uzywany plik
 \param array - macierz z obserwacjami
 \param rows - liczba obserwacji
 */
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

/**
 @brief print_to_console
  Funkcja wyswietlajca uzyskane wyniki w konsoli.
 \param array - macierz z obserwacjami
 \param rows - liczba obserwacji
 */
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

/**FUNKCJA MAIN */
int main()
{
    /**Dzieki temu ziarno programu za kazdym razem bedzie inne*/
    srand(time(NULL));

    /**Deklaracja zmiennych pomocniczych*/
    int i, j, columns, rows;
    int b = 1;
    int a = 1;

    /**Zadeklarowanie zmiennych, ktore beda kontrolowac wykonywanie algorytmu (sprawdzac czy wewnetrzna zmiennosc sie zmniejsza)*/
    double iterations_guard;
    double random_guard;

    /**Wczytywanie pliku*/
    FILE *fp1, *fp2, *fpointer;
    fp1 = fopen(FILENAME, "r");
    fp2 = fopen(FILENAME, "r");
    fpointer = fopen(FILENAME, "r");

    /**Obliczenie ilosc obserwacji i zmiennych*/
    rows = count_rows(fp1);
    columns = count_columns(fp2);

    fclose(fp1);
    fclose(fp2);
    /**Dynamiczna deklaracja uzywanych struktur*/
    struct myArray *newArray = malloc(sizeof(*newArray)*rows);
    struct myArray *backupArray = malloc(sizeof(*backupArray)*rows);
    struct myArray *resultsArray = malloc(sizeof(*resultsArray)*rows);

    /**Sprawdzanie czyli plik sie otworzyl prawidlowo*/
    if (!fpointer){
        fprintf (stderr, "Failed to open file!\n");
        return 1;
    }

    /**Pobieranie danych z pliku*/
    printf("POBIERANIE DANYCH Z PLIKU... \n \n");
    for( i = 0; i < rows; i++){
        for(j = 0; j < columns; j++){
            fscanf(fpointer,"%lf,", &newArray[i].values[j]);
        }
    }
    fclose(fpointer);

    /**GLOWNA PETLA
    tutaj zaczyna sie petla, dzieki ktorej mozliwe bedzie kilkukrotnie losowanie klastrow*/
    while(b <= RANDOM_DRAWS){
        printf("LOSOWANIE PRZYDZIELANIE KLASTRÓW nr %d\n\n", b);
    /**Losowe przydzielanie obserwacji do klastrow*/
        for(i = 0; i < rows; i++){
            newArray[i].cluster = random(1, number_clusters);
        }

    /**TUTAJ ZACZYNA SIĘ PĘTLA ITERACYJNA
    szukany jest najlepszy mozliwy podzial obserwacji na skupienia, gdzie punktem poczatkowym jest wylosowanie klastrow b
    */
        while(a <= MAX_ITERATIONS){
            printf("ZACZYNAM ITERACJE NR %d \n", a);
        /**Deklaracja struktury przechowujacej centroidy*/
            struct myArray *centroids = malloc(sizeof(*centroids)*rows);
        /**Obliczanie centroid*/
            for(i = 0; i < rows; i++){
                centroids[i] = calculate_centroid(rows, columns, newArray, i+1);
            }
        /**Przydzielanie obserwacji do nowych, najblizszych klastrow */
            for(i = 0; i < rows; i++){
                newArray[i].cluster = closest_centroid(centroids, newArray[i], columns);
            }
        /**Deklracja zmiennych pomocniczych przy obliczaniu zmiennosci*/
            double sum_variation = 0;
            double temp = 0;
        /**Obliczanie zmiennosci dla kazdego z klastrow*/
            for(i = 1; i <= number_clusters; i++){
               temp = within_cluster_variation(columns, rows, newArray, i);
               sum_variation += temp;
               printf("Suma zmiennosci klastra %i wynosi: %f \n", i, sum_variation);
            }

        /**Sprawdzanie czy natrafiono na minimum lokalne, jesli nie to petla wykonuje sie dalej*/
        /**iterations_guard - zmienna pomocnicza kontrolujaca zmiennosc*/
        /**backupArray zapisuje najlepszy do tej pory podzial na klastry*/
        /** (a == 1) zeby bylo pierwsze wyniki zapisaly sie i bylo do czego porownywac*/
         /**Zmiennosc sie zmniejsza -> continue*/
            if((a == 1) || (sum_variation < iterations_guard) ){
                iterations_guard = sum_variation;
                a++;
                backupArray = newArray;
                continue;
            }
            /**Zmiennosc pozostala taka sama lub sie zwiekszyla sie -> koniec petli*/
            /** a = 1 by zaczynala sie od nowa dla nowego b*/
            else if (sum_variation >= iterations_guard) {
                a = 1;
                break;
            }
        }
        /**Warunek pozwalajacy na zapisywanie nowego najlepszego wyniku (podzialu), gdy takowy sie pojawi */
        /**Wynik jest pokazywany w konsoli, a takze zapisywany do pliku results.txt*/
        if((b == 1) || ((iterations_guard < random_guard))){
            random_guard = iterations_guard;
            resultsArray = backupArray;
            printf("\n############################## \n");
            printf("NOWE NAJLEPSZE ROZWIAZANIE!");
            printf("\n############################## \n");
            printf("\nNOWA NAJLEPSZA SUMA WARIANCJI! : %f \n \n", random_guard);
            printf("WYNIKI ALGORYTMU \n \n");
            print_to_console(resultsArray, rows);

            /** zapisywanie wynikow do pliku*/
            FILE *fpw;
            fpw = fopen("results.txt","w");
            write_to_file(fpw, resultsArray, rows);
            fclose(fpw);
            }
            /**kolejna iteracja glownej petli...*/
            b++;
    }


    return 0;
}






























