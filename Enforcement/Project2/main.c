#include <stdio.h>
#include <string.h>
#include "parser.h"

/*
 * Compile with command "gcc main.c"
 *
 * Run with command "./a.out demo.code demo.data"
 *
 */

int main(int argc,char* argv[]) {
  
  struct program_node prog;
  
  parser_program(&prog, argv[1]);
  printer_program(&prog);
  executor_program(&prog, argv[2]);
  
  delete_program(&prog);

  return 0;
}
