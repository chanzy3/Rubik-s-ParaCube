//
// Created by Tiane Zhu on 11/25/20.
//

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cycleTimer.h"
#include "solver.h"

bool Solver::timedSolve(cube_t *cube) {
  int solution[MAX_DEPTH];
  int num_steps;

  // time the execution
  bool solution_found;
  double start_time = CycleTimer::currentSeconds();
  solution_found = solve(cube, solution, &num_steps);
  double end_time = CycleTimer::currentSeconds();

  // print results
  if (!solution_found) {
    fprintf(stderr, "did not find solution\n");
    exit(0);
  }

  fprintf(stdout, "%d solution steps: ", num_steps);
  for (int i=0; i<num_steps; i++) {
    fprintf(stdout, "%s ", to_string(solution[i]));
  }
  fprintf(stdout, "\n");

  // print timing
  double overallDuration = end_time - start_time;
  fprintf(stdout, "Overall: %.3f ms\n", 1000.f * overallDuration);

  return solution_found;
}


const char *Solver::to_string(int operation) {
  switch(operation) {
    case 0:
      return "F1";
    case 1:
      return "B1";
    case 2:
      return "L1";
    case 3:
      return "R1";
    case 4:
      return "U1";
    case 5:
      return "D1";
    case 6:
      return "F2";
    case 7:
      return "B2";
    case 8:
      return "L2";
    case 9:
      return "R2";
    case 10:
      return "U2";
    case 11:
      return "D2";
    case 12:
      return "F3";
    case 13:
      return "B3";
    case 14:
      return "L3";
    case 15:
      return "R3";
    case 16:
      return "U3";
    case 17:
      return "D3";
    default:
      return "??";
  }
}

node_t *node_new() {
  node_t *node = (node_t *) malloc(sizeof(node_t));
  if (node == NULL) {
    perror("malloc node");
    exit(1);
  }

  node->cube = cube_new(false);
  node->d = 0;

  return node;
}

node_t *node_new_from_cube(cube_t* cube) {
  node_t *node = node_new();
  memcpy(node->cube, cube, sizeof(cube_t));
  return node;
}

node_t *node_cpy(node_t *node) {
  node_t *ret = node_new();
  memcpy(ret->cube, node->cube, sizeof(cube_t));
  memcpy(ret->steps, node->steps, MAX_DEPTH * sizeof(int));
  ret->d = node->d;
  return ret;
}

void node_destroy(node_t *node) {
  free(node->cube);
  free(node);
}

bool can_prune(int prev_op, int op) {
  int u = prev_op % 6;
  int v = op % 6;
  // u even -> prev_op F, L, U
  //
  // u == v :     e.g. Fx followed by Fy
  // v - u == 1 : e.g. Fx followed by By (commutative)

  return (u == v) || (v - u == 1 && (u % 2 == 0));
}

