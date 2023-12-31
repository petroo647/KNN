#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define ITEMS_ROW 1000
#define NUMB_USER_PROP 14

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

void fillUser(Utente*, char*);
char* getUserProp(int _CountUserProp, char* _UserProps, char* _Buf);
Utente* calcPunteggi(Utente** _UserList, Utente* _PropsDaClassificare, char* _NewData, int _Righe);
Utente setProps();

int main() {
    FILE* dataset_ptr = fopen("dataset_knn.csv", "r");
    char riga[ITEMS_ROW];
    char newData[] = "1,debt_consolidation,0.1426,171.53,11.40756495,15.6,727,7560,28532,99.4,1,0,0,0";
    Utente* usersList[10000];
    int righe = 0;
    Utente propsDaClassificare = setProps();
    Utente* propsDaClassificare_ptr = &propsDaClassificare;
    //int.rate, installment, log.annual.inc, dti, fico, days.with.cr.line, revol.bal, revol.util.

    if (dataset_ptr == NULL) {
        perror("Errore nell'apertura del file");
        exit(1);
    }

    for(int i = 0; dataset_ptr != NULL; i++){
        righe++;
        char* row = fgets(riga, ITEMS_ROW, dataset_ptr);
        //printf("%d riga: %s\n", i, row);
        if (row != NULL) {
            if(i == 1){
                continue;
            }
            Utente* ptr_nUser = (Utente*)malloc(sizeof(Utente));
            if(ptr_nUser == NULL){
                printf("Memoria allocata in modo errato");
                exit(1);
            }
            //printf("\n\n%d\n", i);
            fillUser(ptr_nUser, row);
            usersList[i] = ptr_nUser;
            //printf("indirizzo ptr: %p\n", ptr_nUser);
        } else {
            if(feof(dataset_ptr)){
                break;
            }
            perror("Errore nella lettura della riga");
        }
        
    }

    //devo calcolare la classifica per: int.rate, installment, log.annual.inc, dti, fico, days.with.cr.line, revol.bal, revol.util. Calcolando la distanza dal nuovo punto inserito
    calcPunteggi(usersList, propsDaClassificare_ptr, newData, righe);

    //faccio una prova e stampo solo le distanze di intRate
    /*for (int i = 0; i < 1000; i++){
        if(i < 3){
            continue;
        }

        double intRate = usersList[i]->distanze_ptr->dti;
        printf("intrate: %f\n", intRate);
    }*/

    fclose(dataset_ptr);
    return 0;
}

//*((&_User->creditPolicy)+i*(sizeof(char))) //mi consente di iterare le proprietà di una struttura essendo tutte char

void fillUser(Utente* _User, char* _UserProps){
    for(int i = 0; i < NUMB_USER_PROP; i++){
        char* buffer = (char*)malloc(100);
        if(buffer == NULL){
            printf("Memoria allocata in modo errato");
            exit(1);
        }
        *((&_User->creditPolicy)+i*(sizeof(char))) = getUserProp(i, _UserProps, buffer);
        //printf("\n\nINDIRIZZO e': %p\n", getUserProp(i, _UserProps, buffer));
    }
    _User->punteggio_ptr = (Punteggio*)malloc(sizeof(Punteggio));
    _User->distanze_ptr = (Distanze*)malloc(sizeof(Distanze));
    if(_User->punteggio_ptr == NULL || _User->distanze_ptr == NULL){
        printf("Memoria allocata in modo errato");
        exit(1);
    }
}

Utente* calcPunteggi(Utente** _UserList, Utente* _PropsDaClassificare, char* _NewData, int _Righe){
    for(int i = 0; i < NUMB_USER_PROP; i++){
        char* buffer = (char*)malloc(100);
        if(buffer == NULL){
            printf("Memoria allocata in modo errato");
            exit(1);
        }   
        char* propValue = *((&_PropsDaClassificare->creditPolicy)+i*(sizeof(char))); //to know se devo compararla
        if(propValue == "si"){
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
                //puts(newDataPropValue);
                char* endPtr1;
                char* endPtr2;
                Utente* currentUser_ptr = _UserList[j];
                
                char* currentPropValue = *((&currentUser_ptr->creditPolicy)+i*(sizeof(char)));
                double currentPropValue_d = strtod(currentPropValue, &endPtr1);
                double newDataPropValue_d = strtod(newDataPropValue, &endPtr2);
                double distance = currentPropValue_d - newDataPropValue_d;
                if(newDataPropValue_d > currentPropValue_d) distance = newDataPropValue_d - currentPropValue_d;
                printf("distance: %lf, currentPropValue: %lf, newDataPropValue: %lf, i: %d, newDataPropValueString: %s\n\n", distance, currentPropValue_d, newDataPropValue_d, i, newDataPropValue);
                if(distance < 0){
                    distance = distance * (-1);
                }
                *((&currentUser_ptr->distanze_ptr->intRate)+i*(sizeof(char))) = distance;
                //ho la distanza dalla prop del nuovo dato, il problema è che non so come mantenere questo valore e confrontarlo con gli altri
            }
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
    //printf("start index: %d, end index: %d, len given str: %d\n", startIndexUserProp, endIndexUserProp, lenghtUserProp);

    for(int i = 0; i < lenghtUserProp; i++){
        _Buf[i] = userProp[i];
    }
    _Buf[lenghtUserProp] = '\0';
    //printf("buf: %s\n", _Buf);
    return _Buf;
}
