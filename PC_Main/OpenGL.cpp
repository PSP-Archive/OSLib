#include "stdAfx.h"
#include <gl/glu.h>

HDC hdc;
HPALETTE hOldPalette;
RECT oldRect;
int width, height;
HGLRC       hrc;

//Interne
static const unsigned char threeto8[8] =
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

//Interne
static const unsigned char twoto8[4] =
{
	0, 0x55, 0xaa, 0xff
};

//Interne
static const unsigned char oneto8[2] =
{
	0, 255
};

//Interne
static const int defaultOverride[13] =
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

//Interne
unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift)
{
	unsigned char val;

	val = (unsigned char) (i >> shift);
	switch (nbits)
	{

	case 1:
		val &= 0x1;
		return oneto8[val];
	case 2:
		val &= 0x3;
		return twoto8[val];
	case 3:
		val &= 0x7;
		return threeto8[val];

	default:
		return 0;
	}
}

//Interne
static const PALETTEENTRY defaultPalEntry[20] =
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};

void CreateRGBPalette()
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal;
	int n, i;
	HPALETTE pal;

	n = GetPixelFormat(hdc);
	DescribePixelFormat(hdc, n, sizeof(pfd), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		n = 1 << pfd.cColorBits;
		pPal = (PLOGPALETTE) malloc(sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY));
		if (!pPal)
			return;

		pPal->palVersion = 0x300;
		pPal->palNumEntries = n;
		for (i=0; i<n; i++)
		{
			pPal->palPalEntry[i].peRed =
					ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
			pPal->palPalEntry[i].peGreen =
					ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
			pPal->palPalEntry[i].peBlue =
					ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
			pPal->palPalEntry[i].peFlags = 0;
		}

		/* fix up the palette to include the default GDI palette */
		if ((pfd.cColorBits == 8)                           &&
			(pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
			(pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
			(pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
		   )
		{
			for (i = 1 ; i <= 12 ; i++)
				pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
		}

		pal = CreatePalette(pPal);
		free(pPal);

		hOldPalette = SelectPalette(hdc, pal, FALSE);
		RealizePalette(hdc);
	}
}

//Interne
BOOL SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		  PFD_SUPPORT_OPENGL |          // support OpenGL
		  PFD_DOUBLEBUFFER,             // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		32,                             // 32-bit color depth
		8, 0, 8, 8, 8, 16,              // color bits
		8,                              // no alpha buffer
		24,                             // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int pixelformat;

	if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0 )
	{
		MessageBox(NULL, "ChoosePixelFormat failed", "", 0);
		return FALSE;
	}

	if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE)
	{
		MessageBox(NULL, "SetPixelFormat failed", "", 0);
		return FALSE;
	}

	return TRUE;
}

void emuInitGfx()			{
	PIXELFORMATDESCRIPTOR pfd;
	int         n;					//Index du pixelFormat

	//On va dessiner sur la fenêtre
	hdc = GetDC(emuHWnd);

	//Initialise le pixelFormat
	if (!SetupPixelFormat())
		return;

	n = GetPixelFormat(hdc);
	DescribePixelFormat(hdc, n, sizeof(pfd), &pfd);

	CreateRGBPalette();

	hrc = wglCreateContext(hdc);

	wglMakeCurrent(hdc, hrc);
	//Met en place la matrice orthogonale
		glViewport(0, 0, 480, 272);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 480, 272, 0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.1f, 0.1f, 0.0f);
//		glTranslatef(0.375, 0.375, 0.0);
//		glTranslatef(0.0, 0.0, 0.0);

	//NE PAS FAIRE TOUJOURS
	// make the shading smooth
	glShadeModel(GL_SMOOTH); 
	//Alpha standard (marche bien)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Teinte (teinte si alpha=255, opaque si alpha=0)
//	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_COLOR);
	glEnable(GL_BLEND);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
//	glEnable(GL_COLOR_MATERIAL);

}

void emuStartDrawing()		{
	//Ok
//	sceGuAmbientColor(0xffffffff);
}


void emuEndDrawing()		{
	glFinish();
//	wglMakeCurrent (NULL, NULL);
}

