#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <iostream>
#include <random>
#include <thread>

#define LIMITE_X_MIN 2
#define LIMITE_X_MAX 31

#define LIMITE_Y_MIN 2
#define LIMITE_Y_MAX 31

#define LIMITE_TAMANHO_COBRA 30

#define SKIN_PAREDE 176

void gotoxy(int x, int y) {
    COORD pos;
    pos.X = (SHORT)(x - 1);
    pos.Y = (SHORT)(y - 1);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

typedef struct {
    int x;
    int y;
} pedaco;

int sizeCobra = 3;
pedaco corpo[20];
char cabecaCobra = '>';

bool ganhou = false;
bool perdeu = false;
bool comidaSpawnada = false;

static int direcao = 1;

std::mt19937 rng(std::random_device{}());

std::uniform_int_distribution<int> xAleatorio(LIMITE_X_MIN + 1, LIMITE_X_MAX - 1);
std::uniform_int_distribution<int> yAleatorio(LIMITE_Y_MIN + 1, LIMITE_Y_MAX - 1);

int xComida;
int yComida;

void validaBotaoColisao(int oldDirecao) {
    switch (direcao) {
        case 1:
            if (corpo[0].x + 1 == corpo[1].x) {
                direcao = oldDirecao;
            }
            break;
        case 2:
            if (corpo[0].x - 1 == corpo[1].x) {
                direcao = oldDirecao;
            }
            break;
        case 3:
            if (corpo[0].y - 1 == corpo[1].y) {
                direcao = oldDirecao;
            }
            break;
        case 4:
            if (corpo[0].y + 1 == corpo[1].y) {
                direcao = oldDirecao;
            }
            break;
    }
}

void thread_pegarDirecao() {
    while (!ganhou && !perdeu) {  // Loop principal do jogo
        if (_kbhit()) {
            char tecla = _getch();
            int old = direcao;
            switch (tecla) {
                case 'a':
                    direcao = 2;
                    break;
                case 'd':
                    direcao = 1;
                    break;
                case 'w':
                    direcao = 3;
                    break;
                case 's':
                    direcao = 4;
                    break;
            }
            validaBotaoColisao(old);
        }
        // printf("Minha thread");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void verificaColisoes() {
    // Colisões com paredes
    if (
        corpo[0].x >= LIMITE_X_MAX ||
        corpo[0].x <= LIMITE_X_MIN ||
        corpo[0].y <= LIMITE_Y_MIN ||
        corpo[0].y >= LIMITE_Y_MAX) {
        perdeu = true;
        return;
    }

    for (int i = 1; i < sizeCobra; i++) {
        if (corpo[0].x == corpo[i].x && corpo[0].y == corpo[i].y) {
            // printf("corpo[%d].x:%d\tcorpo[%d].y:%d\n", i, corpo[i].x, i, corpo[i].y);
            // i=0;
            // printf("corpo[%d].x:%d\tcorpo[%d].y:%d\n", i, corpo[i].x, i, corpo[i].y);
            perdeu = true;
            return;
        }
    }
}

void atualizarCobra() {
    gotoxy(corpo[sizeCobra - 1].x, corpo[sizeCobra - 1].y);
    printf(" ");
    // Movimentação da cobra
    for (int i = sizeCobra - 1; i > 0; i--) {
        corpo[i] = corpo[i - 1];
    }

    // 1: direita, 2: esquerda, 3: cima, 4: baixo
    switch (direcao) {
        case 1:
            corpo[0].x++;
            cabecaCobra = '>';
            break;
        case 2:
            corpo[0].x--;
            cabecaCobra = '<';
            break;
        case 3:
            corpo[0].y--;
            cabecaCobra = 'A';
            break;
        case 4:
            corpo[0].y++;
            cabecaCobra = 'v';
            break;
    }
}

void desenharCobra() {
    gotoxy(corpo[0].x, corpo[0].y);
    std::cout << cabecaCobra;
    for (int i = 1; i < sizeCobra; i++) {
        gotoxy(corpo[i].x, corpo[i].y);
        printf("o");
    }
}

void desenharParedes() {
    // Parede superior
    for (int i = LIMITE_X_MIN; i <= LIMITE_X_MAX; i++) {
        gotoxy(i, LIMITE_Y_MIN);
        printf("%c", SKIN_PAREDE);
    }
    // Parede inferior
    for (int i = LIMITE_X_MIN; i <= LIMITE_X_MAX; i++) {
        gotoxy(i, LIMITE_Y_MAX);
        printf("%c", SKIN_PAREDE);
    }
    // Parede esquerda
    for (int i = LIMITE_Y_MIN; i <= LIMITE_Y_MAX; i++) {
        gotoxy(LIMITE_X_MIN, i);
        printf("%c", SKIN_PAREDE);
    }
    // Parede direita
    for (int i = LIMITE_Y_MIN; i <= LIMITE_Y_MAX; i++) {
        gotoxy(LIMITE_X_MAX, i);
        printf("%c", SKIN_PAREDE);
    }
}

void esconderCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;  // Tamanho do cursor definido como 1 (mínimo)
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void verificarComeuComida() {
    if (corpo[0].x == xComida && corpo[0].y == yComida) {
        if (sizeCobra + 1 >= LIMITE_TAMANHO_COBRA) {
            ganhou = true;
            return;
        }
        comidaSpawnada = false;
        sizeCobra++;
        corpo[sizeCobra - 1] = corpo[sizeCobra - 2];
    }
}

void spawnComida() {
    if (comidaSpawnada) return;

    xComida = xAleatorio(rng);
    yComida = yAleatorio(rng);

    gotoxy(xComida, yComida);
    printf("\033[31mo");
    printf("\033[0m");
    comidaSpawnada = true;
}

int main() {
    for (int i = 0; i < sizeCobra; i++) {
        corpo[i].x = LIMITE_X_MIN + 1 + sizeCobra - i;
        corpo[i].y = LIMITE_Y_MIN + 1;
    }

    esconderCursor();
    std::thread thread_direcao(thread_pegarDirecao);
    system("cls");
    desenharParedes();

    while (1) {
        atualizarCobra();
        verificaColisoes();
        verificarComeuComida();
        spawnComida();
        if (ganhou || perdeu) break;
        desenharCobra();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    thread_direcao.join();

    system("cls");
    if (perdeu) {
        printf("Ruimzao");
    } else if (ganhou) {
        printf("Boa");
    }
    return 0;
}