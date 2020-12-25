#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"


void call_module(unsigned char module, char * filename, char * options []) {
  if (module == 'f') {
    // chamar a funcao do modulo comas opcoes certas

  }
  else if (module == 't') {
    // chamar a funcao do modulo comas opcoes certas

  }
  else if (module == 'c') {
    // chamar a funcao do modulo comas opcoes certas
    if(!options) {
        module_c(filename);

    }
    else {
      printf("O módulo C não aceita opções.");
    }

  }
  else if (module == 'd') {
    // chamar a funcao do modulo comas opcoes certas

  }


}

int main (int argc, char * argv []) {

  char * ficheiro = argv[1];
  unsigned char module = argv[3][0];
  argv = argc > 4 ? argv + 4  : NULL ;
  call_module(module,ficheiro,argv);
  return 0;
}
