#include <stdio.h>      
#include <stdlib.h>    
#include <unistd.h>     
#include <string.h>     
#include <fcntl.h>      
#include <sys/types.h>  
#include <sys/wait.h>  
#include <signal.h>     

#define MAX_KOMUT_UZUNLUK 1024
#define MAX_ARGUMAN 100

void sinyal_isleyici(int sinyal);
void komut_calistir(char *komut);

int main() {
    char komut[MAX_KOMUT_UZUNLUK];

    while (1) {
        // Kullanıcıya bir komut istemi (prompt) göster
        printf("> ");
        fflush(stdout);

        // Kullanıcıdan bir satır komut oku (fgets boş satırı da alabilir)
        if (fgets(komut, MAX_KOMUT_UZUNLUK, stdin) == NULL) break;

        // Alınan komutun sonundaki '\n' karakterini temizle
        komut[strcspn(komut, "\n")] = 0;

        // Kullanıcı "quit" yazarsa programı sonlandır
        if (strcmp(komut, "quit") == 0) {
            break;
        }

        // Boş komut girilmediyse komutu çalıştır
        if (strlen(komut) > 0) {
            komut_calistir(komut);
        }
    }

    return 0;
}

// Çocuk süreçler öldüğünde tetiklenecek sinyal işleyici fonksiyon
// Bu fonksiyon, arka planda çalışıp biten süreçlerin dönüş değerlerini ekrana yazar
void sinyal_isleyici(int sinyal) {
    int durum;
    pid_t pid;

    // waitpid ile bekleyen tüm çocuk süreçleri al ve durumlarını yaz
    while ((pid = waitpid(-1, &durum, WNOHANG)) > 0) {
        printf("[pid %d] geri_donus_degeri: %d\n", pid, WEXITSTATUS(durum));
    }
}

// Girilen komutu ayrıştırıp çalıştıran fonksiyon
// Komut içinde '&' işareti varsa süreç arka planda çalıştırılır
void komut_calistir(char *komut) {
    char *argumanlar[MAX_ARGUMAN];
    int arka_plan = 0;  // Arka planda çalışma bayrağı
    int i = 0;

    // Komutu boşluklarına göre parçala
    char *parca = strtok(komut, " ");
    while (parca != NULL) {
        // Eğer '&' bulunursa komut arka planda çalışacak
        if (strcmp(parca, "&") == 0) {
            arka_plan = 1;
        } else {
            // Normal argümanları argumanlar dizisine ekle
            argumanlar[i++] = parca;
        }
        parca = strtok(NULL, " ");
    }
    argumanlar[i] = NULL; // Argüman dizisini NULL ile sonlandır

    // Yeni bir süreç oluştur (fork)
    pid_t pid = fork();
    if (pid == 0) {
        // Çocuk süreç: komutu çalıştır
        execvp(argumanlar[0], argumanlar);
        perror("Komut çalıştırılamadı");
        exit(1);
    } else if (pid > 0) {
        // Ebeveyn süreç
        if (!arka_plan) {
            // Arka plan değilse, çocuk sürecin bitmesini bekle
            waitpid(pid, NULL, 0);
        } else {
            // Arka planda ise SIGCHLD sinyalini yakala
            // Çocuk süreç bitince sinyal_isleyici devreye girecek
            signal(SIGCHLD, sinyal_isleyici);
        }
    } else {
        // fork başarısız oldu
        perror("Fork hatası");
    }
}
