#include "crawler.h"

Crawler :: Crawler(int depth, string host) {
  this->depth = depth;
  this->host  = host;

  //baixa a primeira página e empilha seus links
  visit(depth, *(new URL(host)));

  while (depth) {
    depth--;
    //copia as urls disponiveis
    vector<URL*> url;
    while (!urls.empty()) {
      url.push_back(urls.back());
      urls.pop_back();
    }
    //baixa as páginas disponiveis e empilha seus links com threads
    while (!url.empty()) {
      URL* u = url.back();
      url.pop_back();

      thread* t = new thread(&Crawler::visit, this, depth, *u);
      threads.push_back(t);

      //deleta a thread a mais tempo
      if (threads.size() >= MAX_THREADS) {
        threads[0]->join();
        delete threads[0];
        threads.erase(threads.begin(), threads.begin()+1);
      }

      delete u;
    }
    while (!threads.empty()) {
      thread* t =  threads.back();
      t->join();
      delete t;
      threads.pop_back();
    }
  }
}

void Crawler :: visit(int depth, const URL& u) {
  HTTPClient client(u);
  if (client.Connect()) {
    string r = client.Get();
    if ((depth > 0) && (r != "")) {
      vector<string> links = ExtractLinks(r);
      if (links.size() > 0) {
        for (size_t i = 0; i < links.size(); i++) {
          URL* url =  new URL(links[i]);
            if (url->host == "") {
              url->host = u.host;
            }
            if (url->host != "") {
              urls.push_back(url);
            }
        }
      }
    }
  }
}

vector<string> Crawler :: ExtractLinks(string s) {
  vector<string> result;
  if (s == "") {
    return result;
  }

  vector<string> s1, s2;
  s1.push_back("<a ");
  s1.push_back("<img ");
  s2.push_back("href=\"");
  s2.push_back("src=\"");

  string temp;
  size_t p2, p3;

  for (size_t i = 0; i < s1.size(); i++) {
    size_t p0, p1 = 0;
    while (((p0 = s.find(s1[i], p1)) != string::npos) && ((p1 = s.find(">", p0+s1[i].length())) != string::npos)) {
      temp = s.substr(p0, p1-p0+1);
      if ((p2 = temp.find(s2[i])) != string::npos) {
        p2 += s2[i].length();
        if ((p3 = temp.find("\"", p2)) != string::npos) {
          temp = temp.substr(p2, p3-p2);
          //remove link vazio "" e "/"
          //remove link de ancora "#..."
          //remove link de codigo javascript
          if ((temp != "") && (temp != "/")  && (temp[0] != '#') && (temp.find("javascript:") == string::npos)) {
            result.push_back(temp);            
          }
        }
      }
    }
  }
  return result;
}
