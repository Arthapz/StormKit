#import "ObjCBridge.h"

#if __cplusplus
extern "C"
#endif
  void* toCPtr(CALayer* layer) {
    return (__bridge void*)(layer);
}
