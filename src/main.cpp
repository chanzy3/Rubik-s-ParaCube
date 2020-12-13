#include <getopt.h>
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "cube.h"
#include "debug.h"
#include "solver.h"
#include "solver_bfs.h"
#include "solver_ida_rec_omp.h"
#include "solver_ida_rec_seq.h"
#include "solver_ida_iter_seq.h"
#include "solver_ida_iter_omp.h"
#include "solver_ida_iter_omp_main_worker.h"
#include "solver_ida_iter_omp_uneven.h"

enum method {
  BFS = 0,
  IDA_REC_SEQ = 1,
  IDA_REC_OMP = 2,
  IDA_ITER_SEQ = 3,
  IDA_ITER_OMP = 4,
  IDA_ITER_OMP_MAIN_WORKER = 5,
  IDA_ITER_OMP_UNEVEN = 6
};

void usage(const char* progname) {
  printf("Usage: %s [options] method \n", progname);
  printf("Valid methods are: BFS, IDA_REC_SEQ, IDA_REC_OMP, IDA_ITER_SEQ, IDA_ITER_OMP, IDA_ITER_OMP_MAIN_WORKER, IDA_ITER_OMP_UNEVEN\n");
  printf("Program Options:\n");
  printf("  -f  --file  <FILENAME>        Input file name\n");
  printf("  -t  --threads  <NUM_THREADS>  Number of OMP threads, only relevant for IDA_OMP method\n");
  printf("  -?  --help                    This message\n");
}

int main(int argc, char** argv) {
  char *input_file_name = NULL;
  char *solverName = NULL;
  enum method method = BFS;
  int num_omp_threads = 1;

  // parse commandline options ////////////////////////////////////////////
  int opt;
  static struct option long_options[] = {
      {"help",     0, 0,  '?'},
      {"file",     1, 0,  'f'},
      {"threads",  1, 0,  't'},
      {0 ,0, 0, 0}
  };

  while ((opt = getopt_long(argc, argv, "t:f:?", long_options, NULL)) != EOF) {
    switch (opt) {
      case 'f':
        input_file_name = optarg;
        break;
      case 't':
        num_omp_threads = atoi(optarg);
        break;
      case '?':
      default:
        usage(argv[0]);
        return 1;
    }
  }
  // end parsing of commandline options //////////////////////////////////////

  if (optind + 1 > argc) {
    fprintf(stderr, "Error: missing solver name\n");
    usage(argv[0]);
    return 1;
  }

  solverName = argv[optind];

  if (strcmp(solverName, "BFS") == 0) {
    method = BFS;
  } else if (strcmp(solverName, "IDA_REC_SEQ") == 0) {
    method = IDA_REC_SEQ;
  } else if (strcmp(solverName, "IDA_REC_OMP") == 0) {
    method = IDA_REC_OMP;
  } else if (strcmp(solverName, "IDA_ITER_SEQ") == 0) {
    method = IDA_ITER_SEQ;
  } else if (strcmp(solverName, "IDA_ITER_OMP") == 0) {
    method = IDA_ITER_OMP;
  } else if (strcmp(solverName, "IDA_ITER_OMP_MAIN_WORKER") == 0) {
    method = IDA_ITER_OMP_MAIN_WORKER;
  } else if (strcmp(solverName, "IDA_ITER_OMP_UNEVEN") == 0) {
    method = IDA_ITER_OMP_UNEVEN;
  } else {
    fprintf(stderr, "Unknown method name (%s)\n", solverName);
    usage(argv[0]);
    return 1;
  }

  // start parsing input file for cube spec //////////////////////////////////////
  //
  // input file formats
  //
  // 1. (6 x 3 x 3)
  // C
  // 1 2 3
  // 4 5 6
  // 7 8 9
  // ...
  //
  //  2. operations
  // O
  // F F B ....

  cube_t *cube = cube_new(true);

  FILE *f = fopen(input_file_name, "r");
  char option;
  char operation;
  fscanf(f, "%c", &option);
  switch (option) {
    case 'O':
      while (fscanf(f, "%c", &operation) == 1) {
        if (operation >= 'B' && operation <= 'Z') {
          apply(cube, operation);
        }
      }
      break;
    case 'C':
      // TODO(tianez):
      break;
    default:
      fprintf(stderr, "invalid input file option '%c'\n", option);
      exit(1);
  }
  // end parsing input file for cube spec //////////////////////////////////////

  // start execution and benchmark /////////////////////////////////////////////
  switch (method) {
    case BFS:
      SolverBfs().timedSolve(cube);
      break;
    case IDA_REC_SEQ:
      SolverIdaRecSeq().timedSolve(cube);
      break;
    case IDA_REC_OMP:
      SolverIdaRecOmp(num_omp_threads).timedSolve(cube);
      break;
    case IDA_ITER_SEQ:
      SolverIdaIterSeq().timedSolve(cube);
      break;
    case IDA_ITER_OMP:
      SolverIdaIterOmp(num_omp_threads).timedSolve(cube);
      break;
    case IDA_ITER_OMP_MAIN_WORKER:
      SolverIdaIterOmpMainWorker(num_omp_threads).timedSolve(cube);
      break;
    case IDA_ITER_OMP_UNEVEN:
      SolverIdaIterOmpUneven(num_omp_threads).timedSolve(cube);
      break;
  }
}
