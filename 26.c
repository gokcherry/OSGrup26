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
    char *argumanlar[MAX_ARGUMAN]; // Komutun argümanlarının saklanacağı dizi
    int i = 0;

    // Komut boşluklara göre parçalanır
    char *parca = strtok(komut, " ");
    while (parca != NULL) {
        argumanlar[i++] = parca; // Her parça argüman dizisine eklenir.
        parca = strtok(NULL, " "); // Bir sonraki parçaya geçilir.
    }
    argumanlar[i] = NULL; // Argüman dizisi null ile sonlandırılır.

    // Eğer komut boş ise fonksiyon sonlanır.
    if (argumanlar[0] == NULL) return;

    // Yeni bir süreç (child process) oluşturulur.
    pid_t pid = fork();
    if (pid == 0) {
        // Çocuk süreçte komut çalıştırılır.
        execvp(argumanlar[0], argumanlar);
        // Komut çalıştırılamazsa hata mesajı yazdırılır.
        perror("Komut çalıştırılamadı");
        exit(1); // Çocuk süreç hata ile sonlanır.
    } else if (pid > 0) {
        // Ana süreç, çocuk sürecin tamamlanmasını bekler.
        waitpid(pid, NULL, 0);
    } else {
        // fork() başarısız olursa hata mesajı yazdırılır.
        perror("Fork hatası");
    }
}
