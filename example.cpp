//
// Created by hrh on 1/4/23.
//
#include "stdio.h"
#include "mcs/api.h"

int plus(int a, int b) {
  return a + b;
}
MCS_REMOTE(plus);

int main() {
  mcs::Init();

  return 0;
}