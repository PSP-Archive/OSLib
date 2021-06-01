#include "oslib.h"
#include "ostools.h"

//#define KERNEL_MODE

#ifdef KERNEL_MODE
	PSP_MODULE_INFO("OSLib kernel", 0x1000, 0, 1); /* 0x1000 REQUIRED!!! */
	PSP_MAIN_THREAD_ATTR(0);
#else
	PSP_MODULE_INFO("OSLib user", 0, 0, 1);
	PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);
#endif

//Map.bmp
//Converti avec GBA Graphics par Brunni
//Map ("","Map.til.c",0,ffffffff)
//Taille: 16*16 * 32*12
//Mémoire: 768 octets

const unsigned short mountains_map[12][32]=	{
	{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0001|1024|2048, 0x0002, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0002, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0003,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0002, 0x0000, 0x0000},
	{0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0004, 0x0005, 0x0006,
0x0007, 0x0008, 0x0003, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{0x0000, 0x0000, 0x0000, 0x0004, 0x0005, 0x0009, 0x000a, 0x000b,
0x000c, 0x000d, 0x0006, 0x0007, 0x0008, 0x0000, 0x0000, 0x0000,
0x0000, 0x000e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{0x0000, 0x0004, 0x0005, 0x0009, 0x000b, 0x000f, 0x000a, 0x000a,
0x000a, 0x000a, 0x000b, 0x000a, 0x000d, 0x0007, 0x0008, 0x0010,
0x0011, 0x0012, 0x0013, 0x0014, 0x0000, 0x0000, 0x0000, 0x0000,
0x0003, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
	{0x0005, 0x0009, 0x000c, 0x000b, 0x0015, 0x0012, 0x0016, 0x000b,
0x000a, 0x000a, 0x000a, 0x000c, 0x000b, 0x0017, 0x0018, 0x0019,
0x001a, 0x001b, 0x001c, 0x001d, 0x0013, 0x0014, 0x0004, 0x0005,
0x0006, 0x0007, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0004},
	{0x000b, 0x000b, 0x000a, 0x0015, 0x001e, 0x001b, 0x001f, 0x0016,
0x000a, 0x000c, 0x000a, 0x0017, 0x0018, 0x0019, 0x001a, 0x001a,
0x001b, 0x001a, 0x001a, 0x001a, 0x0020, 0x0021, 0x0009, 0x000b,
0x000a, 0x000a, 0x000d, 0x0007, 0x0008, 0x0004, 0x0005, 0x0009},
	{0x000a, 0x000b, 0x0015, 0x001e, 0x001b, 0x001a, 0x001b, 0x001f,
0x0016, 0x0017, 0x0018, 0x0019, 0x001b, 0x001a, 0x001a, 0x001a,
0x001a, 0x001a, 0x0020, 0x0021, 0x0009, 0x000a, 0x000a, 0x000a,
0x000a, 0x000a, 0x000c, 0x000b, 0x000d, 0x0022, 0x0023, 0x000b},
	{0x0023, 0x0015, 0x001e, 0x001a, 0x001a, 0x001a, 0x001a, 0x001c,
0x001f, 0x0024, 0x001c, 0x001a, 0x001a, 0x001a, 0x001a, 0x001b,
0x0020, 0x0021, 0x0009, 0x000b, 0x000a, 0x000a, 0x000a, 0x000a,
0x000a, 0x000c, 0x000a, 0x000a, 0x000a, 0x000a, 0x000d, 0x0022},
	{0x0015, 0x001e, 0x001c, 0x001b, 0x001a, 0x001b, 0x001a, 0x001a,
0x001a, 0x001f, 0x0024, 0x001b, 0x001b, 0x001a, 0x0020, 0x0021,
0x0009, 0x000a, 0x000b, 0x000a, 0x000a, 0x000a, 0x000b, 0x000c,
0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x000a, 0x000b}
};

void displayCPU()		{
	int ms4;
	ms4 = oslMeanBenchmarkTestEx(OSL_BENCH_GET, 4);
	ms4 *= 6;
	oslSetTextColor(RGB(255,255,255));
	oslSetBkColor(RGBA(255,255,255,0));
	oslPrintf_xy(0,0, "CPU: %i.%i%% (100 MHz)", ms4 / 1000, (ms4 % 1000) / 100);
}

#ifdef KERNEL_MODE
int user_main(void)
#else
int main(void)
#endif
{
	OSL_IMAGE *imgMountains;
	OSL_MAP *mapMountains;
	OSL_FONT *fontGlow;
	OSL_IMAGE *image;
	OSL_SOUND *zik, *saut, *cle;
	OSL_ALPHA_PARAMS alpha;
	bool pause = 0, resol = 0;
	int i;

	oslInit(0);
	oslInitGfx(OSL_PF_8888, 1);
	oslInitAudio();
	oslSetQuitOnLoadFailure(1);
//	oslSetFramerate(30);

	//Should be enough
	scePowerSetClockFrequency(100, 100, 50);
	//We need to reduce the default number of samples per read else VBLANK can be missed.
	oslAudioSetDefaultSampleNumber(128);

	zik = oslLoadSoundFileMOD("music.mod", 0);
	saut = oslLoadSoundFileWAV("jump.wav", OSL_FMT_STREAM);
	cle = oslLoadSoundFileWAV("key.wav", OSL_FMT_STREAM);
	oslSetModSampleRate(22050, 1, 1);
	oslSetSoundLoop(zik, 1);
	oslPlaySound(zik, 0);

	oslSetTransparentColor(RGB(255, 0, 254));
	imgMountains = oslLoadImageFilePNG("mountains.png", OSL_IN_VRAM | OSL_SWIZZLED, OSL_PF_4BIT);
	oslDisableTransparentColor();

	//Now we can create a map with that tileset
	mapMountains = oslCreateMap(
		imgMountains,						//Tileset
		mountains_map,						//Map data
		16,16,								//Tiles size
		32,12,								//Map size
		OSL_MF_U16_GBA);					//Map format

	fontGlow = oslLoadFontFile("glow.oft");
	oslSetFont(fontGlow);

	image = oslLoadImageFileJPG("image.jpg", OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_5650);
	oslImageSetRotCenter(image);
	image->x = OSL_SCREEN_WIDTH / 2;
	image->y = OSL_SCREEN_HEIGHT / 2;

	while(!osl_quit)		{
		if (!osl_skip)		{
			oslStartDrawing();
			oslClearScreen(RGB(0, 0, 128));
			oslDrawMap(mapMountains);
			oslSetBilinearFilter(1);
			oslDrawImage(image);
			oslSetBilinearFilter(0);
			oslSetTextColor(RGB(255, 192, 0));
			oslSetBkColor(RGBA(0, 255, 255, 128));
			oslPrintf_xy(0, 100, "Hello world!");

			//Save alpha to restore it later
			oslGetAlphaEx(&alpha);
			{
				oslSetAlpha(OSL_FX_SUB, 0xff);
				oslDrawFillRect(0, 0, 100, 100, RGB(128, 0, 0));
			}
			oslSetAlphaEx(&alpha);

			displayCPU();
			oslEndDrawing();
		}

		oslReadKeys();
		if (osl_pad.pressed.triangle)
			break;
		if (osl_pad.pressed.circle)
			oslPlaySound(cle, 1);
		if (osl_pad.pressed.square)
			oslPlaySound(saut, 0);
		if (osl_pad.pressed.L)		{
			resol = !resol;
			if (resol)
				oslInitGfx(OSL_PF_5650, 1);
			else
				oslInitGfx(OSL_PF_8888, 1);
		}
		if (osl_pad.pressed.R)
			oslPlaySound(zik, 0);
		if (osl_pad.pressed.cross)
			pause = !pause;

		if (!pause)		{
			mapMountains->scrollX++;
			image->angle++;
		}

		oslSyncFrameEx(1, 4, 0);
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
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenClear();

	pspDebugScreenPrintf("I regret to inform you your psp has just crashed\n\n");
	pspDebugScreenPrintf("Exception Details:\n");
	pspDebugDumpException(regs);
	pspDebugScreenPrintf("\nThe offending routine may be identified with:\n\n"
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
            PSP_THREAD_ATTR_USER | THREAD_ATTR_VFPU, NULL);

    // start user thread, then wait for it to do everything else
    sceKernelStartThread(thid, 0, 0);
    sceKernelWaitThreadEnd(thid, NULL);

    return 0;
}
#endif

