#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_KOMUT_UZUNLUK 1024 // Maksimum uzunlukta komut saklanabilir
#define MAX_ARGUMAN 100 // Maksimum arg�man say�s�

// Sinyal i�leyici, tamamlanan arka plan i�lemleri hakk�nda bilgi verir.
void sinyal_isleyici(int sinyal) {
    int durum;
    pid_t pid;

    while ((pid = waitpid(-1, &durum, WNOHANG)) > 0) {
        printf("[pid %d] geri_donus_degeri: %d\n", pid, WEXITSTATUS(durum));
    }
}

void komut_calistir(char* komut) {
    char* argumanlar[MAX_ARGUMAN]; // Komut arg�manlar�n� saklamak i�in dizi tan�mland�.
    int arka_plan = 0; // Komutun arka planda �al���p �al��mad���n� belirliyor.
    int i = 0;

    // Komut par�alanarak arg�manlara ayr�l�r.
    char* parca = strtok(komut, " ");
    while (parca != NULL) {
        if (strcmp(parca, "&") == 0) {
            arka_plan = 1; // '&' operat�r� arka plan �al��may� temsil eder
        }
        else {
            argumanlar[i++] = parca;
        }
        parca = strtok(NULL, " ");
    }
    argumanlar[i] = NULL; // Arg�man dizisinin sonunu i�aretlemek i�in NULL eklenir

    pid_t pid = fork(); // Yeni bir s�re� olu�turulur
    if (pid == 0) {
        execvp(argumanlar[0], argumanlar); // Komut �al��t�r�l�r
        perror("Komut �al��t�r�lamad�"); // Hata olu�ursa bildirilir
        exit(1);
    }
    else if (pid > 0) {
        if (!arka_plan) {
            waitpid(pid, NULL, 0); // Arka planda de�ilse ana s�re� bekler
        }
        else {
            signal(SIGCHLD, sinyal_isleyici); // Arka planda ise sinyal i�leyici ayarlan�r
        }
    }
    else {
        perror("Fork hatas�"); // Fork ba�ar�s�z olduysa hata bildirilir
    }
}

int main() {
    char komut[MAX_KOMUT_UZUNLUK];

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(komut, MAX_KOMUT_UZUNLUK, stdin) == NULL) break;

        komut[strcspn(komut, "\n")] = 0;

        if (strcmp(komut, "quit") == 0) {
            break;
        }

        komut_calistir(komut);
    }

    return 0;
}
