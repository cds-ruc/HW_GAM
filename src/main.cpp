//
// Created by hrh on 1/4/23.
//
#include <iostream>
#include "mcs/api.h"

int plus(int a, int b) {
  return a + b;
}
MCS_REMOTE(plus);

int main() {
  mcs::Init();
  auto task_object = mcs::Task(plus).Remote(1, 2);

  sleep(1);
  int task_result = *(mcs::Get(task_object));
  std::cout << "task_result = " << task_result << std::endl;

  return 0;
}