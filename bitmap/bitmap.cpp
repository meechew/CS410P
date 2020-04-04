// Created by CBunt on 1 APR 2020.
//

#include "bitmap.h"

Bitmap::Bitmap(){

}

std::istream &operator>>(std::istream &in, Bitmap &b) {

  in.read((char*)&b.type, 2);
  if(!strncmp(b.type,"BM",2))
    throw ; //TODO: Write throw.
  in.read((char*)& b.fSize, 4);
  in.read((char*)& b.grbge, 4);
  in.read((char*)& b.offst, 4);
  in.read((char*)& b.hSize, 4);
  in.read((char*)& b.iWide, 4);
  in.read((char*)& b.iHigh, 4);
  in.read((char*)& b.plan, 2);
  in.read((char*)& b.bits, 2);
  in.read((char*)& b.compr, 4);
  in.read((char*)& b.iSize, 4);
  in.read((char*)& b.XpPm, 4);
  in.read((char*)& b.YpPm, 4);
  in.read((char*)& b.color, 4);
  in.read((char*)& b.iColr, 4);
  //TODO: Fill in
  return <#initializer#>;
}

std::ostream &operator<<(std::ostream &in, const Bitmap &b) {
  return <#initializer#>;
}
