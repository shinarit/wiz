#include <iostream>
#include <cstdlib>
#include <vector>

#ifdef __linux__
# include <sys/ioctl.h>
#elif defined _WIN32
//what to include on windows?
#endif

struct Size
{
  int columns;
  int rows;
};


Size getConsoleDimensions()
{
  int col;
  int row;

#ifdef __linux__
  winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  col = w.ws_col;
  row = w.ws_row;

#elif defined _WIN32

#endif

  return Size{col, row};
}

class DrawArea
{
public:
  DrawArea(int row, int col): m_board(row * col, ' '), m_width(col)
  {}
  void setPixel(int x, int y, char pixel = 'X')
  {
    m_board[y * m_width + x] = pixel;
  }
  void clearPixel(int x, int y)
  {
    setPixel(x, y, ' ');
  }
  void drawLine(int x1, int y1, int x2, int y2)
  {/*
   function line(x0, y0, x1, y1)
     d x := ab*s(x1-x0)
     dy := abs(y1-y0)
     if x0 < x1 then sx := 1 else sx := -1
       if y0 < y1 then sy := 1 else sy := -1
         err := dx-dy

         loop
         setPixel(x0,y0)
         if x0 = x1 and y0 = y1 exit loop
           e2 := 2*err
          if e2 > -dy then
             err := err - dy
             x0 := x0 + sx
          end if
          if e2 <  dx then
            err := err + dx
            y0 := y0 + sy
          end if
               end loop
*/
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);

    int sx = ((x1 < x2) ? 1 : -1);
    int sy = ((y1 < y2) ? 1 : -1);

    int err = dx - dy;

    while (true)
    {
      setPixel(x1, y1);
      if (x1 == x2 && y1 == y2)
      {
        break;
      }
      int err2 = 2 * err;
      if (err2 > -dy)
      {
        err -= dy;
        x1 += sx;
      }
      if (err2 < dx)
      {
        err += dx;
        y1 += sy;
      }
    }
  }
  void drawCircle(int x, int y, int r)
  {

  }

  void print()
  {
    int i(0);
    while (m_board.size() > i)
    {
      std::cout.write(&m_board[i], m_width);
      std::cout << '\n';

      i += m_width;
    }
  }

private:
  typedef std::vector<char> Board;
  Board m_board;
  int m_width;
};


int main(int argc, char* argv[])
{
  Size console(getConsoleDimensions());
  std::cout << "rows: " << console.rows << ", columns: " << console.columns << '\n';

  DrawArea drawArea(console.rows, console.columns);

  for (int i(1); i<argc; ++i)
  {
    switch (argv[i][0])
    {
      case 'l':
      case 'L':
      {
        //draw line
        int x1(std::atoi(argv[++i]));
        int y1(std::atoi(argv[++i]));
        int x2(std::atoi(argv[++i]));
        int y2(std::atoi(argv[++i]));

        std::cout << "draw line: " << x1 << ':' << y1 << " -> " << x2 << ':' << y2 << '\n';
        drawArea.drawLine(x1, y1, x2, y2);
        break;
      }
      case 'c':
      case 'C':
      {
        //draw circle
        int x(std::atoi(argv[++i]));
        int y(std::atoi(argv[++i]));
        int r(std::atoi(argv[++i]));

        std::cout << "draw circle: " << x << ':' << y << "; " << r << '\n';
        drawArea.drawCircle(x, y, r);
        break;
      }
    }
  }

  drawArea.print();
}
