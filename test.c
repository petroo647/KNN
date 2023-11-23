#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char* getUserProp(int _CountUserProp, char* _UserProps, char* _Buf);

/*typedef struct Persona {
    char*nome;
    char*cognome;
} Persona;*/

typedef struct Utente {
    char* creditPolicy;
    char* intRate;
    char* installment;
    char* logAnnualInc;
    char* dti;
    char* fico;// daysWithCrLine, revolBal, revolUtil, inqLastSixMonths, delinqTwoYrs, pubRec, notFullyPaid;
} Utente;

int main(){
    char stringa[] = "credit.policy,purpose,int.rate,installment,log.annual.inc,dti,fico,days.with.cr.line,revol.bal,revol.util,inq.last.6mths,delinq.2yrs,pub.rec,not.fully.paid"; // ho un segmentation fault per indice 5
    char extract[100];
    
    printf("la stringa estratta è: %s\n", getUserProp(0, stringa, extract));

    /*Persona luca;
    luca.nome = "Luca";
    luca.cognome = "Marzocchi";
    Persona* pers_ptr = &luca;
    *((&pers_ptr->nome)+sizeof(char)) = "Prova";
    Utente luca;

    luca.creditPolicy = "prova1";
    luca.intRate = "prova2";
    luca.installment = "prova3";
    luca.logAnnualInc = "prova4";
    luca.dti = "prova5";

    Utente* ptr_luca = &luca;

    char buffer[100];
    for(int i = 0; i < 5; i++){
        printf("%s\n", *((&ptr_luca->creditPolicy)+i*(sizeof(char))));
    }

    printf("%s\n", *((&ptr_luca->creditPolicy)+1*(sizeof(char))));

    //printf("%s", *((&pers_ptr->nome)+sizeof(char)));*/

    return 0;
}

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

    printf("start index: %d, end index: %d, len given str: %d\n", startIndexUserProp, endIndexUserProp, lenghtUserProp);
    for(int i = 0; i < lenghtUserProp; i++){
        _Buf[i] = userProp[i];
    }
    _Buf[lenghtUserProp] = '\0';
    return _Buf;
}
