#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

// Tanımlar
#define MAX_KOMUT_UZUNLUK 1024  // Maksimum komut uzunluğu
#define MAX_ARGUMAN 100         // Maksimum argüman sayısı

// Çocuk işlemler için sinyal işleyici
void sinyal_isleyici(int sinyal) {
    int durum;
    pid_t pid;

    // Bitmiş çocuk işlemleri kontrol et
    while ((pid = waitpid(-1, &durum, WNOHANG)) > 0) {
        printf("[pid %d] geri_donus_degeri: %d\n", pid, WEXITSTATUS(durum));
    }
}

// Komutu analiz eden ve çalıştıran fonksiyon
void komut_calistir(char *komut) {
    char *argumanlar[MAX_ARGUMAN];   // Komut argümanlarını tutar
    char *giris_dosyasi = NULL;      // Giriş dosyasını tutar
    char *cikis_dosyasi = NULL;      // Çıkış dosyasını tutar
    int arka_plan = 0;               // Arka plan işlemi kontrolü

    // Komut metnini ayrıştır
    int i = 0;
    char *parca = strtok(komut, " ");
    while (parca != NULL) {
        if (strcmp(parca, "<") == 0) {
            giris_dosyasi = strtok(NULL, " ");  // Giriş dosyasını ayarla
        } else if (strcmp(parca, ">") == 0) {
            cikis_dosyasi = strtok(NULL, " "); // Çıkış dosyasını ayarla
        } else if (strcmp(parca, "&") == 0) {
            arka_plan = 1;  // Arka plan işlemi olarak işaretle
        } else {
            argumanlar[i++] = parca;  // Argüman listesine ekle
        }
        parca = strtok(NULL, " ");
    }
    argumanlar[i] = NULL; // Argüman listesini sonlandır

    if (argumanlar[0] == NULL) return; // Boş komut kontrolü

    if (strcmp(argumanlar[0], "cikis") == 0) {
        exit(0); // Kullanıcı "cikis" yazarsa programdan çık
    }

    pid_t pid = fork(); // Yeni işlem oluştur

    if (pid == 0) {
        // Çocuk işlem
        if (giris_dosyasi) {
            // Giriş dosyasını stdin'e yönlendir
            int fd = open(giris_dosyasi, O_RDONLY);
            if (fd < 0) {
                perror("Giriş dosyası hatası");
                exit(1);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (cikis_dosyasi) {
            // Çıkış dosyasını stdout'a yönlendir
            int fd = open(cikis_dosyasi, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Çıkış dosyası hatası");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Komutu çalıştır
        execvp(argumanlar[0], argumanlar);
        perror("Çalıştırma başarısız"); // Eğer execvp başarısız olursa hata mesajı
        exit(1);
    } else if (pid > 0) {
        // Ebeveyn işlem
        if (!arka_plan) {
            // Eğer işlem arka planda değilse, çocuk işlemin tamamlanmasını bekle
            waitpid(pid, NULL, 0);
        }
    } else {
        perror("Fork başarısız"); // Fork başarısız olduysa hata mesajı
    }
}

// Ana fonksiyon
int main() {
    signal(SIGCHLD, sinyal_isleyici); // Çocuk işlemler için sinyal işleyici kur
    char komut[MAX_KOMUT_UZUNLUK];   // Kullanıcıdan alınan komut

    while (1) {
        printf("> "); // Komut istemi
        fflush(stdout);

        // Kullanıcıdan komut al
        if (fgets(komut, MAX_KOMUT_UZUNLUK, stdin) == NULL) break;

        // Satır sonu karakterini kaldır
        komut[strcspn(komut, "\n")] = 0;
        komut_calistir(komut); // Komutu çalıştır
    }

    return 0;
}
