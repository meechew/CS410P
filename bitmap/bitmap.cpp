#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "hicpp-use-auto"
// Created by CBunt on 1 APR 2020.
//

#include <cstring>
#include "bitmap.h"


std::istream &operator>>(std::istream &in, Bitmap &b) {

  in.read((char *) &b.fHead.type, 2);
  if (!strncmp(b.fHead.type, "BM", 2));
  //throw ; //TODO: Write throw.
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
  int pixels;
  if (!b.iHead.compr) {
    pixels = b.iHead.iSize / 3; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    b.raw = new Bitmap::Pixel[pixels];
    for (int r = 0; r < pixels; ++r) {
      b.raw[r].R = in.get();
      b.raw[r].G = in.get();
      b.raw[r].B = in.get();
    }
  } else {
    pixels = b.iHead.iSize / 4; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
    b.raw = new Bitmap::Pixel[pixels];
    for (int r = 0; r < pixels; ++r) {
      b.raw[r].R = in.get();
      b.raw[r].G = in.get();
      b.raw[r].B = in.get();
      b.raw[r].A = in.get();
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
  int len = b.iHead.iWide * b.iHead.iHigh; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
  if (!b.iHead.compr){
    for(int k = 0; k < len; ++k){
      out.put(b.raw[k].R);
      out.put(b.raw[k].G);
      out.put(b.raw[k].B);
    }
  }
  else {
    for(int k = 0; k < len; ++k){
      out.put(b.raw[k].R);
      out.put(b.raw[k].G);
      out.put(b.raw[k].B);
      out.put(b.raw[k].A);
    }
  }


  out.write((char*) &b.raw, b.iHead.iSize);
  return out;
}


void Bitmap::cellShade() {
  int len = iHead.iWide * iHead.iHigh; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
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
  int len = iHead.iWide * iHead.iHigh; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
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
      tmpm[row - r][col - c] = raw[mk];
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
  for(int c = 0; c < col; ++c )
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
      tmpm[c][row - r] = raw[mk];
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
      tmpm[col - c][row - r] = raw[mk];
      ++mk;
    }
  //delete(raw);
  raw = tmpr;
  iHead.iWide = row;
  iHead.iHigh = col;
}


void Bitmap::scaleUp() {
  int col = iHead.iWide * 2; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
  int row = iHead.iHigh * 2; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
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
  int col = iHead.iWide / 2; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
  int row = iHead.iHigh / 2; // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
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
  }
  else {
    iHead.iSize = iHead.iWide * iHead.iHigh * sizeof(Pixel);
  }
  iHead.hSize = sizeof iHead;
  fHead.offst = sizeof fHead + iHead.hSize;
  if (iHead.compr)
    fHead.offst += sizeof mask;
  fHead.offst -= 2;
  fHead.fSize = fHead.offst + iHead.iSize;
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



#pragma clang diagnostic pop


