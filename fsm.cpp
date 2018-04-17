//
// fsm.cpp
//

#include "fsm.hpp"

using namespace std;

FSM::FSM() {
  // TODO
}

int FSM::addState(string label, bool is_accept_state) {
  // TODO
  return 0;
}

int FSM::addState(string label) {
  // TODO
  return 0;
}

int FSM::addTransition(int stateA, int stateB, 
		       int signal, string transLabel) {
  // this method's documentation is longer than the implementation. to
  // help out, I'm including the comments for my implementation.
  //
  // 1. if stateA or stateB is out of range for the states vector,
  // return -1.
  //
  // 2. get the State* for stateA and look for a duplicate
  // transition. if the new transition is FAILURE_SIGNAL, only need to
  // check the state's `failure_trans`. if it is a regular signal,
  // need to look at each id in the state's `trans` vector. if there's
  // a duplicate, return -1.
  //
  // 3. build a new Transition, and set its values.
  //
  // 4. add the new transition to the FSM and get its ID.
  //
  // 5. using the State* from step 2, use the new transition ID to
  // either set the failure_trans or add to the trans list.
  //
  // 6. return the new transition's ID.

  // TODO
  return 0;
}

int FSM::countStates() {
  // TODO
  return 0;
}

int FSM::countTransitions() {
  // TODO
  return 0;
}

int FSM::getCurrentState() {
  // TODO
  return 0;
}

bool FSM::isAcceptState() {
  // TODO
  return false;
}

State* FSM::getState(int id) {
  // TODO
  return NULL;
}

Transition* FSM::getTransition(int id) {
  // TODO
  return NULL;
}

int FSM::getDefaultState() {
  // TODO
  return 0;
}

void FSM::setState(int id) {
  // TODO  
}

bool FSM::handleSignal(int signal) {
  // like addTransition, the documentation is longer than the
  // implementation. Here's my pseudocode:
  //
  // 1. If the FSM is currently in a bad state, return false.
  //
  // 2. Iterate through the current state's normal transitions and
  // look for one that has the same signal as the one given to the
  // method.
  //
  // 3. If there wasn't a normal transition, see if there was a
  // failure_trans transition for the state. If not, return false.
  //
  // 4. If there was a normal or a failure transition, update the FSM
  // to be in the next_state indicated by that transition, and return
  // true.

  // TODO
  return false;
}

ostream &operator << (ostream& out, FSM* fsm) {
  int c = 0;
  for (auto it=fsm->states.begin(); it != fsm->states.end(); ++it) {
    out << "  state " << c << ": " << *it << endl;
    c++;
  }
  c = 0;
  for (auto it=fsm->transitions.begin(); it != fsm->transitions.end(); ++it) {
    out << "  trans " << c << ": " << *it << endl;
    c++;
  }
  out << "  current state: " << fsm->state;
  if (fsm->isAcceptState()) {
    out << " (accepting state!)";
  }
  out << endl;
  return out;
}

ostream &operator << (ostream& out, Transition* tr) {
  if (tr == NULL) {
    out << "Transition NULL";
  } else {
    out << "\"" << tr->label << "\" (" << tr->signal << ") --> " << tr->next_state;
  }
  return out;
}

ostream &operator << (ostream& out, State* st) {
  if (st == NULL) {
    out << "State: NULL";
  } else {
    out << "State: " << st->label;
    if (st->accept) {
      out << " +";
    }
  }
  return out;
}
