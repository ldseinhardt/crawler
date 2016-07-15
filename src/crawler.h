#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include "HTTPClient.h"

#define MAX_THREADS 7

using namespace std;

class Crawler {
  private :
    string host;

    int depth;

    vector<URL*> urls;

    vector<thread*> threads;

    //Extrai de uma página (string) os links encontrados em tags <a> e <img>
    vector<string> ExtractLinks(string s);

    //Função executada por threads
    //Baixa uma página e empilha as urls para serem baixadas
    void visit(int depth, const URL& u);

  public :
    Crawler(int depth, string host);
};
