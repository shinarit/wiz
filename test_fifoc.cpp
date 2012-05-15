#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: " << argv[0] << " <commands file> <data file>\n";
    return 1;
  }

  std::ifstream in(argv[2]);
  std::ofstream out(argv[1]);
  
  if (!out || !in)
  {
    std::cerr << "Error with opening file\n";
    return 2;
  }

  std::cerr << "\"Connection\" established\n";

  const std::string command = "speed 10 10\n";
  const std::string end = "end\n";
  std::string str;
  while (true)
  {
    std::getline(in, str);
    std::cerr << "Got " << str << '\n';

    out << command;
    out.flush();
    std::cerr << "sent command: " << command;

    std::getline(in, str);
    std::cerr << "Got " << str << '\n';

    out << "shoot 100 200\n";
    out.flush();
    std::cerr << "sent command: shoot 100 200\n";

    std::getline(in, str);
    std::cerr << "Got " << str << '\n';

    out << "get speed\n";
    out.flush();
    std::cerr << "queried speed\n";

    std::getline(in, str);
    std::cerr << "Got " << str << '\n';

    out << "get enemies\n";
    out.flush();
    std::cerr << "queried enemies\n";

    std::getline(in, str);
    std::cerr << "Got " << str << '\n';

    out << end;
    out.flush();
    std::cerr << "sent end: " << end;

    std::cerr << '\n';
  }
}

