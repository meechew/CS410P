// Created by CBunt on 1 APR 2020.
//



#include "bitmap.h"


std::istream &operator>>(std::istream &in, Bitmap &b) {

  in.read((char *) &b.fHead.type, 2);
  if (strncmp(b.fHead.type, "BM", 2))
    throw BitmapException("***File Format ERROR***", __LINE__, __func__);
  in.read((char *) &b.fHead.fSize, 4);
  in.read((char *) &b.fHead.grbge, 4);
  in.read((char *) &b.fHead.offst, 4);
  in.read((char *) &b.iHead.hSize, 4);
  in.read((char *) &b.iHead.iWide, 4);
  in.read((char *) &b.iHead.iHigh, 4);
  in.read((char *) &b.iHead.plan, 2);
  in.read((char *) &b.iHead.bits, 2);
  in.read((char *) &b.iHead.compr, 4);
  in.read((char *) &b.iHead.iSize, 4);
  in.read((char *) &b.iHead.XpPm, 4);
  in.read((char *) &b.iHead.YpPm, 4);
  in.read((char *) &b.iHead.iColr, 4);
  in.read((char *) &b.iHead.eColr, 4);

  int pixels = b.iHead.iWide * b.iHead.iHigh;
  b.raw = new Bitmap::Pixel[pixels];

  if (!b.iHead.compr) {
    b.pad = b.iHead.iWide % 4;
    in.seekg(b.fHead.offst);
    for (int c = 0; c < pixels; ++c) {
      b.raw[c].R = in.get();
      b.raw[c].G = in.get();
      b.raw[c].B = in.get();
      if ((c + 1) % b.iHead.iWide == 0) {
        in.ignore(b.pad);
      }
    }

  } else {
    uint32_t buff;
    in.read((char *) &b.mask.Rmask, 4);
    in.read((char *) &b.mask.Gmask, 4);
    in.read((char *) &b.mask.Bmask, 4);
    in.read((char *) &b.mask.Amask, 4);
    in.seekg(b.fHead.offst);
    for (int c = 0; c < pixels; ++c) {
      in.read((char *) &buff, 4);
      b.raw[c].R = Bitmap::Unmask(b.mask.Rmask, buff);
      b.raw[c].G = Bitmap::Unmask(b.mask.Gmask, buff);
      b.raw[c].B = Bitmap::Unmask(b.mask.Bmask, buff);
      b.raw[c].A = Bitmap::Unmask(b.mask.Amask, buff);
    }
  }

  return in;
}

std::ostream &operator<<(std::ostream &out, const Bitmap &b) {
  out.write(b.fHead.type, 2);
  out.write((char*) &b.fHead.fSize, 4);
  out.write((char*) &b.fHead.grbge, 4);
  out.write((char*) &b.fHead.offst, 4);
  out.write((char*) &b.iHead.hSize, 4);
  out.write((char*) &b.iHead.iWide, 4);
  out.write((char*) &b.iHead.iHigh, 4);
  out.write(b.iHead.plan, 2);
  out.write(b.iHead.bits, 2);
  out.write((char*) &b.iHead.compr, 4);
  out.write((char*) &b.iHead.iSize, 4);
  out.write((char*) &b.iHead.XpPm, 4);
  out.write((char*) &b.iHead.YpPm, 4);
  out.write((char*) &b.iHead.iColr, 4);
  out.write((char*) &b.iHead.eColr, 4);

  if (b.iHead.compr){
    out.write((char*) &b.mask.Rmask, 4);
    out.write((char*) &b.mask.Gmask, 4);
    out.write((char*) &b.mask.Bmask, 4);
    out.write((char*) &b.mask.Amask, 4);
  }
  int len = b.iHead.iWide * b.iHead.iHigh;
  uint64_t ZERO[1] = {0};
  if (!b.iHead.compr){
    for(int k = 0; k < len; ++k){
      out.put(b.raw[k].R);
      out.put(b.raw[k].G);
      out.put(b.raw[k].B);
      if ((k + 1) % b.iHead.iWide == 0)
        out.write((char*)ZERO, b.pad);
    }
  }
  else {
    out.seekp(b.fHead.offst);
    for(int k = 0; k < len; ++k){
      out.put(b.raw[k].A);
      out.put(b.raw[k].B);
      out.put(b.raw[k].G);
      out.put(b.raw[k].R);
    }
  }


  out.write((char*) &b.raw, b.iHead.iSize);
  return out;
}

void cellShade(Bitmap &b) {b.cellShade();}
void grayscale(Bitmap& b) {b.grayscale();}
void pixelate(Bitmap& b) { b.pixelate();}
void blur(Bitmap& b) { b.blur();}
void rot90(Bitmap& b) { b.rot90();}
void rot180(Bitmap& b) { b.rot180();}
void rot270(Bitmap& b) { b.rot270();}
void flipv(Bitmap& b) { b.flipv();}
void fliph(Bitmap& b) { b.fliph();}
void flipd1(Bitmap& b) { b.flipd1();}
void flipd2(Bitmap& b) { b.flipd2();}
void scaleUp(Bitmap& b) { b.scaleUp();}
void scaleDown(Bitmap& b) { b.scaleDown();}

void Bitmap::cellShade() {
  int len = iHead.iWide * iHead.iHigh;
  for(int k = 0; k < len; ++k){
    if (raw[k].R < 64)
      raw[k].R = 0;
    else if (raw[k].R < 192)
      raw[k].R = 128;
    else raw[k].R = 255;
    if (raw[k].G < 64)
      raw[k].G = 0;
    else if (raw[k].G < 192)
      raw[k].G = 128;
    else raw[k].G = 255;
    if (raw[k].B < 64)
      raw[k].B = 0;
    else if (raw[k].B < 192)
      raw[k].B = 128;
    else raw[k].B = 255;
  }
}
void Bitmap::grayscale() {
  int len = iHead.iWide * iHead.iHigh;
  int pix;
  for(int k = 0; k < len; ++k){
    pix = (raw[k].B + raw[k].G + raw[k].R) / 3;
    raw[k].B = (char)pix;
    raw[k].G = (char)pix;
    raw[k].R = (char)pix;
  }
}

void Bitmap::pixelate() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmap[row];
  for(int r = 0; r < row; ++r )
    tmap[r] = &tmpr[r * col];
  for(int r = 0; r < row; r += 16)
    for(int c = 0; c < col; c += 16)
      PixelateBlock(r, c, tmap);
  delete(raw);
  raw = tmpr;
}

void Bitmap::PixelateBlock(int row, int col, Pixel **tmp) {
  int Rpool = 0, Gpool = 0, Bpool = 0;
  Pixel *tmap[iHead.iHigh];
  for(int c = 0; c < iHead.iHigh; ++c )
    tmap[c] = &raw[c * iHead.iWide];
  for(int r = row; r < row + 16 && r < iHead.iHigh; ++r)
   for(int c = col; c < col + 16 && c < iHead.iWide; ++c){
     Rpool += tmap[r][c].R;
     Gpool += tmap[r][c].G;
     Bpool += tmap[r][c].B;
   }
  Rpool /= 256;
  Gpool /= 256;
  Bpool /= 256;
  Pixel pool((char)Rpool, (char)Gpool, (char)Bpool);
  for(int r = row; r < row + 16 && r < iHead.iHigh; ++r)
    for(int c = col; c < col + 16 && c < iHead.iWide; ++c){
      FillBlock(r, c, tmp, 16, pool, *this);
    }
}

void Bitmap::FillBlock(int row, int col, Pixel **tmp, int met, Pixel &p, Bitmap &boarders) {
  for(int r = row; r < row + met && r < boarders.iHead.iHigh; ++r)
    for(int c = col; c < col + met && c < boarders.iHead.iWide; ++c){
      tmp[r][c] = p;
    }
}

void Bitmap::blur() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmap[row];
  int mk = 0;
  for(int c = 0; c < row; ++c )
    tmap[c] = &raw[c * col];

  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c){
      tmpr[mk] = KernelBlur(r, c, tmap);
      mk++;
      }
  delete raw;
  raw = tmpr;
}

Bitmap::Pixel Bitmap::KernelBlur(int row, int col, Pixel **map) {
  Pixel ret;
  Pixel ker[25];
  int k = 0;
  for(int r = row-2; r < row + 3; ++r)
    for(int c = col-2; c < col + 3; ++c) {
      if(c + col < 0 || r + row <0|| c + col > iHead.iWide || r + row > iHead.iHigh) {
        k++;
        continue;
      }
      ker[k] = map[r][c];
      ++k;
    }
  ret.R += (ker[0].R + ker[4].R + ker[20].R + ker[24].R)/1024;
  ret.G += (ker[0].G + ker[4].G + ker[20].G + ker[24].G)/1024;
  ret.B += (ker[0].B + ker[4].B + ker[20].B + ker[24].B)/1024;
  ret.R += (ker[1].R + ker[3].R + ker[5].R + ker[9].R) * 4 /1024;
  ret.G += (ker[1].G + ker[3].G + ker[5].G + ker[9].G) * 4 /1024;
  ret.B += (ker[1].B + ker[3].B + ker[5].B + ker[9].B) * 4 /1024;
  ret.R += (ker[15].R + ker[19].R + ker[21].R + ker[23].R) * 4 /1024;
  ret.G += (ker[15].G + ker[19].G + ker[21].G + ker[23].G) * 4 /1024;
  ret.B += (ker[15].B + ker[19].B + ker[21].B + ker[23].B) * 4 /1024;
  ret.R += (ker[15].R + ker[10].R + ker[14].R + ker[22].R) * 6 /1024;
  ret.G += (ker[15].G + ker[10].G + ker[14].G + ker[22].G) * 6 /1024;
  ret.B += (ker[15].B + ker[10].B + ker[14].B + ker[22].B) * 6 /1024;
  ret.R += (ker[6].R + ker[8].R + ker[16].R + ker[18].R) * 16 /1024;
  ret.G += (ker[6].G + ker[8].G + ker[16].G + ker[18].G) * 16 /1024;
  ret.B += (ker[6].B + ker[8].B + ker[16].B + ker[18].B) * 16 /1024;
  ret.R += (ker[7].R + ker[11].R + ker[13].R + ker[17].R) * 24 /1024;
  ret.G += (ker[7].G + ker[11].G + ker[13].G + ker[17].G) * 24 /1024;
  ret.B += (ker[7].B + ker[11].B + ker[13].B + ker[17].B) * 24 /1024;
  ret.R += ker[12].R * 36 /256;
  ret.G += ker[12].G * 36 /256;
  ret.B += ker[12].B * 36 /256;

  return ret;
}

void Bitmap::rot90() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[col];
  for(int c = 0; c < col; ++c)
    tmpm[c] = &tmpr[c * row];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[col - c - 1][row - r] = raw[mk];
      mk++;
    }
  delete(raw);
  raw = tmpr;
  iHead.iWide = row;
  iHead.iHigh = col;
}

void Bitmap::rot180() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[row];
  for(int r = 0; r < row; ++r )
    tmpm[r] = &tmpr[r * col];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[row - r - 1][col - c] = raw[mk];
      ++mk;
    }
  delete(raw);
  raw = tmpr;
}

void Bitmap::rot270() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[col];
  for(int c = 0; c < col; ++c)
    tmpm[c] = &tmpr[c * row];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[c][row - r] = raw[mk];
      ++mk;
    }
  delete(raw);
  raw = tmpr;
  iHead.iWide = row;
  iHead.iHigh = col;
}

void Bitmap::flipv() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[row];
  for(int c = 0; c < row; ++c )
    tmpm[c] = &tmpr[c * col];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[r][col - c] = raw[mk];
      ++mk;
    }
  delete(raw);
  raw = tmpr;
}

void Bitmap::fliph() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[row];
  for(int c = 0; c < row; ++c )
    tmpm[c] = &tmpr[c * col];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[row - r][c] = raw[mk];
      ++mk;
    }
  delete(raw);
  raw = tmpr;
}


void Bitmap::flipd1() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[col];
  for(int c = 0; c < col; ++c)
    tmpm[c] = &tmpr[c * row];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[col - c - 1][row - r] = raw[mk];
      ++mk;
    }
  delete(raw);
  raw = tmpr;
  iHead.iWide = row;
  iHead.iHigh = col;
}

void Bitmap::flipd2() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  Pixel *tmpr = new Pixel[col * row];
  Pixel *tmpm[col];
  for(int c = 0; c < col; ++c)
    tmpm[c] = &tmpr[c * row];
  int mk = 0;
  for(int r = 0; r < row; ++r)
    for(int c = 0; c < col; ++c) {
      tmpm[c][r] = raw[mk];
      ++mk;
    }
  delete(raw);
  raw = tmpr;
  iHead.iWide = row;
  iHead.iHigh = col;
}

void Bitmap::scaleUp() {
  int col = iHead.iWide * 2;
  int row = iHead.iHigh * 2;
  Pixel *tmpr = new Pixel[col * row];
  Pixel **tmpm = new Pixel*[row];
  for(int c = 0; c < col; ++c )
    tmpm[c] = &tmpr[c * col];
  Bitmap boarders;
  boarders.iHead.iWide = col;
  boarders.iHead.iHigh = row;
  int mk = 0;
  for(int r = 0; r < iHead.iHigh; ++r)
    for(int c = 0; c < iHead.iWide; ++c) {
      FillBlock(r * 2, c * 2, tmpm, 2, raw[mk], boarders);
      mk++;
    }
  delete(raw);
  raw = tmpr;
  iHead.iWide = col;
  iHead.iHigh = row;
  Measure();
}

void Bitmap::scaleDown() {
  int col = iHead.iWide / 2;
  int row = iHead.iHigh / 2;
  Pixel *tmpr = new Pixel[col * row];
  Pixel **tmpm = new Pixel*[row];
  for(int c = 0; c < col; ++c )
    tmpm[c] = &tmpr[c * col];
  int mk = 0;
  for(int r = 0; r < row; ++r) {
    if(r%2 != 0) {
      mk += col;
      continue;
    }
    for(int c = 0; c < col; ++c){
      if(c%2 != 0) {
        mk++;
        continue;
      }
      tmpm[r][c] = raw[mk];
      mk++;
    }
  }
  delete(raw);
  raw = tmpr;
  iHead.iWide = col;
  iHead.iHigh = row;
  Measure();
}

void Bitmap::Measure() {
  if (!iHead.compr) {
    iHead.iSize = ((iHead.iWide * iHead.iHigh) / 4) * 3 * sizeof(Pixel);
    pad = iHead.iWide % 4;
  }
  else {
    iHead.iSize = iHead.iWide * iHead.iHigh * sizeof(Pixel);
  }
  iHead.iSize += pad * iHead.iHigh;
  iHead.hSize = sizeof iHead;
  fHead.offst = sizeof fHead + iHead.hSize;
  if (iHead.compr)
    fHead.offst += sizeof mask + 68;
  fHead.offst -= 2;
  fHead.fSize = fHead.offst + iHead.iSize;
}

uint32_t Bitmap::Unmask(const uint32_t Cmask, const uint32_t Cpixel) {
  bitset<32> ret = 0;
  bitset<32> Tmask(Cmask);
  bitset<32> Tpixel(Cpixel);
  for(int k = 0; k < 32; ++k)
    if(!Tmask[0]) {
      Tmask >>= 1;
      Tpixel >>= 1;
    }
    else break;
  for(int k = 0; k < 32; ++k)
    if(Tmask[k])
    ret[k] = Tmask[k] && Tpixel[k];
    else break;
  return ret.to_ulong();
}



Bitmap::Bitmap() = default;
Bitmap::~Bitmap() = default;
Bitmap::Pixel::Pixel() {
  R = 0;
  G = 0;
  B = 0;
  A = 0;
}
Bitmap::Pixel::Pixel(char nR, char nG, char nB) {
  R = nR;
  G = nG;
  B = nB;
  A = 0;
}

Bitmap::Pixel::Pixel(char nR, char nG, char nB, char nA) {
  R = nR;
  G = nG;
  B = nB;
  A = nA;
}


BitmapException::BitmapException(std::string &&message, uint32_t position, std::string &&function) {
  _message = message;
  _position = position;
  _function = function;
}

void BitmapException::print_exception() {
  cerr << _message << endl <<  "\tIn " << _function << "at position 0x" << _position;
}
