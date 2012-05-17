//
// author: Kovacs Marton
// email: tetra666@gmail.com
// license: whatever. note my name
//
// implementation.cpp
//
// the drawing implementation with conditional compiling for different OSs
// using x11 and gdi
//

#include "drawinterface.hpp"
#include "wiz.hpp"
#include <iostream>

#include <cstdlib>
#include <fstream>
#include <string>

struct printer
{
  printer(const std::string& print, std::ostream& out): print(print), out(out)
  {
    out << "BEGIN " << print << '\n';
    out.flush();
  }
  ~printer()
  {
    out << "END " << print << '\n';
    out.flush();
  }

  std::ostream& out;
  std::string print;
};

#ifdef __linux__

#include "implementation_xlib.cpp"

#elif defined _WIN32

#include "implementation_gdi.cpp"

#endif

