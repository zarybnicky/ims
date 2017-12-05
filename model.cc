// Cost Efficiency versus Quality of Service in Autoscaling Web Services
// xzaryb00fit.vutbr.cz

#include "model.hh"

const int TIMEOUT = 20;
const int SERVICE = 10;
const int SPOOL_UP = 100;

Store replicas("Online replicas", 1);
Histogram hgram("Queue time", 0, 1, SERVICE + TIMEOUT + 1);
long left = 0;

int main() {
  //DebugON();
  Print("Web service scaling model (xzaryb00)\n");
  SetOutput("model.out");
  Init(0, 1000);
  (new LinearGenerator(10))->Activate();
  (new AutoScaler(75, 85))->Activate();
  Run();
  Print("Počet netrpělivých zákazníků: %d \n", left);
  replicas.Output();
  hgram.Output();
  return 0;
}


void AutoScaler::Behavior() {
  // Handle replicas started in the previous iteration
  if (queued > 0) {
    replicas.SetCapacity(replicas.Capacity() + queued);
    queued = 0;
  }

  double utilization = (replicas.Used() + replicas.Q->Length()) / replicas.Capacity();
  if (utilization < bottom) {
    replicas.SetCapacity(replicas.Capacity() - floor(bottom / utilization));
  } else if (utilization > top) {
    queued = ceil(utilization / top);
  }
  Activate(Time + SPOOL_UP);
};


void LinearGenerator::Behavior() {
  (new Request(TIMEOUT))->Activate();
  Activate(Time + Exponential(meanTime));
}

void BurstGenerator::Behavior() {
  (new Request(TIMEOUT))->Activate();
  Activate(Time + delay);
}

void Burst::Behavior() {
  if (--amount < 0) {
    Out();
  } else {
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
      current+= step;
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
  left++;
  Cancel();
}

void Request::Behavior() {
  double arrival = Time;
  if (timeout > 0) {
    Event *t = new Timeout(timeout, this);
    Enter(replicas);
    delete t;
  } else {
    Enter(replicas);
  }
  Wait(SERVICE);
  Leave(replicas);
  hgram(Time - arrival);
}
