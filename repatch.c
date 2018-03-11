//
// SilicaTeam 2.0  @dots_tb @SiIicaAndPina ‏
// RePatch -- PATCHING WITH FREEDOM

// Based off ioPlus by @dots_tb: https://github.com/CelesteBlue-dev/PSVita-RE-tools/tree/master/ioPlus/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <vitasdkkern.h>
#include <taihen.h>


#define printf ksceDebugPrintf

static int hooks_uid;
static tai_hook_ref_t ref_hooks;


static int ksceIoOpenForPid_patched(SceUID pid, const char *filename, int flag, SceIoMode mode) {
	int ret = -1, state;
	ENTER_SYSCALL(state);
	if (memcmp("app0:", filename, sizeof("app0:") - 1)==0) {
		char new_path[128];
		char titleid[32];
		ret = ksceKernelGetProcessTitleId(pid, titleid, sizeof(titleid));
		if(ret > -1) {
			strncat(new_path, titleid, sizeof(titleid));
			char *old_path = strchr(filename, ':') + 1;
			if(old_path[0] == '/')
				old_path++;
			snprintf(new_path, sizeof(new_path), "ux0:/rePatch/%s/%s",titleid,old_path);
			ret = ksceIoOpen(new_path, flag, mode);
		}
			
	}
	if(ret < 0) ret = TAI_CONTINUE(int, ref_hooks, pid, filename, flag, mode);
	EXIT_SYSCALL(state);
	return ret;
}

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start(SceSize argc, const void *args) {
	hooks_uid = taiHookFunctionExportForKernel(KERNEL_PID, &ref_hooks, "SceIofilemgr", TAI_ANY_LIBRARY, 0xC3D34965, ksceIoOpenForPid_patched);
	return SCE_KERNEL_START_SUCCESS;
}

int module_stop(SceSize argc, const void *args) {
	if (hooks_uid >= 0) taiHookReleaseForKernel(hooks_uid, ref_hooks);   
	return SCE_KERNEL_STOP_SUCCESS;
}