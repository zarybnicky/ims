// Cost Efficiency versus Quality of Service in Autoscaling Web Services
// xzaryb00fit.vutbr.cz

#include "model.hh"

const int TIMEOUT = 0;
const int SERVICE = 10;
const int SPOOL_UP = 100;
const double COST_PER_REPLICA_PER_TICK = 1;

Store replicas("Online replicas", 10);
Histogram hgram("Queue time", 0, 1, SERVICE + TIMEOUT + 1);
TStat cost("Cost per auto-scaler frame");
double accumCost = 0;

long dropped = 0;

int main() {
  //DebugON();
  Print("Web service scaling model (xzaryb00)\n");
  //SetOutput("model.out");
  Init(0, 1000);
  //(new RandomGenerator(1))->Activate();
  (new BurstGenerator(50, 2, 50))->Activate();
  (new AutoScaler(0, 0, .5, .8))->Activate(Time + SERVICE);
  Run();
  Print("Accumulated cost: %.2f \n", accumCost);
  //cost.Output();
  Print("Dropped requests: %d \n", dropped);
  hgram.Output();
  replicas.Output();
  return 0;
}


void AutoScaler::Behavior() {
  double curCost = (Time + SERVICE + 1 - prevTime) * replicas.Capacity() * COST_PER_REPLICA_PER_TICK;
  cost(curCost);
  accumCost += curCost;
  prevTime = Time;

  double utilization = (replicas.Used() + replicas.Q->Length()) / (double)replicas.Capacity();
  Print("%.2f %u %u\n", utilization, replicas.Used() + replicas.Q->Length(), replicas.Capacity());
  if (utilization < bottom) {
    unsigned r = ceil(replicas.Capacity() * (utilization / bottom));
    if (min > 0 && min > r) {
      r = min;
    }
    replicas.SetCapacity(r);
  } else if (utilization > top) {
    unsigned n = replicas.Capacity() + ceil(utilization / top);
    if (n == 0) {
      n = 1;
    }
    if (max > 0 && n > max) {
      n = max;
    }
    (new SpoolUp(SPOOL_UP, n - replicas.Capacity()))->Activate();
  }
  Activate(Time + SERVICE + 1);
};

void SpoolUp::Behavior() {
  replicas.SetCapacity(replicas.Capacity() + n);
}


void SteadyGenerator::Behavior() {
  (new Request(TIMEOUT))->Activate();
  Activate(Time + delay);
}

void RandomGenerator::Behavior() {
  (new Request(TIMEOUT))->Activate();
  Activate(Time + Exponential(meanTime));
}

void BurstGenerator::Behavior() {
  (new Burst(amount, delay))->Activate();
  Activate(Time + Exponential(meanTime));
}

void Burst::Behavior() {
  if (--amount >= 0) {
    (new Request(TIMEOUT))->Activate();
    Activate(Time + delay);
  }
}

void VariedGenerator::Behavior() {
  (new Request(TIMEOUT))->Activate();
  Activate(Time + Exponential(current));
  if (increasing) {
    if (current + step >= max) {
      current = max;
      increasing = false;
    } else {
      current += step;
    }
  } else {
    if (current - step <= min) {
      current = min;
      increasing = true;
    } else {
      current -= step;
    }
  }
}


// Class Timeout - taken directly from SIMLIB examples
// (how many other 'timeout' implementations can there be?)
void Timeout::Behavior() {
  ptr->Out();
  delete ptr;
  dropped++;
  Cancel();
}

void Request::Behavior() {
  double arrival = Time;
  Event *t = NULL;
  if (timeout > 0) {
    t = new Timeout(timeout, this);
  }
  if (replicas.Capacity() == 0) {
    replicas.QueueIn(this, 1);
    Passivate();
  } else {
    Enter(replicas);
  }
  if (t != NULL) {
    delete t;
  }
  Wait(SERVICE);
  Leave(replicas);
  hgram(Time - SERVICE - arrival);
}
