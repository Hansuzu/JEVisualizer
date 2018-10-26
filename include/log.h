#include <sstream>
#include <iostream>
#include <globalsettings.h>
  
#ifndef LOG_H
#define LOG_H

class LOG_LEND{};

class LOG{
  std::stringstream s;
  LOG* parent;
public:
  LOG(LOG* p):parent(p){
    
  }
  template <typename T> LOG& operator<<(T t){
    if (parent){
      s<<t;
      return *this;
    }else return (new LOG(this))->operator<<(t);
  }
  LOG& operator<<(LOG_LEND t){
    if (parent){
      std::cout << s.str() << std::endl;
      s.str("");
      delete this;
      return *parent;
    }else{
      return (new LOG(this))->operator<<(t);
    }
  }
};


extern LOG_LEND LEND;
extern LOG lout;



#endif
