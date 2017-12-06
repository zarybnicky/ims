#include "simlib/simlib.h"
#include "simlib/internal.h"
#include <cmath>

class Timeout : public Event {
  Process *ptr;
public:
  Timeout(double t, Process *p): ptr(p) {
    Activate(Time + t);
  };
  void Behavior();
};

class Request : public Process {
  int timeout;
public:
  explicit Request(int t): timeout(t) {}
  void Behavior();
};


class SteadyGenerator : public Event {
  int delay;
public:
  explicit SteadyGenerator(int m): delay(m) {}
  void Behavior();
};

class RandomGenerator : public Event {
  int meanTime;
public:
  explicit RandomGenerator(int m): meanTime(m) {}
  void Behavior();
};

class BurstGenerator : public Event {
  int amount, delay, meanTime;
public:
  explicit BurstGenerator(int l, int d, int m):
    amount(l), delay(d), meanTime(m) {}
  void Behavior();
};

class Burst : public Event {
  int amount;
  int delay;
public:
  explicit Burst(int l, int d): amount(l), delay(d) {}
  void Behavior();
};

class VariedGenerator : public Event {
  int min, max, step, current;
  bool increasing = true;
public:
  explicit VariedGenerator(int a, int b, int s):
    min(a), max(b), step(s), current(a) {};
  void Behavior();
};


class AutoScaler : public Event {
  unsigned int min, max;
  double bottom, top, prevTime = 0;
public:
  explicit AutoScaler(int _min, int _max, double b, double t):
    min(_min), max(_max), bottom(b), top(t) {}
  void Behavior();
};

class SpoolUp : public Process {
  int n;
public:
  explicit SpoolUp(int delay, int _n): n(_n) {
    Activate(Time + delay);
  }
  void Behavior();
};
