//
// fsm_test.cpp
//

#define CATCH_CONFIG_MAIN

// in order to access private members, we have to defeat the safety
// mechanisms. This is acceptable for unit testing, but not much
// else. It is also not portable because it is not supported by the
// C++ spec, and therefore might not work with all compilers.

#include "catch.hpp"
#define private public
#include "fsm.hpp"

using namespace std;

// forward declaration of helpers.
FSM fsm_simple();
FSM fsm_moonman();
FSM fsm_brain_bag();
void recognize(FSM& fsm, string input, bool exp, bool is_bogus);


// Unit Tests

TEST_CASE("FSM: constructor", "[constructor]") {
  FSM fsm;
  REQUIRE(fsm.state == -1);
  REQUIRE(fsm.default_state == -1);
}

TEST_CASE("FSM: addState", "[add get state]") {
  FSM fsm;
  int oneID = fsm.addState("one", true);
  REQUIRE(oneID == 0); // first state id should be 0
  REQUIRE(fsm.states.size() == 1); // should have one state only
  REQUIRE(fsm.states[0]->label == "one"); // state label should be 'one'
  REQUIRE(fsm.states[0]->accept); // state should have accept = true
  int twoID = fsm.addState("two");
  REQUIRE(twoID == 1); // next state id should be 1
  REQUIRE(fsm.states.size() == 2); // should have two states now
  REQUIRE(fsm.states[1]->label == "two"); // state label should be 'two'
  REQUIRE_FALSE(fsm.states[1]->accept); // state should have accept = false

  // now get states back
  State* s1 = fsm.getState(oneID);
  REQUIRE(s1 != NULL);
  REQUIRE(s1->label == "one");
  State* s2 = fsm.getState(twoID);
  REQUIRE(s2 != NULL);
  REQUIRE(s2->label == "two");

  // try to fetch states that arent' there
  State* non1 = fsm.getState(-1);
  REQUIRE(non1 == NULL);
  State* non2 = fsm.getState(2);
  REQUIRE(non2 == NULL);  
}

TEST_CASE("FSM: test defaults", "[default and counts]") {
  FSM fsm;
  int current = fsm.state;
  REQUIRE(current == -1); // Default current state should be -1
  current = fsm.getCurrentState();
  REQUIRE(current == -1); // getCurrentState should return fsm.state  
  bool accept = fsm.isAcceptState();
  REQUIRE(accept == false); // Default accept state should be false
  State* bogus = fsm.getState(0);
  REQUIRE(bogus == NULL); // Default getState should give NULL
  int defstate = fsm.default_state;
  REQUIRE(defstate == -1); // Default default state should be -1
  defstate = fsm.getDefaultState();
  REQUIRE(defstate == -1); // getDefaultState should return fsm.default_state
  bool handle = fsm.handleSignal(0);
  REQUIRE(handle == false);  // Default fsm should not be able to handle signals
  int numStates = fsm.countStates();
  REQUIRE(numStates == 0); // Default num states should be 0
  bool numTrans = fsm.countTransitions();
  REQUIRE(numTrans == 0); //  Default num transitions should be 0
}

TEST_CASE("FSM: add states", "[current set state]") {
  // requires [add get state] to pass
  FSM fsm;
  bool accept = fsm.isAcceptState();
  REQUIRE_FALSE(accept); // uninitialized FSM should not be in accept state
  // first state should become the FSM's current state.
  int even = fsm.addState("even", true); 
  int odd = fsm.addState("odd");
  REQUIRE(even >= 0);   // FSM assigned negative ID to state
  REQUIRE(odd >= 0);    // FSM assigned negative ID to state
  REQUIRE(even != odd); // FSM assigned same ID to two states
  int current = fsm.getCurrentState(); // remember, even was first state added.
  REQUIRE(current == even); // Current state is wrong
  accept = fsm.isAcceptState();
  REQUIRE(accept); // Accept state should be true
  fsm.setState(odd);
  current = fsm.getCurrentState();
  REQUIRE(current == odd); // Current state is wrong
  accept = fsm.isAcceptState();
  REQUIRE_FALSE(accept); // Accept state should be false
  int count = fsm.countStates();
  REQUIRE(count == 2); // There should be two states
}

TEST_CASE("FSM: add transitions", "[add transitions]") {
  // requires [add get state] and [default and counts] to pass
  FSM fsm;
  int start = fsm.addState("No Input");
  int m1 = fsm.addState("M");
  int o1 = fsm.addState("O"); 
  int o2 = fsm.addState("O");
  int n1 = fsm.addState("N");
  int m2 = fsm.addState("M");
  int a1 = fsm.addState("A");
  int n2 = fsm.addState("N", true);
  
  int start_tr = fsm.addTransition(start, m1, (int) 'M', "M"); // 1
  REQUIRE(start_tr >= 0); // IDs can't be negative
  Transition* tr = fsm.getTransition(start_tr);
  REQUIRE(tr != NULL); // Transition just added is NULL
  if (tr != NULL) {
    REQUIRE(tr->signal == (int) 'M');// Signal not recorded
    REQUIRE(tr->label == "M"); // Label not recorded
    fsm.addTransition(m1, o1, (int) 'O', "O");    // 2
    fsm.addTransition(o1, o2, (int) 'O', "O");    // 3
    fsm.addTransition(o2, n1, (int) 'N', "N");    // 4
    fsm.addTransition(n1, m2, (int) 'M', "M");    // 5
    fsm.addTransition(m2, a1, (int) 'A', "A");    // 6
    fsm.addTransition(a1, n2, (int) 'N', "N");    // 7

    int num_trans = fsm.countTransitions();
    REQUIRE(num_trans == 7); // Wrong number of transitions

    // now add some transitions that it should reject
    fsm.addTransition(-1, n2, (int) 'X', "X"); // still 7
    num_trans = fsm.countTransitions();
    REQUIRE(num_trans == 7); // Wrong number of transitions
    fsm.addTransition(n1, m2, (int) 'M', "M"); // still 7
    num_trans = fsm.countTransitions();
    REQUIRE(num_trans == 7); // Wrong number of transitions
  }
}

TEST_CASE("FSM: Simple signals", "[handle signal]") {
  // requires all preceding tests to pass
  FSM fsm = fsm_simple(); // accepts even numbers of zeros
  int def = fsm.getDefaultState();
  State* st;
  bool went;
  bool acc;
  went = fsm.handleSignal(1);
  REQUIRE(went); // Should transition on signal
  REQUIRE(fsm.isAcceptState()); // Accept state wrong
  went = fsm.handleSignal(0);
  REQUIRE(went); // Should transition on signal
  REQUIRE_FALSE(fsm.isAcceptState()); // Accept state wrong
  fsm.setState(fsm.getDefaultState());
  REQUIRE(fsm.getCurrentState() == def); // Reset to Default wrong
  acc = fsm.isAcceptState();
  REQUIRE(acc); // Accept state wrong
  bool trial_ok = true;
  for (int i=0; i< 3; i++) {
    fsm.handleSignal(0);
    if (acc == fsm.isAcceptState()) {
      REQUIRE(acc == fsm.isAcceptState()); // Accept state wrong
      trial_ok = false;
      break;
    }
    acc = fsm.isAcceptState();
  }
  if (trial_ok) {
    REQUIRE(trial_ok); // Trial for zeros OK
  }
  fsm.setState(fsm.getDefaultState());
  acc = fsm.isAcceptState();
  trial_ok = true;
  for (int i=0; i< 3; i++) {
    fsm.handleSignal(1);
    if (acc != fsm.isAcceptState()) {
      REQUIRE(acc != fsm.isAcceptState()); //  Accept state wrong
      trial_ok = false;
      break;
    }
    acc = fsm.isAcceptState();
  }
  if (trial_ok) {
    REQUIRE(trial_ok); // Trial for ones OK
  }
  went = fsm.handleSignal(42);
  REQUIRE_FALSE(went); // Shouldn't go anywhere for that input
}

TEST_CASE("FSM: moonman test", "[moonman]") {
  // this test makes a text recognizer that is accepting when it
  // recieves the input 'MOONMAN'.
  FSM moonman = fsm_moonman();
  State* st;
  moonman.handleSignal((int) 'M');
  REQUIRE_FALSE(moonman.isAcceptState()); // Shouldn't accept yet
  st = moonman.getState(moonman.getCurrentState());
  REQUIRE(st != NULL); //  Current state pointer is NULL
  moonman.handleSignal((int) 'O');
  moonman.handleSignal((int) 'O');
  moonman.handleSignal((int) 'N');
  moonman.handleSignal((int) 'M');
  moonman.handleSignal((int) 'A');
  moonman.handleSignal((int) 'N');
  REQUIRE(moonman.isAcceptState()); // Should accept now
  st = moonman.getState(moonman.getCurrentState());
  REQUIRE(st != NULL); // Current state pointer is NULL
  moonman.setState(moonman.getDefaultState());
  REQUIRE_FALSE(moonman.isAcceptState()); // Shouldn't accept yet
  bool went;
  went = moonman.handleSignal((int) 'Q');
  REQUIRE(went); // Transition on weirdo signals because of failure transition
  REQUIRE_FALSE(moonman.isAcceptState()); // Shouldn't accept yet
  State* cur = moonman.getState(moonman.getCurrentState());
  REQUIRE(cur->label == "Not MOONMAN");
}

TEST_CASE("FSM: brain bag", "[brain bag]") {
  // this test case makes a text recognizer that can recognize several
  // sequences of input, including "BRAIN" and "BAG", ergo the name.
  FSM brain_bag = fsm_brain_bag();
  recognize(brain_bag, "MONKEY", false, true);
  recognize(brain_bag, "BIN", true, false);
  recognize(brain_bag, "BINS", true, false);
  recognize(brain_bag, "BA", false, false);
  recognize(brain_bag, "BAA", false, true);
  recognize(brain_bag, "BRAIN", true, false);
  recognize(brain_bag, "BRAINS", true, false);
  recognize(brain_bag, "BUS", false, true);
}

FSM fsm_simple() {
  FSM fsm;
  int even = fsm.addState("Even", true);
  int odd = fsm.addState("Odd");
  fsm.addTransition(even, even, 1, "1");
  fsm.addTransition(even, odd, 0, "0");
  fsm.addTransition(odd, odd, 1, "1");
  fsm.addTransition(odd, even, 0, "0");
  return fsm;
}

FSM fsm_moonman() {
  // the moonman FSM will be terminally stuck in a bogus state if it
  // gets input that it doesn't expect. The only way to get into the
  // accepting state is to feed it "MOONMAN".
  FSM fsm;
  int start = fsm.addState("No Input"); // this should be the default state now.
  int bogus = fsm.addState("Not MOONMAN");
  int m1 = fsm.addState("M");
  int o1 = fsm.addState("O"); 
  int o2 = fsm.addState("O");
  int n1 = fsm.addState("N");
  int m2 = fsm.addState("M");
  int a1 = fsm.addState("A");
  int n2 = fsm.addState("N", true);
  
  fsm.addTransition(start, m1, (int) 'M', "M"); // 1
  fsm.addTransition(m1, o1, (int) 'O', "O");    // 2
  fsm.addTransition(o1, o2, (int) 'O', "O");    // 3
  fsm.addTransition(o2, n1, (int) 'N', "N");    // 4
  fsm.addTransition(n1, m2, (int) 'M', "M");    // 5
  fsm.addTransition(m2, a1, (int) 'A', "A");    // 6
  fsm.addTransition(a1, n2, (int) 'N', "N");    // 7

  fsm.addTransition(start, bogus, FAILURE_SIGNAL, "X"); // 8
  fsm.addTransition(m1, bogus, FAILURE_SIGNAL, "X"); // 9
  fsm.addTransition(o1, bogus, FAILURE_SIGNAL, "X"); // 10
  fsm.addTransition(o2, bogus, FAILURE_SIGNAL, "X"); // 11
  fsm.addTransition(n1, bogus, FAILURE_SIGNAL, "X"); // 12
  fsm.addTransition(m2, bogus, FAILURE_SIGNAL, "X"); // 13
  fsm.addTransition(a1, bogus, FAILURE_SIGNAL, "X"); // 14
  fsm.addTransition(n2, bogus, FAILURE_SIGNAL, "X"); // 15
  
  return fsm;
}

FSM fsm_brain_bag() {
  FSM fsm;
  int S  = fsm.addState("Start State");
  int Z  = fsm.addState("Bogus State");
  int b  = fsm.addState("B");
  int i  = fsm.addState("I");
  int r  = fsm.addState("R");
  int a  = fsm.addState("A");
  int n  = fsm.addState("N", true);
  int g  = fsm.addState("G", true);
  int a2 = fsm.addState("A", true);
  int s  = fsm.addState("S", true);
  int i2 = fsm.addState("I");
  int n2 = fsm.addState("N", true);
  fsm.addTransition(S, b, (int) 'B', "B");
  fsm.addTransition(b, i, (int) 'I', "I");
  fsm.addTransition(b, r, (int) 'R', "R");
  fsm.addTransition(b, a, (int) 'A', "A");
  fsm.addTransition(i, n, (int) 'N', "N");
  fsm.addTransition(i, g, (int) 'G', "G");  
  fsm.addTransition(r, a2, (int) 'A', "A");
  fsm.addTransition(a, g, (int) 'G', "G");
  fsm.addTransition(n, s, (int) 'S', "S");
  fsm.addTransition(a2, s, (int) 'S', "S");
  fsm.addTransition(a2, i2, (int) 'I', "I");
  fsm.addTransition(i2, n2, (int) 'N', "N");
  fsm.addTransition(n2, s, (int) 'S', "S");
  fsm.addTransition(S, Z, -1, "Bogus");
  fsm.addTransition(b, Z, -1, "Bogus");
  fsm.addTransition(i, Z, -1, "Bogus");
  fsm.addTransition(r, Z, -1, "Bogus");
  fsm.addTransition(a, Z, -1, "Bogus");
  fsm.addTransition(n, Z, -1, "Bogus");
  fsm.addTransition(g, Z, -1, "Bogus");
  fsm.addTransition(a2, Z, -1, "Bogus");
  fsm.addTransition(s, Z, -1, "Bogus");
  fsm.addTransition(i2, Z, -1, "Bogus");
  fsm.addTransition(n2, Z, -1, "Bogus");
  return fsm;
}

/**
 * In case you are curious: this treats the FSM like a text input
 * recognizer. It resets the FSM to a default state and then processes
 * each character in the input string. If at the end of the string it
 * is in an accepting state, it means the FSM matched that string.
 *
 * the 'exp' param tells us if we actually expect it to recognize it.
 *
 * the 'is_bogus' param tells us if we expect the FSM to go into a
 * terminally bogus state, meaning it absolutely won't recognize the
 * input, no matter what comes next.
 *
 * It is possible for it to be in an unaccepted state without being
 * bogus. For example the string "BA" is not accepted, but if the next
 * character is "G", then the FSM would be in an accepted state.
 **/
void recognize(FSM& fsm, string input, bool exp, bool is_bogus) {
  fsm.setState(fsm.getDefaultState());
  State* st;
  st = fsm.getState(fsm.getCurrentState());
  REQUIRE(st != NULL); // Current State should not be NULL
  //  cout << "Initial state: " << st << endl;
  string::iterator it = input.begin(); 
  for(; it != input.end(); it++) {
    char c = *it;
    int sig = (int) c;
    fsm.handleSignal(sig);
    st = fsm.getState(fsm.getCurrentState());
    //    cout << "After " << c << ", current state: " << st << endl;
  }
  REQUIRE(fsm.isAcceptState() == exp); // Wrong Accept State
  if (is_bogus) {
    st = fsm.getState(fsm.getCurrentState());
    REQUIRE(st->label == "Bogus State"); // Should be in Bogus state
  }
}

void homeworkEpisodeCode() {
  FSM fsm;
  int sA = fsm.addState("State A", false);
  int sB = fsm.addState("State B", true);
  int tAB42 = fsm.addTransition(sA, sB, 42, "42");
  int tBA23 = fsm.addTransition(sB, sA, 23, "23");

  fsm.handleSignal(42);
  cout << "in an accepting state? " << fsm.isAcceptState() << endl;
  
  cout << "after handling signal 42, graph is: " << endl << &fsm << endl;
  fsm.handleSignal(99);
  cout << "after handling signal 99, graph is: " << endl << &fsm << endl;

  int failState = fsm.addState("Fail", false);
  fsm.addTransition(sB, failState, FAILURE_SIGNAL, "failure signal");
  cout << "added failure state. graph is now " << endl << &fsm << endl;
  fsm.handleSignal(99);
  cout << "after handling signal 99 again, graph is: " << endl << &fsm << endl;
}

TEST_CASE("FSM: homeworkEpisodeCode", "[hw]") {
  homeworkEpisodeCode();
}
