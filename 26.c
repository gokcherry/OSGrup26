#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Sabitler
#define MAX_KOMUT_UZUNLUK 1024 // Maksimum komut uzunluğu
#define MAX_ARGUMAN 100        // Maksimum argüman sayısı

// Ana program
int main() {
    char komut[MAX_KOMUT_UZUNLUK]; // Kullanıcıdan alınan komutun saklanacağı dizi

    while (1) {
        // Komut istemi yazdırılır.
        printf("> ");
        fflush(stdout); // Çıktıyı hemen ekrana yazdır.

        // Kullanıcıdan komut alınır
        if (fgets(komut, MAX_KOMUT_UZUNLUK, stdin) == NULL) break;

        // Komuttan yeni satır karakteri (\n) kaldırılır.
        komut[strcspn(komut, "\n")] = 0;

        // "quit" komutu girilirse döngü sonlanır ve program biter.
        if (strcmp(komut, "quit") == 0) {
            break;
        }
    }

    return 0; // Program başarıyla sonlanır.
}

// Komut çalıştırma fonksiyonu
void komut_calistir(char *komut) {
    char *argumanlar[MAX_ARGUMAN];
    char *giris_dosyasi = NULL;                    // Giriş dosyası adı için değişken
    int i = 0;



    // Komut ve argümanları parçalar
    char *parca = strtok(komut, " ");
    while (parca != NULL) {
        if (strcmp(parca, "<") == 0) {
            giris_dosyasi = strtok(NULL, " ");     // "<" operatöründen sonra gelen dosya adını alır
        } else {
            argumanlar[i++] = parca;              // Argümanları diziye ekler
        }
        parca = strtok(NULL, " ");
    }
    argumanlar[i] = NULL;                         // Argüman dizisini sonlandırır

    if (giris_dosyasi) {
        pid_t pid = fork();                       // Yeni bir işlem oluşturur
        if (pid == 0) {                              // Çocuk işlem için
            int fd = open(giris_dosyasi, O_RDONLY);    // Giriş dosyasını okuma modunda açar
            if (fd < 0) {                                 
                perror("Giriş dosyası bulunamadı");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);            // Giriş dosyasını standart girdiyle değiştirir
            close(fd);
            execvp(argumanlar[0], argumanlar);        // Komutu çalıştırır
            perror("Komut çalıştırılamadı");
            exit(1);
        } else if (pid > 0) {                      // Ebeveyn işlem için
            waitpid(pid, NULL, 0);                   // Çocuk işlemin tamamlanmasını bekler
        } else {
            perror("Fork hatası");
        }
    }
}
}
