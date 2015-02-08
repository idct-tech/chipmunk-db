#include "../Headers/helpers.h"

vector<string> helpers::explode(const string& str, const char delimiter)
{
  vector<string> elements;
  stringstream stream(str);
  string item;
  while (getline(stream, item, delimiter))
  {
	  elements.push_back(item);
  }


  return elements;
}

string helpers::chtos(char* buf) {
	int max = 1024;
	size_t len = 0;
	while ((len < max) && (buf[len] != '\0' && buf[len] != EOF)) {
		len++;
	}

	return string(buf, len);
}