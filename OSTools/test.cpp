#include "oslib/oslib.h"
#ifdef PSP
#include <psputils.h>
#endif

PSP_MODULE_INFO("OSLib Sample", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

int main()
{
	oslQuit();
	return 0;
}


