// Created by CBunt on 1 APR 2020.
//

#include <cstring>
#include <assert.h>
#include "bitmap.h"


std::istream &operator>>(std::istream &in, Bitmap &b) {

  in.read((char*)& b.fHead.type, 2);
  if(!strncmp(b.fHead.type,"BM",2));
    //throw ; //TODO: Write throw.
  in.read((char*)& b.fHead.fSize, 4);
  in.read((char*)& b.fHead.grbge, 4);
  in.read((char*)& b.fHead.offst, 4);
  in.read((char*)& b.iHead.hSize, 4);
  in.read((char*)& b.iHead.iWide, 4);
  in.read((char*)& b.iHead.iHigh, 4);
  in.read((char*)& b.iHead.plan, 2);
  in.read((char*)& b.iHead.bits, 2);
  in.read((char*)& b.iHead.compr, 4);
  in.read((char*)& b.iHead.iSize, 4);
  in.read((char*)& b.iHead.XpPm, 4);
  in.read((char*)& b.iHead.YpPm, 4);
  in.read((char*)& b.iHead.iColr, 4);
  in.read((char*)& b.iHead.eColr, 4);
  int pixels;
  if (!b.iHead.compr) {
    pixels = b.iHead.iSize / 3;
    b.raw = new Bitmap::Pixel[pixels];
    for (int k = 0; k < pixels; ++k) {
      b.raw[k].R = in.get();
      b.raw[k].G = in.get();
      b.raw[k].B = in.get();
    }
  }
  else {
    pixels = b.iHead.iSize / 4;
    b.raw = new Bitmap::Pixel[pixels];
    for (int k = 0; k < pixels; ++k) {
      b.raw[k].R = in.get();
      b.raw[k].G = in.get();
      b.raw[k].B = in.get();
      b.raw[k].A = in.get();
    }
  }
  b.map[b.iHead.iWide][b.iHead.iHigh];
  b.map = &b.raw;
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

Bitmap::Bitmap() = default;
Bitmap::~Bitmap() = default;

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

void Bitmap::Measure() {
  Bitmap tmp;
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
  fHead.fSize = fHead.offst + iHead.iSize;
}

void Bitmap::pixelate() {
  int col = iHead.iWide;
  int row = iHead.iHigh;
  for(int c = 0; c < col; c += 16)
    for(int r = 0; r < row; r += 16) {
      PixelateBlock(c, r, reinterpret_cast<Pixel **>(&tmp[0][0]));
    }
  raw = &tmp[0][0];
  map = &raw;
}

void Bitmap::PixelateBlock(int col, int row, Pixel **tmp) {
  int Rpool = 0, Gpool = 0, Bpool = 0;
  for(int c = col; c < col && c < iHead.iWide + 16; ++c)
   for(int r = row; r < row && r < iHead.iHigh + 16; ++r){
     Rpool += map[c][r].R;
     Gpool += map[c][r].G;
     Bpool += map[c][r].B;
   }
  Rpool /= 256;
  Gpool /= 256;
  Bpool /= 256;
  for(int c = col; c < col + 16; ++c)
    for(int r = row; r < row + 16; ++r){
      tmp[c][r].R = Rpool;
      tmp[c][r].G = Gpool;
      tmp[c][r].B = Bpool;
    }
}







