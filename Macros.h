#ifndef MACROS_H
#define MACROS_H

#define LERP(startValue, endValue, frameNo, startFrameNo, endFrameNo)\
startValue + ((endValue - startValue) * (frameNo - startFrameNo)) / (endFrameNo - startFrameNo)
//#define LERP(l, h, a)  l + (h - l) * a

#define CLAMP(x, a, b) x > b ? b : (x < a ? a : x)


#endif // MACROS_H
