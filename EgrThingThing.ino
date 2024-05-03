
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
const int trigPin = 5;
const int echoPin = 3;
const int leftPin = 9;
const int rightPin = 8;
const int sigStop = 90;
const int sigFwd = 180;
Servo left;
Servo right;
void spinLeft() {
    left.write(sigFwd);
}
void stopLeft() {
    left.write(sigStop);
}
void spinRight() {
    right.write(0);
}
void stopRight() {
    right.write(91);
}
void reverseRight() {
    right.write(180);
}

double calDist() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    double distance = duration / 74.0 / 2.0;
    return distance;
}

StateMachine state;
void setup() {
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    // pinMode(leftPin, OUTPUT);
    // pinMode(rightPin, OUTPUT);
    // for (int i = 2; i <= 10; i++) {
    //     pinMode(i, OUTPUT);
    // }
    // digitalWrite(leftPin, HIGH);
    left.attach(leftPin);
    right.attach(rightPin);
    // delay(10000);
    // left.attach(9);
    Serial.println("yo");
    // right.attach(10);
}

int i = 0;
void loop() {
    double dist = calDist();
    switch (state.getState()) {
    case States::PowerOn:
        stopLeft();
        stopRight();
        if (dist < 4 && state.getTime() > 5000) {
            Serial.println("Switching to drive state");
            state.changeState(States::Drive);
        }
        break;
    case States::Drive:
        spinLeft();
        spinRight();
        if (dist < 4) {
            Serial.println("Switching to blocked state");
            state.changeState(States::Blocked);
        }
        break;
    case States::Blocked:
        spinLeft();
        reverseRight();
        if (dist > 4 && state.getTime() > 1000) {
            Serial.println("Switching to drive state");
            state.changeState(States::Drive);
        }
        if (state.getTime() > 5000) {
            Serial.println("Switching to sleep state");
            state.changeState(States::Sleep);
        }
        break;
    case States::Sleep:
        stopLeft();
        stopRight();
        state.sleep(100);
        if (dist < 4 && state.getTime() > 5000) {
            Serial.println("Switching to sleep block state");
            state.changeState(States::SleepBlocked);
        }
        break;
    case States::SleepBlocked:
        if (dist > 4) {
            Serial.println("Switching to sleep state");
            state.changeState(States::Sleep);
        }
        if (state.getTime() > 2000) {
            Serial.println("Switching to drive state");
            state.changeState(States::Drive);
        }
        break;
    }
    state.sleep(50);
}
