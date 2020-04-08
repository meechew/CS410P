#include <iostream>
#include <fstream>
#include <string>

using namespace std;



class Bitmap
{
private:

  /**
   * Read in an image.
   * reads a bitmap in from the stream
   *
   * @param in the stream to read from.
   * @param b the bitmap that we are creating.
   *
   * @return the stream after we've read in the image.
   *
   * @throws BitmapException if it's an invalid bitmap.
   * @throws bad_alloc exception if we failed to allocate memory.
   */
  friend std::istream& operator>>(std::istream& in, Bitmap& b);

  /**
   * Write the binary representation of the image to the stream.
   *
   * @param out the stream to write to.
   * @param b the bitmap that we are writing.
   *
   * @return the stream after we've finished writting.
   *
   * @throws failure if we failed to write.
   */
  friend std::ostream& operator<<(std::ostream& in, const Bitmap& b);

  struct FileHeader{
    char type[2];                             // File type. must =BM
    uint32_t fSize;                           // Size of the whole file
    uint32_t grbge;                           // Garbage. reserved for special use
    uint32_t offst;                           // Offset to start of the file
  };
  struct ImageHeader{
    uint32_t hSize;                           // Size of the header
    uint32_t iWide;                           // Width of image
    uint32_t iHigh;                           // Hight of image
    char plan[2];                             // Color Planes
    char bits[2];                             // Color depth
    uint32_t compr;                           // Compression method: 0=24, 3=32
    uint32_t iSize;                           // Image raw data size
    int32_t XpPm;                             // Pixels per meter on the X-axis
    int32_t YpPm;                             // Pixels per meter on the Y-axis
    uint32_t iColr;                           // Color pallet
    uint32_t eColr;                           // Extra color pallet
  };
  struct Pixel{
    char A;
    char B;
    char G;
    char R;
  };
  struct ColorWheel{
    uint32_t Rmask;
    uint32_t Gmask;
    uint32_t Bmask;
    uint32_t Amask;
  };
  struct
  FileHeader fHead{};                           // BMP file format header
  ImageHeader iHead{};                          // Image data header
  Pixel* raw{};                                 // Raw image data
  Pixel** map{};                                // Imaged data mapped
  ColorWheel mask{};                            // Bit mask data
  void Measure();
  void grayscale_piv();
public:
  Bitmap();
  void grayscale_wrap();

};

/**
 * cell shade an image.
 * for each component of each pixel we round to 
 * the nearest number of 0, 180, 255
 *
 * This has the effect of making the image look like.
 * it was colored.
 */
void cellShade(Bitmap& b);

/**
 * Grayscales an image by averaging all of the components.
 */
void grayscale(Bitmap& b);

/**
 * Pixelats an image by creating groups of 16*16 pixel blocks.
 */
void pixelate(Bitmap& b);

/**
 * Use gaussian bluring to blur an image.
 */
void blur(Bitmap& b);

/**
 * rotates image 90 degrees, swapping the height and width.
 */
void rot90(Bitmap& b);

/**
 * rotates an image by 180 degrees.
 */
void rot180(Bitmap& b);

/**
 * rotates image 270 degrees, swapping the height and width.
 */
void rot270(Bitmap& b);

/**
 * flips and image over the vertical axis.
 */
void flipv(Bitmap& b);

/**
 * flips and image over the horizontal axis.
 */
void fliph(Bitmap& b);

/**
 * flips and image over the line y = -x, swapping the height and width.
 */
void flipd1(Bitmap& b);

/**
 * flips and image over the line y = xr, swapping the height and width.
 */
void flipd2(Bitmap& b);

/**
 * scales the image by a factor of 2.
 */
void scaleUp(Bitmap& b);

/**
 * scales the image by a factor of 1/2.
 */
void scaleDown(Bitmap& b);



/**
 * BitmapException denotes an exception from reading in a bitmap.
 */
class BitmapException : public std::exception
{
  // the message to print out
  std::string _message;

  // position in the bitmap file (in bytes) where the error occured.
  uint32_t _position;

public:
  BitmapException() = delete;

  BitmapException(const std::string& message, uint32_t position);
  BitmapException(std::string&& message, uint32_t position);

  /**
   * prints out the exception in the form:
   *
   * "Error in bitmap at position 0xposition :
   * message"
   */
  void print_exception();
};
