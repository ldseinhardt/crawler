#include "HTTPClient.h"

HTTPClient :: HTTPClient(const URL& u) {
  url = new URL(u);

  security = !url->protocol.compare("HTTPS");
  sock = 0;
  ssl = NULL;
  ctx = NULL;
}

HTTPClient :: HTTPClient(string u) {
  url = new URL(u);

  security = !url->protocol.compare("HTTPS");
  sock = 0;
  ssl = NULL;
  ctx = NULL;
}

HTTPClient :: ~HTTPClient() {
  delete url;

  // Desconecta TCP
  if (sock) {
    close(sock);
  }

  // Desconecta SSL
  if (security) {
    if (ssl) {
      SSL_shutdown(ssl);
      SSL_free(ssl);
    }

    if (ctx) {
      SSL_CTX_free(ctx);
    }
  }
}

int HTTPClient :: TCPConnect() {
  int handle;
  if ((handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    return 0;
  }

  struct hostent *he;
  if (!(he = gethostbyname(url->host.c_str()))) {
    return 0;
  }

  struct sockaddr_in remote;
  remote.sin_family = AF_INET;
  remote.sin_port = htons(url->port);
  remote.sin_addr = *((struct in_addr*)he->h_addr);
  memset(&(remote.sin_zero), '\0', 8);

  if (connect(handle, (struct sockaddr*)&remote, sizeof(struct sockaddr)) == -1) {
    return 0;
  }

  return handle;
}

bool HTTPClient :: Connect() {
  if ((sock = TCPConnect()) == 0) {
    return false;
  }

  if (security) {
    // Carrega as strings de erro para libcrypto e libssl
    SSL_load_error_strings();

    // Inicializa a biblioteca ssl
    SSL_library_init();

    // Novo contexto para o cliente utilizando ssl 2 ou 3
    if (!(ctx = SSL_CTX_new(SSLv23_client_method()))) {
      ERR_print_errors_fp(stderr);
      return false;
    }

    // Carrega os certificados das autoridades certificadoras
    if (!SSL_CTX_load_verify_locations(ctx, SSL_CA_CERTIFICATE, NULL)) {
      ERR_print_errors_fp(stderr);
      return false;
    }

    // Cria a estrututa de conexão para o ssl
    if (!(ssl = SSL_new(ctx))) {
      ERR_print_errors_fp(stderr);
      return false;
    }

    // Conecta a estrutura ssl a conexão tcp corrente
    if (!SSL_set_fd(ssl, sock)){
      ERR_print_errors_fp(stderr);
      return false;
    }

    // Realiza a conexão ssl
    if (SSL_connect(ssl) != 1) {
      ERR_print_errors_fp(stderr);
    }

    cout << "Requisição: \e[1;34m" << url->getURL() << "\e[0m" << endl;

    // Pega a verificação do certificado
    long verify = SSL_get_verify_result(ssl);
    if (verify == X509_V_OK) {
      cout << "\e[1;32mHTTPS Ok\e[0m" << endl;
    } else {
      if ((verify == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT) || (verify == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)) {
        cout << "\e[1;33mHTTPS Certificado autoassinado\e[0m" << endl;
      } else {
        cout << "\e[1;31mHTTPS Erro (" << verify << ") no certificado\e[0m" << endl;
        return false;
      }
    }

    // Pega o certificado
    X509 *cert;
    if ((cert = SSL_get_peer_certificate(ssl))) {
      cout << "Certificado: " << endl;

      // Pega o dono do certificado: Organização (O)
      int organizationName_loc;
      if ((organizationName_loc = X509_NAME_get_index_by_NID(X509_get_subject_name((X509 *) cert), NID_organizationName, -1))) {
        X509_NAME_ENTRY* corganizationName_entry;
        if ((corganizationName_entry = X509_NAME_get_entry(X509_get_subject_name((X509 *) cert), organizationName_loc))) {
          char* organizationName;
          if ((organizationName = (char *) ASN1_STRING_data(X509_NAME_ENTRY_get_data(corganizationName_entry)))) {
            cout << "\tOrganização (O): \e[1;34m" << organizationName << "\e[0m" << endl;
          }
        }
      }

    /* Pega todas informações do dono do certificado
      char* name = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
      cout << name << endl;
      free(name);
    */
      X509_free(cert);
    } else {
      cout << "\e[1;33mCertificado não encontrado\e[0m" << endl;
    }
  } else {
      cout << "Requisição: \e[1;34m" << url->getURL() << "\e[0m" << endl;
  }

  cout << "-------------------------------------------------------------------------------" << endl;

  return true;
}

bool HTTPClient :: Save(string filename, const char* content, int size) {
  string url = filename;
  size_t p;
  string folder = "";
  struct stat s;

  while ((p = url.find("/")) != string::npos) {
    folder += url.substr(0, p+1);
    url = url.substr(p+1, url.length()-1-(p+1));
    if (stat(folder.c_str(), &s) == -1)
      mkdir(folder.c_str(), S_IRWXU);
  }

  FILE *fp;
  if ((fp = fopen(filename.c_str(), "wb")) != NULL) {
    fwrite(content, size, 1, fp);
    fclose(fp);
    return true;
  }
  return false;
}

string HTTPClient :: ReadHeader() {
  string result = "";
  char buffer[2] = {0};
  while (true) {
    int r = security
      ? SSL_read(ssl, buffer, 1)
      : recv(sock, buffer, 1, 0);
    if (r <= 0) {
      return result;
    }
    result += buffer;
    if (result.rfind("\r\n\r\n") != string::npos) {
      return result;
    }
  }
  return result;
}

string HTTPClient :: ReadContent() {
  string result = "";
  char buffer[4096];
  int r;
  while (true) {
    r = security
      ? SSL_read(ssl, buffer, 4095)
      : recv(sock, buffer, 4095, 0);
    if (r == 0 || r == -1) {
      return result;
    }
    buffer[r] = '\0';
    result += buffer;
  }
  return result;
}

char* HTTPClient :: ReadContentSize(int size) {
  char* result = new char[size+1];
  int r = 0;
  while (true) {
    r += security
      ? SSL_read(ssl, result+r, size-r)
      : recv(sock, result+r, size-r, 0);
    if (r >= size) {
      break;
    }
  }
  result[size] = '\0';
  return result;
}

string HTTPClient :: SendGet() {
  string request = "";
  request += "GET /"+url->url+" HTTP/1.1\r\n";
  request += "Host: "+url->host+"\r\n";
  request += "User-Agent: WEBCRAWLER\r\n";
  request += "Connection: close\r\n";
  request += "\r\n";

  int total = request.length();
  while (1) {
    int sent = security
      ? SSL_write(ssl, request.c_str(), total)
      : send(sock, request.c_str(), total, 0);
    if (sent == -1) {
      return "";
    }
    total -= sent;
    if (total == 0) {
      return request;
    }
  }
  return request;
}

string HTTPClient :: Get() {
  string request = SendGet();
  if (request == "") {
    return "";
  }

  string filename = FILE_PATH + url->host +"/"+ url->path +"/"+ url->file;

  //faz a leitura do cabeçalho da resposta HTTP
  string header = ReadHeader();

#ifdef DEBUG
  cout << "Pedido GET: " << endl << request;
  cout << header;

  string r = request + header;
  Save(filename+".request", r.c_str(), r.length());
#endif

  size_t pl0, pl1;
  //pega o codigo de resposta
  int cod = 0;
  if ((pl0 = header.find("HTTP/1.1 ")) != string::npos) {
    pl0 += strlen("HTTP/1.1 ");
    cod = atoi(header.substr(pl0, 3).c_str());
  }

  if (cod >= 400) { //se (reposta >= 400) retorna "";
    string r = request + header;
    Save(filename+".request", r.c_str(), r.length());
    return "";
  } else if (cod >= 300) { //se (resposta >= 300) verifica se tem redirecionamento
    if ((pl0 = header.find("Location: ")) != string::npos) {
      pl0 += strlen("Location: ");
      if ((pl1 = header.find("\r\n", pl0)) != string::npos) {
        //se url-direcionada diferente da pagina já requisitada, redireciona
        if (!url->equals(header.substr(pl0, pl1-pl0))) {
          HTTPClient client(header.substr(pl0, pl1-pl0));
          return client.Connect() ? client.Get() : "";
        }
      }
    }
  }
  //verifica se o conteudo já tem o tamanho definido
  int size = 0;
  if ((pl0 = header.find("Content-Length: ")) != string::npos) {
    pl0 += strlen("Content-Length: ");
    if ((pl1 = header.find("\r\n", pl0)) != string::npos) {
      size = atoi(header.substr(pl0, pl1-pl0).c_str());
    }
  }
  //se tiver tamanho definido: comum em imagens
  if (size > 0) {
    char* content = ReadContentSize(size);
    Save(filename, content, size);
    if (header.find("text/html") != string::npos) {
      return content;
    }
  } else {
    if (header.find("text/html") != string::npos) {
      string content = ReadContent();
      size_t p0 = content.find("<");
      size_t p1 = content.rfind(">");
      if ((p0 != string::npos) && (p0 != string::npos)) {
        content = content.substr(p0, (p1+1)-p0);
      }
      Save(filename, content.c_str(), content.length());
      return content;
    }
  }
  return "";
}
