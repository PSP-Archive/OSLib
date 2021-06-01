
#include "../OSLib/oslib.h"

//PSP_MODULE_INFO("MP3decodeTest", 0, 0, 1);
//PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);
PSP_MODULE_INFO("MP3decodeTest", 0x1000, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU);

char startPath[512];

int user_main(void)
{
	int musicThread = -1, i;
	OSL_SOUND *mp3, *mp32;

	//Keep the start path from the main (kernel) thread
	sceIoChdir(startPath);

	oslInit(0);
	oslInitGfx(OSL_PF_5650, 1);
	oslInitAudio();

//	scePowerSetClockFrequency(80, 80, 40);

	mp3 = oslLoadSoundFileAT3("ms0:/MP3/test.at3", OSL_FMT_STREAM);
	mp32 = oslLoadSoundFileMP3("ms0:/MP3/test.mp3", OSL_FMT_STREAM);
	oslSetSoundLoop(mp3, 1);
	oslSetSoundLoop(mp32, 1);
	mp3->volumeLeft = mp3->volumeRight = 0xffff;

	while (!osl_quit)		{
	   oslStartDrawing();
	   for (i=0;i<90000;i++)		{
		   oslVramMgrInit();
	   }
	   oslDrawFillRect(0, 0, 10, 10, RGB(255, 0, 0));
	   oslSysBenchmarkDisplay();
	   oslEndDrawing();
	   oslEndFrame();
	   oslSyncFrame();

	   oslReadKeys();

	   if (osl_pad.pressed.cross)
			oslPlaySound(mp3, 0);
	   if (osl_pad.pressed.circle)
			oslPlaySound(mp32, 1);
	   if (osl_pad.pressed.square)
			oslStopSound(mp32);
	   if (osl_pad.pressed.triangle)
			oslStopSound(mp3);

	   if (osl_pad.pressed.L)
			oslPauseSound(mp3, -1);
	   if (osl_pad.pressed.R)
			oslPauseSound(mp32, -1);

	   if (osl_pad.pressed.start)
		   break;
   }
   
   sceKernelExitGame();
   return 0;
}

// Copying the exception handler from SDLSMS as it gives a nice output to find the bug
void sdl_psp_exception_handler(PspDebugRegBlock *regs)
{
	pspDebugScreenInit();

	pspDebugScreenSetBackColor(0x00FF0000);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	pspDebugScreenPrintf("I regret to inform you your psp has just crashed\n\nException Details:\n");
	pspDebugDumpException(regs);
	pspDebugScreenPrintf("\nThe offending routine may be identified with:\n\n"
		"\tpsp-addr2line -e target.elf -f -C 0x%x 0x%x 0x%x\n\n"
		"Press START to exit.",
		regs->epc, regs->badvaddr, regs->r[31]);

	while(1)		{
		oslReadKeys();
		if (osl_keys->pressed.start)
			oslQuit();
	}
}

int main(int argc, char *argp[])
{
	//Get path
	sceIoGetThreadCwd(sceKernelGetThreadId(), startPath, sizeof(startPath));

	oslInitAudioME(OSL_FMT_ALL);
	pspDebugInstallErrorHandler(sdl_psp_exception_handler);

    // create user thread, tweek stack size here if necessary
    SceUID thid = sceKernelCreateThread("User Mode Thread", user_main,
            0x12, // default priority
            256 * 1024, // stack size (256KB is regular default)
            PSP_THREAD_ATTR_USER | THREAD_ATTR_VFPU, NULL);

    // start user thread, then wait for it to do everything else
    sceKernelStartThread(thid, 0, 0);
    sceKernelWaitThreadEnd(thid, NULL);

    return 0;
}

