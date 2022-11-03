#pragma once
#include <vector>
#include <utility>
#include "_main.hxx"

using std::vector;
using std::move;




// RAK-OPTIONS
// -----------

struct RakOptions {
  int  repeat;
  int  maxIterations;

  RakOptions(int repeat=1, int maxIterations=500) :
  repeat(repeat), maxIterations(maxIterations) {}
};




// RAK-RESULT
// ----------

template <class K>
struct RakResult {
  vector<K> membership;
  int   iterations;
  float time;

  RakResult(vector<K>&& membership, int iterations=0, float time=0) :
  membership(membership), iterations(iterations), time(time) {}

  RakResult(vector<K>& membership, int iterations=0, float time=0) :
  membership(move(membership)), iterations(iterations), time(time) {}
};
