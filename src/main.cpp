#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include <iostream>
#include <random>
#include <skins.hpp>
#include <thread>

#define LIMITE_X_MIN 2
#define LIMITE_X_MAX 31

#define LIMITE_Y_MIN 2
#define LIMITE_Y_MAX 31

#define LIMITE_TAMANHO_COBRA 30

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
char cabecaCobra = SKIN_CABECA_DIREITA;

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
    // Valida se a tecla pressionada vai bater no corpo
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
    // Pegar e validar teclas
    while (!ganhou && !perdeu) {
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

    // Colisão com o corpo
    for (int i = 1; i < sizeCobra; i++) {
        if (corpo[0].x == corpo[i].x && corpo[0].y == corpo[i].y) {
            perdeu = true;
            return;
        }
    }
}

void atualizarCobra() {
    // Tirar o ultimo caractere do rabo
    gotoxy(corpo[sizeCobra - 1].x, corpo[sizeCobra - 1].y);
    printf("%c", SKIN_VAZIO);

    // Movimentação da cobra
    for (int i = sizeCobra - 1; i > 0; i--) {
        corpo[i] = corpo[i - 1];
    }

    // 1: direita, 2: esquerda, 3: cima, 4: baixo
    switch (direcao) {
        case 1:
            corpo[0].x++;
            cabecaCobra = SKIN_CABECA_DIREITA;
            break;
        case 2:
            corpo[0].x--;
            cabecaCobra = SKIN_CABECA_ESQUERDA;
            break;
        case 3:
            corpo[0].y--;
            cabecaCobra = SKIN_CABECA_CIMA;
            break;
        case 4:
            corpo[0].y++;
            cabecaCobra = SKIN_CABECA_BAIXO;
            break;
    }
}

void desenharCobra() {
    // Desenha a cabeça
    gotoxy(corpo[0].x, corpo[0].y);
    std::cout << cabecaCobra;

    // Desenha o corpo da cobra
    for (int i = 1; i < sizeCobra; i++) {
        gotoxy(corpo[i].x, corpo[i].y);
        printf("%c", SKIN_CORPO);
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
    // Esconder o cursor pra não atrapalhar
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void verificarComeuComida() {
    // verifica se comeu a comidinha
    if (corpo[0].x == xComida && corpo[0].y == yComida) {
        // Se o tamanho da cobra já está no limite pra ganhar
        if (sizeCobra + 1 >= LIMITE_TAMANHO_COBRA) {
            ganhou = true;
            return;
        }
        // Seta para realizar o spawn de outra comida
        comidaSpawnada = false;

        // Aumenta o tamanho da cobra
        sizeCobra++;

        // O pedaço adicionado vai estar no mesmo espaço do ultimo
        corpo[sizeCobra - 1] = corpo[sizeCobra - 2];
    }
}

void spawnComida() {
    // Spawna a comida se n tiver
    if (comidaSpawnada) return;

    // Coord aleatorias limitadas para o spawn
    xComida = xAleatorio(rng);
    yComida = yAleatorio(rng);

    // Spawna a comida com cor diferente
    gotoxy(xComida, yComida);
    printf("%s%c", SKIN_COR_VERMELHO, SKIN_COMIDA);
    printf("%s", SKIN_COR_BRANCO);
    comidaSpawnada = true;
}

void apagarTela() {
    system("cls");
}

int main() {
    // Seta spawn inicial da cobra
    for (int i = 0; i < sizeCobra; i++) {
        corpo[i].x = LIMITE_X_MIN + 1 + sizeCobra - i;
        corpo[i].y = LIMITE_Y_MIN + 1;
    }

    esconderCursor();
    std::thread thread_direcao(thread_pegarDirecao);
    apagarTela();
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

    apagarTela();
    if (perdeu) {
        printf("Ruimzao");
    } else if (ganhou) {
        printf("Boa");
    }
    return 0;
}