//   ____  __  __ _  __  ____  ____               
//  (  __)(  )(  ( \(  )(_  _)(  __)              
//   ) _)  )( /    / )(   )(   ) _)               
//  (__)  (__)\_)__)(__) (__) (____)              
//   ____  ____  __  ____  ____                   
//  / ___)(_  _)/ _\(_  _)(  __)                  
//  \___ \  )( /    \ )(   ) _)                   
//  (____/ (__)\_/\_/(__) (____)                  
//   _  _   __    ___  _  _  __  __ _  ____  ____ 
//  ( \/ ) / _\  / __)/ )( \(  )(  ( \(  __)/ ___)
//  / \/ \/    \( (__ ) __ ( )( /    / ) _) \___ \
//  \_)(_/\_/\_/ \___)\_)(_/(__)\_)__)(____)(____/

#ifndef __fsm_h__
#define __fsm_h__

#include <string>
#include <iostream>
#include <vector>
#include <iostream>

#define FAILURE_SIGNAL -1

using namespace std;

// forward declarations
class State;
class Transition;

class FSM {
private:

  vector<State*> states; // A state's index is its ID

  vector<Transition*> transitions; // A transition's index is its ID

  int state; // the current state of the FSM. Default should be -1

  int default_state; // default state of the FSM. Default should be
		     // -1. This can be used to reset the FSM with
		     // setState(getCurrentState()).

public:

  // FSM constructs a finite state machine with default
  // values. Initialize some variables as described above.
  FSM();


  // addState creates a new State. The new state:
  //   -- has the given label
  //   -- has the given accept field
  //   -- has an initial failure_trans to -1
  //   -- has zero initial transition IDs.
  //
  // The state is installed at the end of the FSM's `state` list. The
  // newly added state's index in the `state` list is thereafter used
  // as that state's ID.
  //
  // That id is returned.
  //
  // If this is the first state added to the FSM (e.g. its ID is 0),
  // two things happen:
  //   -- the FSM enters this state
  //   -- the FSM's default state is set to its ID
  int addState(string label, bool is_accept_state);

  // addState adds a state with the given label. This is equivalent to
  // addState(label, false). See the other addState function for
  // remaining documentation.
  int addState(string label);

  // addTransition might create a new unique Transition between two
  // states that activates when the given signal is received.
  //
  // If such a transition would not be unique (i.e. it is a
  // duplicate), this returns -1 without modifying the FSM.
  //
  // This is considered duplicate if there is another transition from
  // stateA to stateB with the same signal. The transition label is
  // ignored when determining duplicate status.
  // 
  // If either state is not present in the FSM, nothing is done and -1
  // is returned.
  //
  // Otherwise, a new transition is installed at the end of the FSM's
  // `transitions` list. Its ID is thereafter the index in that
  // list. That id is returned.
  //
  // stateA: the start state's id. 
  //
  // stateB: the end state's id. 
  //
  // signal: the signal that (assuming the FSM is in stateA) will
  //         trigger this transition. The FAILURE_SIGNAL is a
  //         catch-all for bad input received while in stateA. If this
  //         is the failure signal, stateA's `failure_trans` will be
  //         set to the new transition's ID. If this is not a failure
  //         transitions, the new transition will be installed in
  //         stateA's `trans` list.
  //
  // transLabel: the label for this transition. 
  int addTransition(int stateA, int stateB, 
		    int signal, string transLabel);

  // countState returns the number of states this FSM has.
  int countStates();

  // countTransitions returns the number of transitions this FSM has.
  int countTransitions();

  // getCurrentState returns the FSM's current state, or -1 if the FSM
  // has no current state.
  int getCurrentState();

  // isAcceptState returns true if the current state is 'acceptable',
  // false otherwise. If the FSM has no states,this returns false and
  // doesn't explode on a null reference.
  bool isAcceptState();

  // getState returns a pointer to the State object with the given id,
  // or NULL if no such state is in the FSM.
  State* getState(int id);

  // getTransition eturns a pointer to the Transition object with the
  // given id, or NULL if no such transition is in the FSM.
  Transition* getTransition(int id);

  // getDefaultState returns the default state's ID for this FSM. If
  // there isn't default state (e.g. the FSM has no state data yet)
  // this returns -1.
  int getDefaultState();

  // setState sets the state to the given value.
  void setState(int id);

  // handleSignal attempts to find a normal or failure transition for
  // the FSM's current state using the provided input event signal.
  //
  // If there is a match, that transition is taken, the FSM enters the
  // state on the other end of the transition, and returns true.
  //
  // If no transition was taken this returns false.
  bool handleSignal(int signal);

  // for user-friendly debugging output
  friend ostream &operator << (ostream& out, FSM* fsm);
}; // end class FSM

class State {
public:
  bool accept;  // true if the state is an 'accepting' state

  string label; // state's text label. Only used to debug.

  int failure_trans; // if non-negative, this is the transition used
		     // when we receive a signal while in this state
		     // but don't have any matching transitions in the
		     // 'trans' vector for the input signal.

  vector<int> trans; // normal transition ids are stored here.

  // operator << is used to send a State reference to an output
  // stream.
  friend ostream &operator << (ostream& out, State* state);
};

class Transition {
public:
  string label;   // label for this transition. a debugging var.
  int signal;     // signal this transition reacts to
  int next_state; // id of the state we transition to when activated
  friend ostream &operator << (ostream& out, Transition* trans);

};

#endif
