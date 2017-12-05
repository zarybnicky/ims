#include "simlib/simlib.h"
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


class LinearGenerator : public Event {
  int meanTime;
public:
  explicit LinearGenerator(int m): meanTime(m) {}
  void Behavior();
};

class BurstGenerator : public Event {
  int amount;
  int delay;
public:
  explicit BurstGenerator(int l, int d): amount(l), delay(d) {}
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
  explicit VariedGenerator(int a, int b, int s): min(a), max(b), step(s), current(a) {};
  void Behavior();
};


class AutoScaler : public Process {
  double bottom, top;
  int queued = 0;
public:
  explicit AutoScaler(double b, double t): bottom(b), top(t) {}
  void Behavior();
};
