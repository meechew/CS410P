// Created by CBunt on 1 APR 2020.
//

#include <cstring>
#include <assert.h>
#include "bitmap.h"

Bitmap::Bitmap() = default;

std::istream &operator>>(std::istream &in, Bitmap &b) {

  in.read((char*)& b.fHead.type, 2);
  if(!strncmp(b.fHead.type,"BM",2))
    throw ; //TODO: Write throw.
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
  *b.map = b.raw;
  return in;
}

std::ostream &operator<<(std::ostream &in, const Bitmap &b) {

  in.put(b.fHead.type[0]);
  in << b.fHead.type << b.fHead.fSize << b.fHead.grbge << b.fHead.offst;
  in << b.iHead.hSize << b.iHead.iWide << b.iHead.iHigh << b.iHead.plan
     << b.iHead.bits << b.iHead.compr << b.iHead.iSize << b.iHead.XpPm
     << b.iHead.YpPm << b.iHead.iColr << b.iHead.eColr;
  return in;
}

int Bitmap::Messure() {
  assert(iHead.iWide * iHead.iHigh != (sizeof raw)/4) /* The Number of pixels must match the number pi
  if (!iHead.compr)
    iHead.iSize = size(raw);
  iHead.hSize = sizeof iHead;

  return 0;
}


