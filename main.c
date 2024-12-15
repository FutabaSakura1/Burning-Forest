#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <stdlib.h>
#include <string.h>
#include <pspctrl.h>
#include <unistd.h>
#include <stdbool.h>


// PSP_MODULE_INFO is required
PSP_MODULE_INFO("Burning-Forest", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int cookies = 0;
int grandmas = 0;
int factories = 0;
int grandmaCost = 10;
int factoryCost = 25;
char selectedUpgrade[20] = "Grandma";
int upgradeNumber = 0;
int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int idle_cookies_thread(SceSize args, void *argp) {
    while (1) {
        if (factories > 0) {
            int facCookiesPerSec = factories * 2;
            cookies += facCookiesPerSec;
        }if (grandmas > 0) {
            int gramCookiesPerSec = grandmas;
            cookies += gramCookiesPerSec;
        }
        sceKernelDelayThread(1000000);  // Sleep for 1 second (in microseconds)
    }
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}
int setup_idle_cookies_thread(void) {
    int thid = sceKernelCreateThread("IdleCookies", idle_cookies_thread, 0x18, 0x1000, 0, NULL);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, NULL);
    } 

    return thid;
}

void updateSelection() {
    switch (upgradeNumber) {
        case 0:
            strcpy(selectedUpgrade, "Grandma");
            break;
        case 1:
            strcpy(selectedUpgrade, "Factory");
            break;
    }
}
void drawScreen() {
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("Cookies: %d", cookies);
    pspDebugScreenSetXY(0, 1);
    pspDebugScreenPrintf("Selected: %s", selectedUpgrade);
    pspDebugScreenSetXY(0,2);
    pspDebugScreenPrintf("---------------------------------------");
    pspDebugScreenSetXY(0,3);
    pspDebugScreenPrintf("Grandmas: %d", grandmas);
    pspDebugScreenSetXY(0,4);
    pspDebugScreenPrintf("Cost per Grandma: %d", grandmaCost);
    pspDebugScreenSetXY(0,5);
    pspDebugScreenPrintf("Factories: %d", factories);
    pspDebugScreenSetXY(0,6);
    pspDebugScreenPrintf("Cost per Factory: %d", factoryCost);
    
}

int main(void)  {
    SceCtrlData pad;


    int pressedX = 0;
    int pressedO = 0;
    int pressedL = 0;
    int pressedR = 0;
    // Use above functions to make exiting possible
    setup_callbacks();
    setup_idle_cookies_thread();
    pspDebugScreenInit();
    pspDebugScreenClear();
    
    while(1) {
        sceCtrlReadBufferPositive(&pad,1);
        pspDebugScreenClear();
        drawScreen();
        sceDisplayWaitVblankStart();
        
        sceDisplaySetMode(0, 480, 272); // Set resolution to 480x272 (default PSP resolution)
        sceDisplaySetFrameBuf((void *)0x44000000, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
        if(pad.Buttons != 0)
        {
            if (pad.Buttons & PSP_CTRL_CROSS && pressedX == 0)
            {
                cookies = cookies + 1;
                pressedX = 1;
            }
            if (pad.Buttons & PSP_CTRL_CIRCLE && pressedO == 0)
            {
                switch (upgradeNumber){
                    case 0:
                        if (cookies >= grandmaCost){
                            cookies = cookies - grandmaCost;
                            grandmaCost = grandmaCost + 2;
                            grandmas = grandmas + 1;
                        }
                        break;
                    case 1:
                        if (cookies >= factoryCost){
                            cookies = cookies - factoryCost;
                            factoryCost = factoryCost + 5;
                            factories = factories + 1;
                            pspDebugScreenClear();
                        }
                        break;
                }
                
                pressedO = 1;
            }
            if (pad.Buttons & PSP_CTRL_LTRIGGER && pressedL == 0){
                pressedL = 1;
                if(upgradeNumber > 0){
                    upgradeNumber -= 1;
                    updateSelection();
                }
            }
            if (pad.Buttons & PSP_CTRL_RTRIGGER && pressedR == 0){
                pressedR = 1;
                if(upgradeNumber < 1){
                    upgradeNumber += 1;
                    updateSelection();
                }
            }
        }else {
            pressedX = 0;
            pressedO = 0;
            pressedL = 0;
            pressedR = 0;
        }

    }

    return 0;
}



