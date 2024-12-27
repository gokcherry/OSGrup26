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

void sinyal_isleyici();
void komut_calistir(char *komut);
