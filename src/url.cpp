#include "url.h"

URL :: URL(string u) {
  if (u != "") {
    string temp = u;
    size_t p0, p1;

    // Padrão http
    protocol = "HTTP";
    port = 80;

    // Verifica se é http ou https
    if ((p0 = temp.substr(0, strlen("http://")).find("http://")) != string::npos) {
      p0 += strlen("http://");
      temp = temp.substr(p0, temp.length()-p0);
    } else if ((p0 = temp.substr(0, strlen("https://")).find("https://")) != string::npos) {
      p0 += strlen("https://");
      temp = temp.substr(p0, temp.length()-p0);
      // Padrão https
      protocol = "HTTPS";
      port = 443;
    }

    // Remove barras desnecessárias se houver
    while (temp.substr(0, 1) == "/") {
      temp = temp.substr(1, temp.length()-1);
    }

    // Verifica o host
    if ((p0 = temp.find("/")) != string::npos) {
      host = temp.substr(0, p0);
      if ((p1 = host.rfind(":")) != string::npos) {
        size_t p77;
        if ((p77 = host.rfind("/")) == string::npos) {
          p77 = host.length();
        }
        port = atoi(host.substr(p1+1, p77-p1).c_str());
        host = host.substr(0, p1);
      }
      if (host != "") {
        if (gethostbyname(host.c_str())) {
          temp = temp.substr(p0+1, temp.length());
        } else {
          host = "";
        }
      }
    } else { // URL tipo [protocol]www.site.com[:port]
      host = temp;
      if ((p1 = host.rfind(":")) != string::npos) {
        size_t p77;
        if ((p77 = host.rfind("/")) == string::npos) {
          p77 = host.length();
        }
        port = atoi(host.substr(p1+1, p77-p1).c_str());
        host = host.substr(0, p1);
      }
      if (gethostbyname(host.c_str())) {
        temp = "";
      } else {
        host = "";
      }
    }

    // Remove voltar pastas
    while (temp.substr(0, 3) == "../") {
      temp = temp.substr(3, temp.length()-3);
    }

    url = temp;

    // Verifica a query
    if ((p0 = temp.find("?")) != string::npos) {
      query = temp.substr(p0+1, temp.length()-(p0+1));
      temp = temp.substr(0, p0);
    }

    // Verifica o path e filename
    file = temp;
    if ((p0 = temp.rfind(".")) != string::npos) {
      if ((p0 = temp.rfind("/")) != string::npos) {
        file = temp.substr(p0+1, temp.length());
        file = ((file != "") ? file : "index.html");
        path = temp.substr(0, p0+1);
      }
    } else {
      path = temp;
      file = "index.html";
    }
  }
}

URL :: URL(const URL &u) {
  url      = u.url;
  protocol = u.protocol;
  host     = u.host;
  port     = u.port;
  path     = u.path;
  file     = u.file;
  query    = u.query;
}

bool URL :: equals(string u) {
  URL c(u);
  return !url.compare(c.url)
    && !protocol.compare(c.protocol)
    && !host.compare(c.host)
    && (port == c.port)
    && !path.compare(c.path)
    && !file.compare(c.file)
    && !query.compare(c.query);
}

void URL :: print() {
  cout << "URL:      " << url << endl;
  cout << "PROTOCOL: " << protocol << endl;
  cout << "HOST:     " << host << endl;
  cout << "PORT:     " << port << endl;
  cout << "PATH:     " << path << endl;
  cout << "FILE:     " << file << endl;
  cout << "QUERY:    " << query << endl;
}

string URL :: getURL() {
  string p = "/";
  if ((port != 80) && (port != 443)) {
    p  = ":";
    p += to_string(port);
    p += "/";
  }
  string u = protocol + "://" + host + p + url;
  transform(u.begin(), u.end(), u.begin(), ::tolower);
  return u;
}
