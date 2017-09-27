// TRA-harjoitustyö, C-kielinen toteutustapa: Hash-taulu ja sanojen lukumäärät
// Mikko Rouru

// Käytetyt kirjastot

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

unsigned long int laskuri = 0;

typedef int (*vertailufunktio)(const void*, const void*);

// Struct-rakenteet missä määritellään hashtaulut sekä yleisimpien sanojen taulukko

typedef struct lista {
    char *string;
    unsigned long int arvo;
    struct lista *seuraava;
} lista;

typedef struct hashtaulu {
    unsigned long int koko;
    lista **taulu;
} hashtaulu;

typedef struct yleisin {
    char string[100];
    unsigned long int arvo;
} yleisin;

// Funktiot

hashtaulu *luoHashtaulu(unsigned long int);                // Hashtaulun luonti
int hash(hashtaulu *hashtaulu, char *sana);                // Hashfunktio
lista *etsi_string(hashtaulu *hashtaulu, char *str);       // Etsii sanalle paikan, jos on jo varattu otetaan seuraava
int lisaa_string(hashtaulu *hashtaulu, char *str);         // Sanan tauluunlisäysfunktio
int vertaa(yleisin *e1, yleisin *e2);                      // Qsort-funktiolle sanojen vertailufunktio
void tyhjennaTaulu(hashtaulu *hashtaulu);                  // Hash-taulun tyhjentäjä

// Main, missä tiedoston kysely, avaus, funktioiden kutsut,
// sadan yleisimmän sanan tulostus sekä tiedoston sulkeminen

int main(int argc, char** argv) {
   
    char tiedostonimi[100];
    char sana[100];
    char c;
    int sanat = 1;
    
    FILE *tiedosto;
    
// Kysytään tiedoston nimeä käyttäjältä
// Lasketaan tiedostosta sanojen määrä
    
    if (argc == 2) {
        tiedosto = fopen(argv[1], "r+");
    }
    else {
        do {
            printf("Syota tiedoston nimi ilman .txt-paatetta: \n");
            scanf("%s", tiedostonimi);
            strcat(tiedostonimi, ".txt");
            tiedosto = fopen(tiedostonimi, "r+");
        } while (tiedosto == 0);
    }
    while((c = fgetc(tiedosto)) != EOF) {
        if(c == ' ')
            sanat++;
    }
    
// Tulostetaan sanojen lukumäärä, tätä määrää käytetään myös hash-arvojen luonnissa
// Luodaan samalla hashtaulu funktion kautta, kooksi tulee sanojen määrä
    
    printf("Sanojen lukumaara = %d\n", sanat);
    rewind(tiedosto);
    
    laskuri = sanat;
    hashtaulu *sanataulu;
    sanataulu = luoHashtaulu(laskuri);
    
// Käydään läpi sanat tiedostosta ja lisätään hash-tauluun
// Otettu huomioon tehtävän vaatimukset: luetaan vain aakkoset ja heittomerkki
    
    while (!feof(tiedosto)) {
        
        fscanf(tiedosto, "%s", sana);
        
        char tempSana[100];
        
        int i = 0;
        int c = 0;
        for (i = 0; i < strlen(sana); i++) {
            if ((isalpha(sana[i])) || (sana[i] == '\'' || sana[i] == 39)) {
                tempSana[c] = sana[i];
                c++;
            }
        }
        tempSana[c] = '\0';
        lisaa_string(sanataulu, tempSana);
    }
 
// Lopuksi lasketaan taulusta sata yleisintä sanaa ja tulostetaan ne
// Käytetty apuna qsort-funktiota

    yleisin yleisin[99];
    lista *lista;
    int loydetytSanat =0;
    
    for (int i = 0; i<laskuri; i++) {
        
        for (lista = sanataulu->taulu[i]; lista != NULL; lista = lista->seuraava) {
            
            if (loydetytSanat<100) {
                strcpy(yleisin[loydetytSanat].string, lista->string);
                yleisin[loydetytSanat].arvo= lista->arvo;
                loydetytSanat = loydetytSanat + 1;
                qsort((void *) &yleisin, 100, sizeof(struct yleisin), (vertailufunktio)vertaa);
            }
            
            else if (lista->arvo > yleisin[0].arvo) {
                strcpy(yleisin[0].string, lista->string);
                yleisin[0].arvo = lista->arvo;
                qsort((void *) &yleisin, 100, sizeof(struct yleisin), (vertailufunktio)vertaa);
            }
        }
    }

    for(int i =99; i>0; i--) {
        printf("%s %lu \n", yleisin[i].string, yleisin[i].arvo);
    }
 
// Tyhjennetään hash-taulu ja suljetaan tiedosto
    
    tyhjennaTaulu(sanataulu);
    int fclose(FILE *tiedosto);
    
    return 0;
}

// Hash-taulun luontifunktio
// Käytetään apuna malloc-funktiota joka luo muistipaikan hash-taululle

hashtaulu *luoHashtaulu(unsigned long int koko) {
    
    hashtaulu *uusitaulu;
    int i;
    
    if (koko < 1) return NULL;
    
    if ((uusitaulu = malloc(sizeof(hashtaulu))) == NULL) {
        return NULL;
    }
    
    if ((uusitaulu->taulu = malloc(sizeof(lista*) * koko)) == NULL) {
        return NULL;
    }
    
    for(i=0; i < koko; i++) uusitaulu->taulu[i] = NULL;
    
    uusitaulu->koko = koko;
    
    return uusitaulu;
}

// Hash-funktio mikä luo sanalle oman hash-arvon.
// Korotetaan seuraavaa kirjaimen arvoa aina yhtä suuremmalla potenssilla,
// jotta vältetään törmäyksiä.
// Samalla muutetaan jokainen sana pieniksi kirjaimiksi.

int hash(hashtaulu *hashtaulu, char *sana) {
    
    int i;
    int j;
    int summa = 0;
    
    for (i = 0; i < strlen(sana); i++) {
        sana[i] = tolower(sana[i]);
    
        int temp = 1;
        
        for (j = 0; j < i; j++) {
            temp = temp * 64;
        }
        
    summa = summa + sana[i] * temp;
    }
    
    int tulos = 0;
    tulos = summa;
    summa = 0;
    
    return tulos % hashtaulu->koko;
}

// Hakee paikan hash-taulukosta sanalle

lista *etsi_string(hashtaulu *hashtaulu, char *str) {
    
    lista *lista;
    int hasharvo = hash(hashtaulu, str);
    
    for (lista = hashtaulu->taulu[hasharvo]; lista != NULL; lista = lista->seuraava) {
        if (strcmp(str, lista->string) == 0) return lista;
    }
    
    return NULL;
}

// Lisää sanan hash-taulukkoon

int lisaa_string(hashtaulu *hashtaulu, char *str) {
    lista *uusilista;
    lista *nykyinenlista;
    int hasharvo = hash(hashtaulu, str);
    
    if ((uusilista = malloc(sizeof(lista))) == NULL) return 1;
    
    nykyinenlista = etsi_string(hashtaulu, str);
    
    if (nykyinenlista != NULL) {
        nykyinenlista->arvo = nykyinenlista->arvo+1;
        return 0;
    };
    
    uusilista->string = strdup(str);
    uusilista->arvo = 1;
    uusilista->seuraava = hashtaulu->taulu[hasharvo];
    hashtaulu->taulu[hasharvo] = uusilista;
    
    return 0;
}

// Vertailufunktio qsort-funktiota varten

int vertaa(yleisin *e1, yleisin *e2) {
    if (e1->arvo < e2->arvo)
        return -1;
    
    else if (e1->arvo > e2->arvo)
        return 1;
    
    else
        return 0;
}

// Hash-taulun tyhjennysfunktio ohjelman lopuksi

void tyhjennaTaulu(hashtaulu *hashtaulu) {
        int i;
        lista *lista, *temp;
        
        if (hashtaulu==NULL) return;
        
        for (i=0; i < hashtaulu->koko; i++) {
            lista = hashtaulu->taulu[i];
            
            while (lista != NULL) {
                temp = lista;
                lista = lista->seuraava;
                free(temp->string);
                free(temp);
            }
            
        }
        free(hashtaulu->taulu);
        free(hashtaulu);
}
