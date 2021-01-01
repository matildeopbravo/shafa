#include "module_f.h"
#include "module_t.h"
#include "module_c.h"

void call_module(unsigned char module, char *filename, char *options[]) {
  if (module == 'f') {
    int r = call_module_f(filename, options);
    if (!r)
      printf("Dados inválidos\n");

  } else if (module == 't') {
    if (!options) {
      generatecode(filename);
    } else {
      printf("O módulo T não aceita opções.");
    }
  } else if (module == 'c') {
    // chamar a funcao do modulo comas opcoes certas
    if (!options) {
      module_c(filename);
    } else {
      printf("O módulo C não aceita opções.");
    }

  } else if (module == 'd') {
    // chamar a funcao do modulo comas opcoes certas
  } else {
    printf("Módulo não existe.");
  }
}

int main(int argc, char *argv[]) {

  int error = 0;
  if (argc < 4) {
    printf("Não foram fornecidos argumentos suficientes\n");
    error = 1;
  }
  else if(strcmp(argv[2],"-m") ){
    printf("%s : opção desconhecida\n",argv[2]);
  }
  else {
    char *ficheiro = argv[1];
    unsigned char module = argv[3][0];
    argv = argc > 4 ? argv + 4 : NULL;
    call_module(module, ficheiro, argv);
  }
  return error;
}
