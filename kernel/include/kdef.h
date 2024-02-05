#ifndef KDEF_H__
#define KDEF_H__

typedef unsigned long long paddr_t;

// whenever a kernel function returns `int` as a status, that value should be
// checked against these generic return codes.
// otherwise it will return a module-specific enum of return codes.
enum generic_rc {
	GRC_OK = 0,
	GRC_OUT_OF_BOUNDS,
	GRC_FAULTY_DEV,
};

#endif
