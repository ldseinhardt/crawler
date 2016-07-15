#include <iostream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <algorithm>

using namespace std;

/* Quebra um link em informações de protocolo, host, porta, pasta, ...
 * http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
 */
class URL {
  public :
    // Endereco passado no GET do HTTP/HTTPS
    string url;
    // Protocolo (HTTP, HTTPS)
    string protocol;
    // DNS/IP
    string host;
    // Porta do TCP/IP
    int port;
    // Local onde será salvo o arquivo
    string path;
    // Nome do arquivo
    string file;
    // Argumentos GET, ?arg1=value1 & ...
    string query;

    // Construtor recebe uma string com endereço
    URL(string u);

    // Construtor de cópia que recebe outra URL
    URL(const URL &u);

    // Verifica se o endereço passado é o mesmo interno
    bool equals(string u);

    // Imprime o endereço na tela em partes
    void print();

    // Recebe o endereço
    string getURL();
};
