#include "vcu.h"

void VCU::shutdown_loop() {
	VCU::flag = 1;
}
