//todo: boot gba cart
/*
void clearmem(u32 dest, u32 size) {
	int i=0;
	swiFastCopy(&i, (u32*)dest, 0x01000000 | (size>>2));
}

void boot_gba(void)
{
	if (Wifi_CheckInit()==0){
		PA_InitWifi();
	}
	PA_SetLedBlink(0,0);

   //switch off one of the screens
	if(topScreenGBA)
	{
		PA_SetScreenLight(1, 0); 
		PA_SetScreenLight(0, 1); 
	}
	else
	{
		PA_SetScreenLight(0, 0);
		PA_SetScreenLight(1, 1);
	}
 	//reset DMA
	clearmem(0x40000B0, 0x30);

	//clear VRAM
	REG_POWERCNT=1;
	VRAM_CR=0x80808080;
	VRAM_E_CR=0x80;
	VRAM_F_CR=0x80;
	VRAM_G_CR=0x80;
	VRAM_H_CR=0x80;
	VRAM_I_CR=0x80;
	if (bordersGBA)
		load_gba_frame(topScreenGBA);
	else
		blank_gba_frame(topScreenGBA);

	VRAM_CR=0;
	VRAM_E_CR=0;
	VRAM_F_CR=0;
	VRAM_G_CR=0;
	VRAM_H_CR=0;
	VRAM_I_CR=0;

	if(topScreenGBA){
			REG_POWERCNT=1;
	}
	else{
		REG_POWERCNT=POWER_SWAP_LCDS|1;
	}
	while(true){
		REG_IPC_FIFO_TX=0x87654322; // send a fifo to the highjacked wifi code
		waitForVBL();
	}
}
*/