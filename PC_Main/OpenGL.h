#ifdef __cplusplus
	extern "C"		{
#endif
extern void emuInit();
extern void emuInitGL();
extern void emuInitGfx();
extern void emuStartDrawing();
extern void emuEndDrawing();
extern HWND emuHWnd;
extern BOOL emuKeyHeld[256];
#ifdef __cplusplus
	}
#endif
