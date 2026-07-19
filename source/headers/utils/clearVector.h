#ifndef CLEARVECTOR 
#define CLEARVECTOR 

#include <vector>

template<typename T>
inline void clearVector(std::vector<T*>& toClearVector) {
  for(auto& element : toClearVector) {
    delete element;
  }
  toClearVector.clear();
}

#endif
