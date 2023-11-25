#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define ITEMS_ROW 1000
#define NUMB_USER_PROP 14
#define NUMB_DISTANCES_PROP 8

typedef struct Punteggio {
    int intRate;
    int installment;
    int logAnnualInc;
    int dti;
    int fico;
    int daysWithCrLine;
    int revolBal;
    int revolUtil;
} Punteggio;

typedef struct Distanze {
    double intRate;
    double installment;
    double logAnnualInc;
    double dti;
    double fico;
    double daysWithCrLine;
    double revolBal;
    double revolUtil;
} Distanze;

typedef struct Utente {
    char* creditPolicy;
    char* purpose;
    char* intRate;
    char* installment;
    char* logAnnualInc;
    char* dti;
    char* fico;
    char* daysWithCrLine;
    char* revolBal;
    char* revolUtil;
    char* inqLastSixMonths;
    char* delinqTwoYrs;
    char* pubRec;
    char* notFullyPaid;
    Punteggio* punteggio_ptr;
    Distanze* distanze_ptr;
} Utente;

typedef Utente** Classifica;

typedef struct Classifiche {
    Classifica intRate;
    Classifica installment;
    Classifica logAnnualInc;
    Classifica dti;
    Classifica fico;
    Classifica daysWithCrLine;
    Classifica revolBal;
    Classifica revolUtil;
} Classifiche;

void calcClassifiche(Utente** _UserList, int _SizeUserList, Classifiche* _CTotali);
void fillUser(Utente*, char*);
char* getUserProp(int _CountUserProp, char* _UserProps, char* _Buf);
Utente* calcDistanze(Utente** _UserList, Utente* _PropsDaClassificare, char* _NewData, int _Righe);
Utente setProps();
void initClassifiche(Classifiche*, int _Size);

int main() {
    FILE* dataset_ptr = fopen("dataset_knn.csv", "r");
    char riga[ITEMS_ROW];
    char newData[] = "1,debt_consolidation,0.1426,171.53,11.40756495,15.6,727,7560,28532,99.4,1,0,0,0";
    Utente* usersList[10000];
    int righe = 0;
    Utente propsDaClassificare = setProps();
    Utente* propsDaClassificare_ptr = &propsDaClassificare;
    Classifiche cTotali;
    Classifiche* cTotali_ptr = &cTotali;

    if (dataset_ptr == NULL) {
        perror("Errore nell'apertura del file");
        exit(1);
    }

    for(int i = 0; dataset_ptr != NULL; i++){
        righe++;
        char* row = fgets(riga, ITEMS_ROW, dataset_ptr);
        if (row != NULL) {
            if(i == 1){
                continue;
            }
            Utente* ptr_nUser = (Utente*)malloc(sizeof(Utente));
            if(ptr_nUser == NULL){
                printf("Memoria allocata in modo errato");
                exit(1);
            }
            fillUser(ptr_nUser, row);
            usersList[i] = ptr_nUser;
        } else {
            if(feof(dataset_ptr)){
                break;
            }
            perror("Errore nella lettura della riga");
        }
        
    }
    calcDistanze(usersList, propsDaClassificare_ptr, newData, righe);
    //mi manca da calcolare il punteggio
    calcClassifiche(usersList, righe-100, cTotali_ptr);


    // provo a stampare un po di valori delle classifiche
    //printf("%p", cTotali_ptr->intRate[10][10]);


    //prova di stampa delle prop
    /*for (int i = 0; i < 9000; i++){
        if(i < 3){
            continue;
        }

        double propDistance = usersList[i]->distanze_ptr->dti;
        printf("distanza: %lf\n", propDistance);
    }*/

    fclose(dataset_ptr);
    return 0;
}
/*void initClassifiche(Classifiche* _Classifiche, int _Size){
    for(int i = 0; i < _Size; i++){
        Classifica* currentClassifica_ptr = (&_Classifiche->intRate)+i*sizeof(Classifica);
    }
}*/

//*((&_User->creditPolicy)+i*(sizeof(char))) //mi consente di iterare le proprietà di una struttura essendo tutte char
void calcClassifiche(Utente** _UserList, int _SizeUserList, Classifiche* _CTotali){
    for(int i = 0; i < NUMB_DISTANCES_PROP; i++){
        Classifica* currentClassifica_ptr = (Classifica*)malloc(sizeof(Classifica));
        currentClassifica_ptr = (&_CTotali->intRate)+i*(sizeof(Classifica));

        (*currentClassifica_ptr) = (Utente**)malloc((_SizeUserList - 2) * sizeof(Utente*));
        if ((*currentClassifica_ptr) == NULL) {
            printf("Memoria allocata in modo errato");
            exit(1);
        }

        for (int j = 2; j < _SizeUserList; j++) {
            (*currentClassifica_ptr)[j - 2] = (Utente*)malloc(sizeof(Utente));
            if ((*currentClassifica_ptr)[j - 2] == NULL) {
                printf("Memoria allocata in modo errato");
                exit(1);
            }
        }
        //adesso la classifica la riempio e basta, poi la ordino dal piu piccolo al piu grande
        
        for(int j = 2; j < _SizeUserList; j++){
            Utente* currentUser = _UserList[j];
            (*currentClassifica_ptr)[j-2] = currentUser;
            //printf("%p %d\n", (*currentClassifica_ptr)[j-2], i);
        }

        //ora ordino le singole classifiche
        for(int n = 2; n < _SizeUserList-100; n++){
            for(int j = 2; j < _SizeUserList-100; j++){
                Utente* currentUser1 = (*currentClassifica_ptr)[j - 2];
                double currentUserDistance1 = *((&currentUser1->distanze_ptr->intRate)+i*(sizeof(double)));
                Utente* currentUser2 = (*currentClassifica_ptr)[j - 1];
                double currentUserDistance2 = *((&currentUser2->distanze_ptr->intRate)+i*(sizeof(double)));

                if(currentUserDistance1 > currentUserDistance2){
                    Utente* tmp = (*currentClassifica_ptr)[j - 2];
                    (*currentClassifica_ptr)[j - 2] = (*currentClassifica_ptr)[j - 1];
                    (*currentClassifica_ptr)[j - 1] = tmp;
                }
            }
        }
    }
    for (int n = 2; n < _SizeUserList-100; n++){
        printf("%s\n", _CTotali->intRate[n]->intRate);
    }
}

/*
typedef struct Classifiche_totali {
    Utente** intRate;
    Utente** installment;
    Utente** logAnnualInc;
    Utente** dti;
    Utente** fico;
    Utente** daysWithCrLine;
    Utente** revolBal;
    Utente** revolUtil;
} Classifiche_totali;
*/

void fillUser(Utente* _User, char* _UserProps){
    for(int i = 0; i < NUMB_USER_PROP; i++){
        char* buffer = (char*)malloc(100);
        if(buffer == NULL){
            printf("Memoria allocata in modo errato");
            exit(1);
        }
        *((&_User->creditPolicy)+i*(sizeof(char))) = getUserProp(i, _UserProps, buffer);
    }
    _User->punteggio_ptr = (Punteggio*)malloc(sizeof(Punteggio));
    _User->distanze_ptr = (Distanze*)malloc(sizeof(Distanze));
    if(_User->punteggio_ptr == NULL || _User->distanze_ptr == NULL){
        printf("Memoria allocata in modo errato");
        exit(1);
    }
}

Utente* calcDistanze(Utente** _UserList, Utente* _PropsDaClassificare, char* _NewData, int _Righe){
    int yesCounter = 0;
    for(int i = 0; i < NUMB_USER_PROP; i++){  
        char* propValue = *((&_PropsDaClassificare->creditPolicy)+i*(sizeof(char))); //to know se devo compararla
        char si[] = "si";
        if(strcmp(propValue, si) == 0){
            for(int j = 0; j < _Righe - 100; j++){
                if(j < 2){
                    continue;
                }
                char* buffer = (char*)malloc(100);
                if(buffer == NULL){
                    printf("Memoria allocata in modo errato");
                    exit(1);
                }
                char* newDataPropValue = getUserProp(i, _NewData, buffer);
                char* endPtr1;
                char* endPtr2;
                Utente* currentUser_ptr = _UserList[j];
                
                char* currentPropValue = *((&currentUser_ptr->creditPolicy)+i*(sizeof(char)));
                double currentPropValue_d = strtod(currentPropValue, &endPtr1);
                double newDataPropValue_d = strtod(newDataPropValue, &endPtr2);
                double distance = currentPropValue_d - newDataPropValue_d;
                if(newDataPropValue_d > currentPropValue_d) distance = newDataPropValue_d - currentPropValue_d;

                if(distance < 0){
                    distance = distance * (-1);
                }
                *((&currentUser_ptr->distanze_ptr->intRate)+yesCounter*(sizeof(char))) = distance;
            }
            yesCounter++;
        }
    }
}

Utente setProps(){
    Utente _PropsDaClassificare;
    _PropsDaClassificare.creditPolicy = "no";
    _PropsDaClassificare.purpose = "no";
    _PropsDaClassificare.intRate = "si";
    _PropsDaClassificare.installment = "si";
    _PropsDaClassificare.logAnnualInc = "si";
    _PropsDaClassificare.dti = "si";
    _PropsDaClassificare.fico = "si";
    _PropsDaClassificare.daysWithCrLine = "si";
    _PropsDaClassificare.revolBal = "si";
    _PropsDaClassificare.revolUtil = "si";
    _PropsDaClassificare.inqLastSixMonths = "no";
    _PropsDaClassificare.delinqTwoYrs = "no";
    _PropsDaClassificare.pubRec = "no";
    _PropsDaClassificare.notFullyPaid = "no";
    return _PropsDaClassificare;
}

/*Restituisce la proprietà richiesta inserendo la sua posizione misurata in virgole.*/
char* getUserProp(int _CountUserProp, char* _UserProps, char* _Buf){
    int commaCount = 0;
    char c;
    int startIndexUserProp = 0;
    int endIndexUserProp;
    int lenghtUserProp = 0;

    for(int i = 0; commaCount < _CountUserProp; i++){
        if(_UserProps[i] == '\n'){
            break;
        }
        if(_UserProps[i] == ','){
            commaCount++;
        }
        startIndexUserProp++;
    }
    
    int n = startIndexUserProp;
    while(_UserProps[n] != ',' && n < strlen(_UserProps)){
        n++;
    }
    endIndexUserProp = n - 1;
    lenghtUserProp = endIndexUserProp - startIndexUserProp + 1;
    char userProp[lenghtUserProp];

    //extract prop
    for(int i = 0; i < lenghtUserProp; i++) {
        userProp[i] = _UserProps[startIndexUserProp + i];
    }
    for(int i = 0; i < lenghtUserProp; i++){
        _Buf[i] = userProp[i];
    }
    _Buf[lenghtUserProp] = '\0';
    return _Buf;
}
