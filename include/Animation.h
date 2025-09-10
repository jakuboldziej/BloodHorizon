#pragma once

class Animation {
 public:
  Animation() : frameCount(0), framesPerSpriteFrame(10), frameCounter(0) {}

  Animation(int frameCount, int framesPerSpriteFrame)
      : frameCount(frameCount), framesPerSpriteFrame(framesPerSpriteFrame), frameCounter(0) {}

  Animation(int frameCount, float length) : frameCount(frameCount), frameCounter(0) {
    framesPerSpriteFrame = static_cast<int>((length * 60.0f) / frameCount);
    if (framesPerSpriteFrame < 1)
      framesPerSpriteFrame = 1;
  }

  float getLength() const { return (frameCount * framesPerSpriteFrame) / 60.0f; }
  float getTime() const { return frameCounter / 60.0f; }
  int getFrameCount() const { return frameCount; }

  int currentFrame() const {
    if (frameCount <= 0)
      return 0;

    int spriteFrame = frameCounter / framesPerSpriteFrame;

    return spriteFrame % frameCount;
  }

  void step(float deltaTime) {
    frameCounter++;

    int totalFrames = frameCount * framesPerSpriteFrame;
    if (frameCounter >= totalFrames) {
      frameCounter = 0;
    }
  }

  void reset() {
    frameCounter = 0;
  }

  bool isDone() const {
    int totalFrames = frameCount * framesPerSpriteFrame;
    return frameCounter >= totalFrames;
  }

 private:
  int frameCount;
  int framesPerSpriteFrame;
  int frameCounter;
};