#include "oslib.h"
#include "ostools.h"

PSP_MODULE_INFO("OSLib user", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

//Map.bmp
//Converti avec GBA Graphics par Brunni
//Map ("","Map.til.c",0,ffffffff)
//Taille: 16*16 * 32*12
//Mémoire: 768 octets
extern const unsigned short mountains_map[12][32];

void displayCPU()		{
	int ms4;
	ms4 = oslMeanBenchmarkTestEx(OSL_BENCH_GET, 4);
	ms4 *= 6;
	oslSetTextColor(RGB(255,255,255));
	oslSetBkColor(RGBA(255,255,255,0));
	oslPrintf_xy(0,0, "CPU: %i.%i%% (100 MHz)", ms4 / 1000, (ms4 % 1000) / 100);
}

int main(void)
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

