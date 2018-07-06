typedef struct data2 {
    unsigned int __NV_dataThresh0;
    unsigned int __NV_dataThresh1;
    unsigned int __NV_dataThresh2;
    unsigned int __NV_dataThresh3;
    unsigned int __NV_dataThresh4;
    unsigned int __NV_dataThresh5;
    unsigned int __NV_dataThresh6;
    unsigned int __NV_dataThresh7;
    unsigned int __NV_dataThresh8;
    unsigned int __NV_dataThresh9;
    unsigned int __NV_dataThresh10;
} data_t2;
data_t2 data2;



unsigned __histo_read__NV_dataThresh(unsigned i){
  if( i == 0 ){
    return data2.__NV_dataThresh0;
  }else if( i == 1 ){
    return data2.__NV_dataThresh1;
  }else if( i == 2 ){
    return data2.__NV_dataThresh2;
  }else if( i == 3 ){
    return data2.__NV_dataThresh3;
  }else if( i == 4 ){
    return data2.__NV_dataThresh4;
  }else if( i == 5 ){
    return data2.__NV_dataThresh5;
  }else if( i == 6 ){
    return data2.__NV_dataThresh6;
  }else if( i == 7 ){
    return data2.__NV_dataThresh7;
  }else if( i == 8 ){
    return data2.__NV_dataThresh8;
  }else if( i == 9 ){
    return data2.__NV_dataThresh9;
  }else if( i == 10 ){
    return data2.__NV_dataThresh10;
  }else { /*should never get here!*/ return 0; }
}
void __histo_write__NV_dataThresh(unsigned i,unsigned v){
  if( i == 0 ){
    data2.__NV_dataThresh0 = v;
  }else if( i == 1 ){
    data2.__NV_dataThresh1 = v;
  }else if( i == 2 ){
    data2.__NV_dataThresh2 = v;
  }else if( i == 3 ){
    data2.__NV_dataThresh3 = v;
  }else if( i == 4 ){
    data2.__NV_dataThresh4 = v;
  }else if( i == 5 ){
    data2.__NV_dataThresh5 = v;
  }else if( i == 6 ){
    data2.__NV_dataThresh6 = v;
  }else if( i == 7 ){
    data2.__NV_dataThresh7 = v;
  }else if( i == 8 ){
    data2.__NV_dataThresh8 = v;
  }else if( i == 9 ){
    data2.__NV_dataThresh9 = v;
  }else if( i == 10 ){
    data2.__NV_dataThresh10 = v;
  }else { /*should never get here!*/ return; }
}
