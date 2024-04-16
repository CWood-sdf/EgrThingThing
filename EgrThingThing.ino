
enum class States {
    PowerOn,
    Drive,
    Blocked,
    Sleep,
    SleepBlocked,
};

class StateMachine {
    uint64_t timeIn = 0;
    States currentState = States::PowerOn;

public:
    void changeState(States state) {
        timeIn = 0;
        currentState = state;
    }
    States getState() {
        return currentState;
    }
    uint64_t getTime() {
        return timeIn;
    }
    void incTime(int time) {
        timeIn += time;
    }
    void sleep(int time) {
        incTime(time);
        delay(time);
    }
};

#include <Servo.h>
const int trigPin = 12;
const int echoPin = 11;
Servo left;
Servo right;
void spinLeft() {
    left.write(90);
}
void stopLeft() {
    left.write(0);
}
void spinRight() {
    right.write(90);
}
void stopRight() {
    right.write(0);
}

double calDist() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    double distance = duration * 0.034 / 2.0;
    return distance;
}

StateMachine state;
void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    left.attach(9);
    right.attach(10);
}

void loop() {
    double dist = calDist();
    Serial.println(dist);
    switch (state.getState()) {
    case States::PowerOn:
        if (dist < 100) {
            state.changeState(States::Drive);
        }
        break;
    case States::Drive:
        spinLeft();
        spinRight();
        if (dist < 10) {
            state.changeState(States::Blocked);
        }
        break;
    case States::Blocked:
        stopLeft();
        spinRight();
        if (dist > 10) {
            state.changeState(States::Drive);
        }
        if (state.getTime() > 2000) {
            state.changeState(States::Sleep);
        }
        break;
    case States::Sleep:
        state.sleep(100);
        if (dist < 10) {
            state.changeState(States::SleepBlocked);
        }
        break;
    case States::SleepBlocked:
        if (dist > 10) {
            state.changeState(States::Sleep);
        }
        if (state.getTime() > 2000) {
            state.changeState(States::Drive);
        }
        break;
    }
    state.sleep(50);
}
