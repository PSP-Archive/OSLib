#include "oslib.h"
//#include "ostools.h"

PSP_MODULE_INFO("OSLib user", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);


/*OSL_IMAGE *loadBackgroundImage(char *filename)		{
	OSL_IMAGE *img = NULL, *temp;
	temp = oslLoadImageFile(filename, OSL_IN_RAM, OSL_PF_8888);
	if (temp)			{
		img = oslCreateImage(temp->sizeX, temp->sizeY, OSL_IN_VRAM, OSL_PF_8888);
		if (img)		{
			oslStartDrawing();
			oslClearImage(img, RGBA(255, 0, 0, 255));
			oslSetDrawBuffer(img);
			oslSetDithering(1);
			oslSetBilinearFilter(0);
			oslDrawImage(temp);
			oslSetDrawBuffer(OSL_DEFAULT_BUFFER);
			oslEndDrawing();
			oslSwizzleImage(img);
		}
		oslDeleteImage(temp);
	}
	return img;
}*/

//Creates a 64x64 image from an file
/*OSL_IMAGE *loadThumbnail(const char *filename)      {
   OSL_IMAGE *src, *dst, *oldDrawBuf = oslGetDrawBuffer();
   int oldBilinear = osl_bilinearFilterEnabled, oldDither = osl_ditheringEnabled;

   src = oslLoadImageFile(filename, OSL_IN_RAM, OSL_PF_8888);
   dst = oslCreateImage(src->sizeX, src->sizeY, OSL_IN_VRAM, OSL_PF_5650);
   if (src && dst)         {
      oslSetDrawBuffer(dst);
      oslSetBilinearFilter(1);
      oslSetDithering(1);
//      src->x = src->y = 0;
//      src->stretchX = 64;
//      src->stretchY = 64;
      oslDrawImage(src);
      oslSetDrawBuffer(oldDrawBuf);
      oslSetBilinearFilter(oldBilinear);
      oslSetDithering(oldDither);
   }
   if (src)
      oslDeleteImage(src);

   return dst;
}*/

OSL_IMAGE *loadScaledImage(char *filename, int location, int pixelFormat)		{
	OSL_IMAGE *img = NULL, *temp, *oldDrawBuf = oslGetDrawBuffer();
	int oldBilin = osl_bilinearFilterEnabled, oldDither = osl_ditheringEnabled;
	//Load our image with the maximum precision possible; it's just temporary
	temp = oslLoadImageFile(filename, OSL_IN_RAM, OSL_PF_8888);
	if (temp)			{
		//Create an image in the right pixel format. Location HAS to be VRAM because we'll draw on it.
		img = oslCreateImage(temp->sizeX, temp->sizeY, OSL_IN_VRAM, pixelFormat);
		if (img)		{
			oslStartDrawing();
			//First clear the image to opaque black
			oslClearImage(img, RGBA(0, 0, 0, 255));
			//We'll draw on that image.
			oslSetDrawBuffer(img);
			//Enable dithering, useful if the pixelFormat is not 8888, because dithering will
			//remove the blocky effect of gradients for example.
			oslSetDithering(1);
			//Use bilinear filter for rescaling
			oslSetBilinearFilter(1);
			//Here, you could add some computation to keep the aspect ratio
			temp->stretchX = 480;
			temp->stretchX = 272;
			//Draw the image, scaled down to the PSP screen size
			oslDrawImageBig(temp);
			//Restore everything back to original values
			oslSetDrawBuffer(oldDrawBuf);
			oslSetBilinearFilter(oldBilin);
			oslSetDithering(oldDither);
			oslEndDrawing();
			//Copy the image to the good place
			oslMoveImageTo(img, location);
			//Finally, swizzle the image for more speed!
			oslSwizzleImage(img);
		}
		oslDeleteImage(temp);
	}
	return img;
}

int main(void)
{
	OSL_IMAGE *image;
	int i, pause = 1;
	int x0 = 0, y0 = 0;

	oslInit(0);
	oslInitGfx(OSL_PF_8888, 1);
	oslSetQuitOnLoadFailure(1);
//	oslSetFramerate(15);

//	image = oslLoadImageFilePNG("res/back.png", OSL_IN_RAM, OSL_PF_8888);
//	image = oslLoadImageFilePNG("mountains.png", OSL_IN_VRAM | OSL_SWIZZLED, OSL_PF_5650);
//	image = loadBackgroundImage("res/back.png");
	image = oslLoadImageFilePNG("res/back.png", OSL_IN_RAM | OSL_SWIZZLED, OSL_PF_8888);
//	oslSetImageTile(image, 0, 0, 8, 8);
//	oslMirrorImageH(image);
	image->x = OSL_SCREEN_WIDTH / 2;
	image->y = OSL_SCREEN_HEIGHT / 2;
	image->centerX = OSL_SCREEN_WIDTH / 2;
	image->centerY = OSL_SCREEN_HEIGHT / 2;

	while(!osl_quit)		{

		if (!osl_skip)		{
			oslStartDrawing();
			oslClearScreen(RGB(0, 0, 128));
			oslSetBilinearFilter((image->angle % 90) != 0);
//			oslDrawImageBig(image);
//			for (i=0;i<1000;i++)
			oslSetImageTileSize(image, x0, y0, 480, 272);
//			image->stretchX = image->sizeX / 2;
//			image->stretchY = image->sizeY / 2;
			image->stretchX *= 2;
			image->stretchY *= 2;
			oslDrawImageBig(image);
			oslSysBenchmarkDisplay();
			oslEndDrawing();
		}

		if (!pause)
			image->angle++;
		oslReadKeys();
		if (osl_pad.pressed.start)
			oslResetImageProperties(image);

		if (osl_pad.analogX <= -64)
			osl_pad.held.left = 1;
		if (osl_pad.analogX >= 64)
			osl_pad.held.right = 1;
		if (osl_pad.analogY <= -64)
			osl_pad.held.up = 1;
		if (osl_pad.analogY >= 64)
			osl_pad.held.down = 1;

		if (osl_pad.held.left)
			x0--;
		if (osl_pad.held.up)
			y0--;
		if (osl_pad.held.right)
			x0++;
		if (osl_pad.held.down)
			y0++;
		if (osl_pad.pressed.cross)
			pause = !pause;
		if (osl_pad.pressed.triangle)
			break;

		oslEndFrame();
		oslSyncFrameEx(1, 4, 0);
	}

	oslEndGfx();
	oslQuit();
	return 0;
}

/*

	A FAIRE:
	Clipping (x < 0, etc.)
	Pourquoi ça merde sur PSP??

*/

