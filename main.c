#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")

#define BACKLOG_MAX 5 // Número máximo de conexões
#define BUFFER_SIZE 1024 // Tamanho máximo das mensagem recebidas
#define MENSAGEM_PARA_SAIR "#sair" // Mensagem para encerrar a conexão

// --------------------- Identificador do host ----------------------------------------------
char hostname[256];
struct hostent *host;
char *ip;

// --------------------- Identificador dos sockets local e remote -----------------------------
SOCKET local_socket;
SOCKET remote_socket = 0;
// ----------------------- Tamanho da estrutura de endereço remoto -----------------------------
int remote_length = 0;
// ---------------------- Tamanho da mensagem recebida ----------------------------------------
int message_length = 0;
// ----------------------- Buffer para armazenar as mensagens recebidas -----------------------
char message[BUFFER_SIZE];
// -------------------------- Portas para os sockets local e remoto
size_t local_port = 0;
size_t remote_port = 0;

// ------------------- Estruturas que armazenam os endereços locais e remotos ----------------
struct sockaddr_in local_address, remote_address;

// -------------- Estrutura usada pela função WSAStartup para inicializar o Winsock ----------
WSADATA wsa_data;

// ----------------- Função auxiliar que exibe uma mensagem de erro no console e encerra o programa --------------
void error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    system("PAUSE");
    exit(EXIT_FAILURE);
}

// -------------------- Função principal ------------------------------------------------------
int main(int argc, char**argv) {

    // ------------------- Inicializa o Winsock -----------------------------------------------
    printf("Inicizalizando Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        error("Falha ao inicializar :(\n");
    }

    //-------------------- Obtem nome do host ------------------------------------------------
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        WSACleanup();
        error("Falha ao obter hostname :(\n");
    }

    printf("Hostname: %s\n", hostname);

    // ------------------- Obtem informações do host -----------------------------------------
    host = gethostbyname(hostname);
    if (host == NULL) {
        WSACleanup();
        error("Falha ao obter informações do host :(\n");
    }

    // ------------------ Cria o socket local para o servidor --------------------------------
    local_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (local_socket == INVALID_SOCKET) {
        WSACleanup();
        error("Não foi possivel criar o socket principal.\n");
    }

    printf("Porta local: ");
    scanf("%d", &local_port);

    // ------------------ Zera a estrutura local address ------------------------------
    memset(&local_address, 0, sizeof(local_address)); // OBS: A função memset é usada para preencher uma área de memória com um valor específico. Neste caso, ela está sendo usada para inicializar a estrutura local_address com o valor zero em todos os seus bytes.
    local_address.sin_family = AF_INET; // Familia de endereços (AF_INET para IPv4)
    local_address.sin_addr.s_addr = INADDR_ANY; // Endereço IP do servidor (INADDR_ANY -> qualquer endereço disponível na maquina local 0.0.0.0)
    local_address.sin_port = htons(local_port); // Porta local em ordem de bytes de rede (big-endian), convertida por htons()


    printf("Socket principal criado! :)\n");

    // --------------- Interliga o socket com o endereço local -----------------------------------
    if (bind(local_socket, (struct sockaddr*)&local_address, sizeof(local_address)) == SOCKET_ERROR) {
        WSACleanup();
        closesocket(local_socket);
        error("Falha ao fazer a ligação do socket principal com o endereço local. :(\n");
    }

    printf("Ligação entre o socket principal e o endereço local realizado com sucesso! :)\n");

    // --------------------- Abre um ouvinte para o socket local começar a aguardar por conexões ---------------------
    if (listen(local_socket, BACKLOG_MAX) == SOCKET_ERROR) {
        WSACleanup();
        closesocket(local_socket);
        error("Algo deu errado com o listen");
    }

    remote_length = sizeof(remote_address);
    printf("Tudo certo! :)\n");
    ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
    printf("Endereço: %s:%llu\n", ip, local_port);
    printf("Aguardando por conexões...\n");

    remote_socket = accept(local_socket, (struct sockaddr*)&remote_address, &remote_length);
    if (remote_socket == INVALID_SOCKET) {
        WSACleanup();
        closesocket(local_socket);
        error("Falha ao aceitar a conexão! :(\n");
    }

    printf("Conexão estabelecida! :)\n");

    do {
        // -------------- Limpa o buffer ---------------------------------------
        memset(&message, 0, BUFFER_SIZE);

        // -------------------- recebe a mensagem do cliente -------------------
        message_length = recv(remote_socket, message, BUFFER_SIZE, 0);
        if (message_length == SOCKET_ERROR) {
            error("Falha no recebimento da mensagem! :(\n");
        }

        printf("%s: %s\n", inet_ntoa(remote_address.sin_addr), message);
    } while (strcmp(message, MENSAGEM_PARA_SAIR) == 0); // sai ao receber mensagem "#sair" do cliente

    printf("Encerrando\n");

    // ----------------------- Fecha o socket e limpa o Winsock --------------------------------
    WSACleanup();
    closesocket(local_socket);
    closesocket(remote_socket);

    system("PAUSE");
    return 0;
}