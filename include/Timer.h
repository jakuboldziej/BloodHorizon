#pragma once

class Timer {
 public:
  Timer(float length) : length(length), time(0), timeout(false) {}

  bool step(float deltaTime) {
    time += deltaTime;
    if (time >= deltaTime) {
      time -= length;
      timeout = true;
      return true;
    }
    return false;
  }

  bool isTimeout() const { return timeout; }
  float getTime() const { return time; }
  float getLength() const { return length; }
  void reset() { time = 0, timeout = false; }

 private:
  float length, time;
  bool timeout;
};