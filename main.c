#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")  // Winsock Library

int main() {
    WSADATA wsadata;
    SOCKET servidor, cliente;
    struct sockaddr_in servidor_addr, cliente_addr;
    int c, read_size;
    char client_message[2000];

    // -------------------- Inicializa o Winsock --------------------------------------
    printf("Inicializando Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsadata) != 0) {
        printf("Falha na inicialização. Código de erro: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Winsock inicializado.\n");

    // -------------------- Cria um socket ---------------------------------------------
    if ((servidor = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Não foi possível criar o socket. Código de erro: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Socket criado com sucesso.\n");

    // -------------------- Preenche a estrutura sockaddr_in ---------------------------
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(8888);

    // -------------------- Associa o socket ao endereço e porta especificados ---------
    if (bind(servidor, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr)) == SOCKET_ERROR) {
        printf("Falha ao fazer o bind. Código de erro: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Bind realizado com sucesso.\n");

    // ------------------- Escuta por conexões --------------------------------
    listen(servidor, 3);

    printf("Aguardando por conexões...\n");
    c = sizeof(struct sockaddr_in);

    // ------------------- Aceita uma conexão de cliente -----------------------
    cliente = accept(servidor, (struct sockaddr *)&cliente_addr, &c);
    if (cliente == INVALID_SOCKET) {
        printf("Falha ao aceitar a conexão. Código de erro: %d\n", WSAGetLastError());
        return 1;
    }
    printf("Conexão aceita.\n");

    // -------------------- Recebe dados do cliente ----------------------------
    while ((read_size = recv(cliente, client_message, 2000, 0)) > 0) {
        // Envia de volta os dados recebidos (eco)
        send(cliente, client_message, strlen(client_message), 0);
    }

    if (read_size == 0) {
        printf("Cliente desconectado.\n");
    } else if (read_size == SOCKET_ERROR) {
        printf("Erro ao receber dados. Código de erro: %d\n", WSAGetLastError());
    }

    // -------------------- Fecha o socket e limpa o Winsock --------------------
    closesocket(servidor);
    WSACleanup();

    return 0;
}
