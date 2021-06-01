#include "oslib.h"
#include "ostools.h"

PSP_MODULE_INFO("OSLib user", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

int main(void)
{
	OSL_IMAGE *img;
	OSL_SOUND *snd, *mod;
	int i, quality = 2;

	//Initialization
	oslInit(0);
	oslInitGfx(OSL_PF_8888, 1);
	oslInitConsole();
	oslInitAudio();
	oslSetQuitOnLoadFailure(1);



	img = oslLoadImageFileGIF("test5.gif", OSL_IN_RAM, OSL_PF_5551);
//	img = oslConvertImageTo(img, OSL_IN_RAM, OSL_PF_8BIT);

/*	oslLockImage(img);
	{
		u32 *ptr = (u32*)img->data;
		int i;
		for (i=0;i<img->totalSize;i+=4)		{
			*ptr &= 0x00ffffff;
			*ptr++ |= 0x80000000;
		}
	}
	oslUnlockImage(img);*/

//	mod = oslLoadSoundFileBGM("music.bgm", 0);
	mod = oslLoadSoundFileMOD("test.it", 0);
//	mod = oslLoadSoundFileMOD("test.it", 0);
	oslPlaySound(mod, 0);
	oslSetSoundLoop(mod, 1);

	while(!osl_quit)		{

		if (!osl_skip)		{
			oslStartDrawing();
			oslClearScreen(RGB(255, 0, 0));
			sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
			sceGuSendCommandi(233, 0xff);

			for (i=0;i<20000;i++)
				oslGetNextPower2(100);

			oslDrawImage(img);
			oslPrintf_xy(0, 272 - osl_curFont->charHeight * 3, "Quality: %i", quality);
			oslDrawString(0, 272 - osl_curFont->charHeight * 2, "Triangle: Quitter");
			oslDrawString(0, 272 - osl_curFont->charHeight, "Rond: Screenshot");
		}

		//Process here
		oslReadKeys();
		if (osl_pad.pressed.cross)
			oslPlaySound(mod, 0);
		if (osl_pad.pressed.square)
			oslStopSound(mod);
		if (osl_pad.pressed.circle)
			oslWriteImageFilePNG(OSL_SECONDARY_BUFFER, "test2.png", 0);
		if (osl_pad.pressed.triangle)
			break;
		if (osl_pad.pressed.L || osl_pad.pressed.R)			{
			if (osl_pad.pressed.R)
				quality++;
			else
				quality--;
			if (quality < 0)		quality += 3;
			if (quality >= 3)		quality -= 3;
			if (quality == 2)
				oslSetModSampleRate(44100, 1, 0);
			else if (quality == 1)
				oslSetModSampleRate(22050, 1, 1);
			else
				oslSetModSampleRate(11025, 1, 2);
			oslPlaySound(mod, 0);
		}
		if (osl_pad.pressed.right)
			oslMoveImageTo(img, OSL_IN_RAM);
		if (osl_pad.pressed.left)
			oslMoveImageTo(img, OSL_IN_VRAM);

		oslSysBenchmarkDisplay();
		oslEndDrawing();
		oslSyncFrame();
	}

	oslEndGfx();
	oslQuit();
	return 0;
}


#ifdef KERNEL_MODE

// Copying the exception handler from SDLSMS as it gives a nice output to find the bug
void sdl_psp_exception_handler(PspDebugRegBlock *regs)
{
	pspDebugScreenInit();

	pspDebugScreenSetBackColor(0x00FF0000);
	oslSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	oslPrintf("I regret to inform you your psp has just crashed\n\n");
	oslPrintf("Exception Details:\n");
	pspDebugDumpException(regs);
	oslPrintf("\nThe offending routine may be identified with:\n\n"
		"\tpsp-addr2line -e target.elf -f -C 0x%x 0x%x 0x%x\n",
		regs->epc, regs->badvaddr, regs->r[31]);
}
#endif

#ifdef KERNEL_MODE
int main(int argc, char *argp[])
{
    // Kernel mode thread

/*    if (adhocLoadDrivers(&module_info) != 0)
    {
        printf("Driver load error\n");
        return 0;
    }*/

	// Need to copy this across otherwise the address is in kernel mode
/*	strcpy (RomPath, (char*)argp[0]);
//	__psp_argv_0 = RomPath;*/

	pspDebugInstallErrorHandler(sdl_psp_exception_handler);

    // create user thread, tweek stack size here if necessary
    SceUID thid = sceKernelCreateThread("User Mode Thread", user_main,
            0x13, // default priority is 0x11
            256 * 1024, // stack size (256KB is regular default)
            PSP_THREAD_ATTR_USER, NULL);

    // start user thread, then wait for it to do everything else
    sceKernelStartThread(thid, 0, 0);
    sceKernelWaitThreadEnd(thid, NULL);

    return 0;
}
#endif

