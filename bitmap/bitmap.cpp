// Created by CBunt on 1 APR 2020.
//

#include "bitmap.h"

Bitmap::Bitmap(){

}


istream &Bitmap::operator>>(Bitmap &b) {
  uint32_t size;
  in.read((char*)&type);
  if(!strncmp(type,"BM",2))
    throw ; //TODO: Write throw.
  in.read((char*)&size, 4);
  //TODO: Fill in
  return <#initializer#>;
}

ostream &Bitmap::operator<<(const Bitmap &b) {
  return <#initializer#>;
}

