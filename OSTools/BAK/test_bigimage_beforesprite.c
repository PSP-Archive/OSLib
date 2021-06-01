#include "oslib.h"
//#include "ostools.h"

PSP_MODULE_INFO("OSLib user", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER|THREAD_ATTR_VFPU);

#define TEXSIZEX_LIMIT 512
#define TEXSIZEX_LIMITF 512.0f
#define TEXSIZEY_LIMIT 512
#define TEXSIZEY_LIMITF 512.0f

static void oslSetTexturePart(OSL_IMAGE *img, int x, int y)		{
	u8 *data = (u8*)img->data;

	oslEnableTexturing();
	if (img->palette && osl_curPalette != img->palette)		{
		osl_curPalette = img->palette;
		//Change la palette
		sceGuClutMode(img->palette->pixelFormat,0,0xff,0);
		//Uploade la palette
		sceGuClutLoad((img->palette->nElements>>3), img->palette->data);
	}

	data = (u8*)oslGetImagePixelAdr(img, x, y);
	if (osl_curTexture != data)		{
		osl_curTexture = data;
		//Change la texture
		sceGuTexMode(img->pixelFormat, 0, 0, oslImageIsSwizzled(img));
//		sceGuTexFunc(GU_TFX_REPLACE, img->pixelFormat==OSL_PF_5650?GU_TCC_RGB:GU_TCC_RGBA);
		sceGuTexImage(0, TEXSIZEX_LIMIT, TEXSIZEY_LIMIT, img->sysSizeX, data);
	}
}

void oslDrawImageBig(OSL_IMAGE *img)		{
	OSL_PRECISE_VERTEX *vertices;
//		int rsX, rsY;
	float cX, cY, tmpY;
	float uVal, xVal;
	const float uCoeff = 64.0f, vCoeff = TEXSIZEY_LIMITF;
	float xCoeff, yCoeff;
	float angleRadians;
	float offsetX0 = img->offsetX0, offsetX1 = img->offsetX1, offsetY0 = img->offsetY0, offsetY1 = img->offsetY1;
	float uLimit, vLimit, vVal;
	int uStart, vStart, swizzleScaleFactor;
	bool flipH = 0, flipV = 0;

	//Handle mirroring
	if (offsetX0 > offsetX1)		{
		float tmp = offsetX0;
		offsetX0 = offsetX1;
		offsetX1 = tmp;
		flipH = 1;
	}
	if (offsetY0 > offsetY1)		{
		float tmp = offsetY0;
		offsetY0 = offsetY1;
		offsetY1 = tmp;
		flipV = 1;
	}

	//To adjust the texture offset when swizzling is enabled (with a normal factor, the image appears trashed)
#ifdef PSP
	swizzleScaleFactor = oslImageIsSwizzled(img) ? 8 : 1;
#else
	swizzleScaleFactor = 1;
#endif

	xVal = 0.f;
	angleRadians = (img->angle * 3.141592653f) / 180.f;

	//X scaling coefficient (pixels per stripe)
	xCoeff = uCoeff / ( (float)/*oslAbs*/(offsetX1 - offsetX0) / (float)img->stretchX );
	//Y scaling coefficient (pixels per block)
	yCoeff = vCoeff / ( (float)/*oslAbs*/(offsetX1 - offsetX0) / (float)img->stretchX );

	cX = (-img->centerX * img->stretchX)/(int)(offsetX1 - offsetX0);
	cY = (-img->centerY * img->stretchY)/(int)(offsetY1 - offsetY0);

	vStart = 0;
	vVal = offsetY0;
	if (offsetY1 >= TEXSIZEY_LIMITF)
		vLimit = vStart + TEXSIZEY_LIMITF;
	else
		vLimit = offsetY1;

	//Ici yCoeff est toujours positif!
	if (flipV)		{
//		cY += (yCoeff * img->stretchY) / vCoeff;
		cY += img->stretchY;
		tmpY = cY - ((vLimit - vStart) * img->stretchY) / (int)(offsetY1 - offsetY0);
	}
	else
		tmpY = cY + ((vLimit - vStart) * img->stretchY) / (int)(offsetY1 - offsetY0);

	if (flipH)
		xCoeff = -xCoeff;
	if (flipV)
		yCoeff = -yCoeff;

	do		{
		
		//For each line...
		uVal = offsetX0;

		xVal = cX;

		//Ici xCoeff est toujours négatif
		if (flipH)
			xVal += img->stretchX;
//			xVal -= (xCoeff * img->stretchX) / uCoeff;

		//We should not get further than the texture width limit
		uStart = ((int)offsetX0 / TEXSIZEX_LIMIT) * TEXSIZEX_LIMIT;
		if (offsetX1 - uStart >= TEXSIZEX_LIMITF)
			uLimit = uStart + TEXSIZEX_LIMITF;
		else
			uLimit = offsetX1;

		do		{
			oslSetTexturePart(img, uStart * swizzleScaleFactor, vStart);

			while (uVal < uLimit)		{
				vertices = (OSL_PRECISE_VERTEX*)sceGuGetMemory(4 * sizeof(OSL_PRECISE_VERTEX));

				vertices[0].u = uVal - uStart;
				vertices[0].v = vVal;
				vertices[0].x = vfpu_cosf(angleRadians, xVal) - vfpu_sinf(angleRadians, cY) + img->x;
				vertices[0].y = vfpu_sinf(angleRadians, xVal) + vfpu_cosf(angleRadians, cY) + img->y;
				vertices[0].z = 0;

				vertices[2].u = uVal - uStart;
				vertices[2].v = vLimit - vStart;
				vertices[2].x = vfpu_cosf(angleRadians, xVal) - vfpu_sinf(angleRadians, tmpY) + img->x;
				vertices[2].y = vfpu_sinf(angleRadians, xVal) + vfpu_cosf(angleRadians, tmpY) + img->y;
				vertices[2].z = 0;

				uVal += uCoeff;
				xVal += xCoeff;
				if (uVal >= offsetX1)		 {
					if (flipH)
						xVal = 0;
					else
						xVal = cX + img->stretchX;
					uVal = offsetX1;
				}

				vertices[1].u = uVal - uStart;
				vertices[1].v = vVal;
				vertices[1].x = vfpu_cosf(angleRadians, xVal) - vfpu_sinf(angleRadians, cY) + img->x;
				vertices[1].y = vfpu_sinf(angleRadians, xVal) + vfpu_cosf(angleRadians, cY) + img->y;
				vertices[1].z = 0;

				vertices[3].u = uVal - uStart;
				vertices[3].v = vLimit - vStart;
				vertices[3].x = vfpu_cosf(angleRadians, xVal) - vfpu_sinf(angleRadians, tmpY) + img->x;
				vertices[3].y = vfpu_sinf(angleRadians, xVal) + vfpu_cosf(angleRadians, tmpY) + img->y;
				vertices[3].z = 0;

				sceGuDrawArray(GU_TRIANGLE_STRIP,GU_TEXTURE_32BITF|GU_VERTEX_32BITF|GU_TRANSFORM_2D,4,0,vertices);
			}

			//Is there any pixel left to draw?
			if (uLimit < offsetX1)		{
				uLimit += TEXSIZEX_LIMITF;
				uStart += TEXSIZEX_LIMIT;
			}
			else
				break;

			if (uLimit >= offsetX1)
				uLimit = offsetX1;
		
		} while (1);

		float oldVl = vLimit;

		//Next blocks all begin from zero
		vVal = 0;

		//Is there any lines left?
		if (vLimit < offsetY1)		{
			vLimit += TEXSIZEY_LIMITF;
			vStart += TEXSIZEY_LIMIT;
		}
		else
			break;
		
		//We are too far?
		if (vLimit >= offsetY1)
			vLimit = offsetY1;

		//Advance our coefficients
		cY += yCoeff;
		tmpY += (vLimit - oldVl) * yCoeff / TEXSIZEY_LIMITF;

	} while (1);
}

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


int main(void)
{
	OSL_IMAGE *image;
	int i, pause = 1;

	oslInit(0);
	oslInitGfx(OSL_PF_8888, 1);
	oslSetQuitOnLoadFailure(1);
//	oslSetFramerate(15);

//	image = oslLoadImageFilePNG("res/back.png", OSL_IN_RAM, OSL_PF_8888);
//	image = oslLoadImageFilePNG("mountains.png", OSL_IN_VRAM | OSL_SWIZZLED, OSL_PF_5650);
//	image = loadBackgroundImage("res/back.png");
	image = oslLoadImageFilePNG("res/back.png", OSL_IN_RAM, OSL_PF_8888);
//	oslSetImageTile(image, 0, 0, 8, 8);
//	oslMirrorImageH(image);

	while(!osl_quit)		{

		if (!osl_skip)		{
			oslStartDrawing();
			oslClearScreen(RGB(0, 0, 128));
			oslSetBilinearFilter(1);
//			oslDrawImageBig(image);
//			for (i=0;i<1000;i++)
//			oslSetImageTile(image, 0, 0, image->sizeX, image->sizeY);
			image->stretchX = image->sizeX / 2;
			image->stretchY = image->sizeY / 2;
			oslDrawImageBig(image);
			oslSysBenchmarkDisplay();
			oslEndDrawing();
		}

		if (!pause)
			image->angle++;
		oslReadKeys();
		if (osl_pad.held.left)
			i--;
		else if (osl_pad.held.right)
			i++;
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

