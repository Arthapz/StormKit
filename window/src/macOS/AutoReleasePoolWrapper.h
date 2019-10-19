#pragma once

#ifdef __cplusplus
extern "C" {
#endif
	void ensureThreadHasPool();
	void drainThreadPool();
#ifdef __cplusplus
}
#endif
