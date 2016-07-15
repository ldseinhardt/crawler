#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "url.h"

//#define DEBUG
#define FILE_PATH "www/"
#define SSL_CA_CERTIFICATE "cacert.pem"

using namespace std;

class HTTPClient {
  private :
    // Endereco
    URL* url;

    // HTTPS ?
    bool security;

    // Porta na qual se da a comunicação TCP após a conexão
    int sock;

    // Informações do SSL
    SSL *ssl;
    SSL_CTX *ctx;

    // Conexão TCP
    int TCPConnect();

    //Salva arquivos
    bool Save(string filename, const char* content, int size);

    //Pega o cabeçalho da resposta HTTP
    string ReadHeader();

    //Pega o corpo da resposta HTTP
    string ReadContent();

    //Pega o corpo da resposta HTTP, quando ja se sabe o tamanho, dado pelo Content-Length
    char* ReadContentSize(int size);

    //Monta e envia o pedido GET
    string SendGet();

  public :
    HTTPClient(string u);

    HTTPClient(const URL& u);

    ~HTTPClient();

    //Realiza conexão com o servidor HTTP
    bool Connect();

    //Função GET do HTTP, retorna a pagina se for HTML, caso contrário só salva
    string Get();
};
