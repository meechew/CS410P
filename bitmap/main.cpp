#include <iostream>
#include <fstream>
#include <string>
#include "bitmap.h"

using namespace std;

int main(int argc, char** argv)
{
  if(argc != 4)
    {
      cout << "usage:\n"
	   << "bitmap option inputfile.bmp outputfile.bmp\n"
	   << "options:\n"
	   << "  -c cell shade\n"
	   << "  -g gray scale\n"
	   << "  -p pixelate\n"
	   << "  -b blur\n"
	   << "  -r90 rotate 90\n"
	   << "  -r180 rotate 180\n"
	   << "  -r270 rotate 270\n"
	   << "  -v flip vertically\n"
	   << "  -h flip horizontally\n"
	   << "  -d1 flip diagonally 1\n"
	   << "  -d2 flip diagonally 2\n"
	   << "  -grow scale the image by 2\n"
	   << "  -shrink scale the image by .5" << endl;

      return 0;
    }

  string flag(argv[1]);
  string infile(argv[2]);
  string outfile(argv[3]);

  ifstream in;
  Bitmap image;
  ofstream out;

  try
    {
      in.open(infile, ios::binary);
      in >> image;
      in.close();
    }
  catch(BitmapException& e)
    {
      //e.print_exception();
      return 0;
    }

  if(flag == "-c"s)
    {
      image.cellShade();
    }
  if(flag == "-g"s)
    {
      image.grayscale();
    }
  if(flag == "-p"s)
    {
      image.pixelate();
    }
  if(flag == "-b"s)
    {
      image.blur();
    }
  if(flag == "-r90"s)
    {
      image.rot90();
    }
  if(flag == "-r180"s)
    {
      image.rot180();
    }
  if(flag == "-r270"s)
    {
      image.rot270();
    }
  if(flag == "-v"s)
    {
      image.flipv();
    }
  if(flag == "-h"s)
    {
      image.fliph();
    }
  if(flag == "-d1"s)
    {
      image.flipd1();
    }
  if(flag == "-d2"s)
    {
      image.flipd2();
    }
  if(flag == "-grow"s)
    {
      image.scaleUp();
    }
  if(flag == "-shrink"s)
    {
      image.scaleDown();
    }

  out.open(outfile, ios::binary);
  out << image;
  out.close();

  return 0;
}

