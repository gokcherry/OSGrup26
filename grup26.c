#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_KOMUT_UZUNLUK 1024 // Maksimum uzunlukta komut saklanabilir
#define MAX_ARGUMAN 100 // Maksimum argüman sayýsý

// Sinyal iþleyici, tamamlanan arka plan iþlemleri hakkýnda bilgi verir.
void sinyal_isleyici(int sinyal) {
    int durum;
    pid_t pid;

    while ((pid = waitpid(-1, &durum, WNOHANG)) > 0) {
        printf("[pid %d] geri_donus_degeri: %d\n", pid, WEXITSTATUS(durum));
    }
}

void komut_calistir(char* komut) {
    char* argumanlar[MAX_ARGUMAN]; // Komut argümanlarýný saklamak için dizi tanýmlandý.
    int arka_plan = 0; // Komutun arka planda çalýþýp çalýþmadýðýný belirliyor.
    int i = 0;

    // Komut parçalanarak argümanlara ayrýlýr.
    char* parca = strtok(komut, " ");
    while (parca != NULL) {
        if (strcmp(parca, "&") == 0) {
            arka_plan = 1; // '&' operatörü arka plan çalýþmayý temsil eder
        }
        else {
            argumanlar[i++] = parca;
        }
        parca = strtok(NULL, " ");
    }
    argumanlar[i] = NULL; // Argüman dizisinin sonunu iþaretlemek için NULL eklenir

    pid_t pid = fork(); // Yeni bir süreç oluþturulur
    if (pid == 0) {
        execvp(argumanlar[0], argumanlar); // Komut çalýþtýrýlýr
        perror("Komut çalýþtýrýlamadý"); // Hata oluþursa bildirilir
        exit(1);
    }
    else if (pid > 0) {
        if (!arka_plan) {
            waitpid(pid, NULL, 0); // Arka planda deðilse ana süreç bekler
        }
        else {
            signal(SIGCHLD, sinyal_isleyici); // Arka planda ise sinyal iþleyici ayarlanýr
        }
    }
    else {
        perror("Fork hatasý"); // Fork baþarýsýz olduysa hata bildirilir
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
