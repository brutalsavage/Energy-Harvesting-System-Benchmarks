

typedef struct data1 {
    unsigned int __NV_dataBin0;
    unsigned int __NV_dataBin1;
    unsigned int __NV_dataBin2;
    unsigned int __NV_dataBin3;
    unsigned int __NV_dataBin4;
    unsigned int __NV_dataBin5;
    unsigned int __NV_dataBin6;
    unsigned int __NV_dataBin7;
    unsigned int __NV_dataBin8;
    unsigned int __NV_dataBin9;
    unsigned int __NV_dataBin10;
    int r;
    int numSamples;
} data_t1;
data_t1 data1;

unsigned __histo_read__NV_dataBin(unsigned i){
  if( i == 0 ){
    return data1.__NV_dataBin0;
  }else if( i == 1 ){
    return data1.__NV_dataBin1;
  }else if( i == 2 ){
    return data1.__NV_dataBin2;
  }else if( i == 3 ){
    return data1.__NV_dataBin3;
  }else if( i == 4 ){
    return data1.__NV_dataBin4;
  }else if( i == 5 ){
    return data1.__NV_dataBin5;
  }else if( i == 6 ){
    return data1.__NV_dataBin6;
  }else if( i == 7 ){
    return data1.__NV_dataBin7;
  }else if( i == 8 ){
    return data1.__NV_dataBin8;
  }else if( i == 9 ){
    return data1.__NV_dataBin9;
  }else if( i == 10 ){
    return data1.__NV_dataBin10;
  }else { /*should never get here!*/ return 0; }
}
void __histo_write__NV_dataBin(unsigned i,unsigned v){
  if( i == 0 ){
    data1.__NV_dataBin0 = v;
  }else if( i == 1 ){
    data1.__NV_dataBin1 = v;
  }else if( i == 2 ){
    data1.__NV_dataBin2 = v;
  }else if( i == 3 ){
    data1.__NV_dataBin3 = v;
  }else if( i == 4 ){
    data1.__NV_dataBin4 = v;
  }else if( i == 5 ){
    data1.__NV_dataBin5 = v;
  }else if( i == 6 ){
    data1.__NV_dataBin6 = v;
  }else if( i == 7 ){
    data1.__NV_dataBin7 = v;
  }else if( i == 8 ){
    data1.__NV_dataBin8 = v;
  }else if( i == 9 ){
    data1.__NV_dataBin9 = v;
  }else if( i == 10 ){
    data1.__NV_dataBin10 = v;
  }else { /*should never get here!*/ return; }
}
