#ifndef VCU_H_
#define VCU_H_

class VCU {
public:
	volatile int flag;
	void shutdown_loop();
};

#endif
