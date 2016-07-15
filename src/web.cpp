#include "crawler.h"

int main(int argc, char** argv) {
    cout << "*** WEB Crawler ***" << endl;

  string url;
  int depth = 1;

  if (argc == 3) {
    depth = atoi(argv[1]);
    url = argv[2];
  } else {
    cout << "Informe a profundidade: " << endl;
    cin >> depth;
    cout << "Informe a URL: " << endl;
    cin >> url;
  }
  Crawler wc(depth, url);

  return 0;
}
