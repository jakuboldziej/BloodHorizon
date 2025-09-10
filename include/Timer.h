class Timer {
 public:
  Timer(float length) : length(length), time(0), timeout(false) {}

  bool step(float deltaTime) {
    time += deltaTime;
    if (time >= length) {
      timeout = true;
      return true;
    }
    timeout = false;
    return false;
  }

  bool isTimeout() const { return timeout; }
  float getTime() const { return time; }
  float getLength() const { return length; }
  void reset() { time = 0.0f, timeout = false; }

 private:
  float length, time;
  bool timeout;
};