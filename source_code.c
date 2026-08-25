#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void* xfb = NULL;
static GXRModeObj* rmode = NULL;

#define MAIN_MENU 0
#define ACCEL_TEST 1
#define BUTTON_TEST 2
#define MULTIPLAYER_STATUS 3
#define RUMBLE_TEST 4
#define NUNCHUCK_TEST 5

void clear_screen() {
    printf("\x1b[2J");
}

int main(int argc, char** argv) {

    // 1. Hardware Video Initialization
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // 2. Wireless Subsystem Initialization (All 4 Channels)
    WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetDataFormat(WPAD_CHAN_1, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetDataFormat(WPAD_CHAN_2, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetDataFormat(WPAD_CHAN_3, WPAD_FMT_BTNS_ACC_IR);

    int current_scenario = MAIN_MENU;
    int selected_option = 0;
    int redraw = 1;

    clear_screen();

    while (1) {

        WPAD_ScanPads();
        u32 pressed = WPAD_ButtonsDown(WPAD_CHAN_0);

        // ====================================================================
        // MAIN MENU NAVIGATION
        // ====================================================================
        if (current_scenario == MAIN_MENU) {

            if (pressed & WPAD_BUTTON_DOWN) {
                selected_option++;
                if (selected_option > 5) selected_option = 0;
                redraw = 1;
            }
            if (pressed & WPAD_BUTTON_UP) {
                selected_option--;
                if (selected_option < 0) selected_option = 5;
                redraw = 1;
            }

            if (pressed & WPAD_BUTTON_A) {
                if (selected_option == 0) {
                    current_scenario = ACCEL_TEST;
                    clear_screen();
                    redraw = 1;
                }
                else if (selected_option == 1) {
                    current_scenario = BUTTON_TEST;
                    clear_screen();
                    redraw = 1;
                }
                else if (selected_option == 2) {
                    current_scenario = MULTIPLAYER_STATUS;
                    clear_screen();
                    redraw = 1;
                }
                else if (selected_option == 3) {
                    current_scenario = RUMBLE_TEST;
                    clear_screen();
                    redraw = 1;
                }
                else if (selected_option == 4) {
                    current_scenario = NUNCHUCK_TEST;
                    clear_screen();
                    redraw = 1;
                }
                else if (selected_option == 5) {
                    break; // Exit to system menu
                }
            }
        }

        // ====================================================================
        // SCENARIOS RUNTIME LOOPS
        // ====================================================================
        else if (current_scenario == ACCEL_TEST) {
            if (pressed & WPAD_BUTTON_HOME) {
                current_scenario = MAIN_MENU;
                clear_screen();
                redraw = 1;
            }
            redraw = 1;
        }

        else if (current_scenario == BUTTON_TEST) {
            if (pressed & WPAD_BUTTON_HOME) {
                current_scenario = MAIN_MENU;
                clear_screen();
                redraw = 1;
            }
            redraw = 1;
        }

        else if (current_scenario == MULTIPLAYER_STATUS) {
            if (pressed & WPAD_BUTTON_HOME) {
                current_scenario = MAIN_MENU;
                clear_screen();
                redraw = 1;
            }
            redraw = 1;
        }

        else if (current_scenario == RUMBLE_TEST) {
            if (pressed & WPAD_BUTTON_A) {
                static int rumble_state = 0;
                rumble_state = !rumble_state;
                WPAD_Rumble(WPAD_CHAN_0, rumble_state);
            }
            if (pressed & WPAD_BUTTON_HOME) {
                WPAD_Rumble(WPAD_CHAN_0, 0);
                current_scenario = MAIN_MENU;
                clear_screen();
                redraw = 1;
            }
            redraw = 1;
        }

        else if (current_scenario == NUNCHUCK_TEST) {
            if (pressed & WPAD_BUTTON_HOME) {
                current_scenario = MAIN_MENU;
                clear_screen();
                redraw = 1;
            }
            redraw = 1;
        }

        // ====================================================================
        // GRAPHICAL USER INTERFACE RENDERING ENGINE
        // ====================================================================
        if (redraw) {

            if (current_scenario == MAIN_MENU) {
                printf("\x1b[2;22H\x1b[36m=================================\x1b[0m");
                printf("\x1b[3;22H\x1b[36m         WIIMOTE TESTER\x1b[0m");
                printf("\x1b[4;24H\x1b[36mWii hardware diagnostic suite\x1b[0m  ");
                printf("\x1b[5;22H\x1b[36m=================================\x1b[0m");
                printf("\x1b[7;24HSelect utility module:                  ");

                if (selected_option == 0) printf("\x1b[9;21H\x1b[44m  > [1] 3D Accelerometer Telemetry   < \x1b[0m");
                else                      printf("\x1b[9;21H\x1b[40m    [1] 3D Accelerometer Telemetry     \x1b[0m");

                if (selected_option == 1) printf("\x1b[11;21H\x1b[44m  > [2] Wiimote Button Live Test     < \x1b[0m");
                else                      printf("\x1b[11;21H\x1b[40m    [2] Wiimote Button Live Test       \x1b[0m");

                if (selected_option == 2) printf("\x1b[13;21H\x1b[44m  > [3] 4-Player Connection Status   < \x1b[0m");
                else                      printf("\x1b[13;21H\x1b[40m    [3] 4-Player Connection Status     \x1b[0m");

                if (selected_option == 3) printf("\x1b[15;21H\x1b[44m  > [4] Haptic Actuator Stress Test  < \x1b[0m");
                else                      printf("\x1b[15;21H\x1b[40m    [4] Haptic Actuator Stress Test    \x1b[0m");

                if (selected_option == 4) printf("\x1b[17;21H\x1b[44m  > [5] Nunchuk Test                 < \x1b[0m");
                else                      printf("\x1b[17;21H\x1b[40m    [5] Nunchuk Test                   \x1b[0m");

                if (selected_option == 5) printf("\x1b[19;21H\x1b[41m  > [6] Exit to Wii System Menu      < \x1b[0m");
                else                      printf("\x1b[19;21H\x1b[40m    [6] Exit to Wii System Menu        \x1b[0m");
            }

            else if (current_scenario == ACCEL_TEST) {
                struct vec3w_t accel_data;
                WPAD_Accel(WPAD_CHAN_0, &accel_data);

                printf("\x1b[2;22H\x1b[32m--- 3D ACCELEROMETER TELEMETRY (CH 0) --- \x1b[0m");
                printf("\x1b[5;28H\x1b[KAxis X: \x1b[33m%4d\x1b[0m    ", accel_data.x);
                printf("\x1b[7;28H\x1b[KAxis Y: \x1b[33m%4d\x1b[0m    ", accel_data.y);
                printf("\x1b[9;28H\x1b[KAxis Z: \x1b[33m%4d\x1b[0m    ", accel_data.z);

                printf("\x1b[14;20H\x1b[33m[Move the Wiimote in space to test sensors]\x1b[0m");
                printf("\x1b[16;24H\x1b[33m[Press HOME to return to main menu]\x1b[0m");
            }

            else if (current_scenario == BUTTON_TEST) {
                u32 held = WPAD_ButtonsHeld(WPAD_CHAN_0);

                printf("\x1b[2;22H\x1b[32m--- LIVE BUTTON STATUS TEST (CH 0) --- \x1b[0m");

                printf("\x1b[6;21H\x1b[K[A]: %-3s | [B]: %-3s | [1]: %-3s | [2]: %-3s",
                    (held & WPAD_BUTTON_A) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_B) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_1) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_2) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m");

                printf("\x1b[8;28H\x1b[K[+]: %-3s | [-]: %-3s",
                    (held & WPAD_BUTTON_PLUS) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_MINUS) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m");

                printf("\x1b[10;19H\x1b[K[UP]: %-3s | [DW]: %-3s | [LT]: %-3s | [RT]: %-3s",
                    (held & WPAD_BUTTON_UP) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_DOWN) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_LEFT) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_BUTTON_RIGHT) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m");

                printf("\x1b[16;24H\x1b[33m[Press HOME to return to main menu]\x1b[0m");
            }

            else if (current_scenario == MULTIPLAYER_STATUS) {
                printf("\x1b[2;22H\x1b[35m--- MULTIPLAYER BLUETOOTH CHANNELS --- \x1b[0m");

                for (int i = 0; i < 4; i++) {
                    u32 type;
                    int probe_res = WPAD_Probe(i, &type);
                    u8 batt = WPAD_BatteryLevel(i);
                    int pct = (int)((batt / 255.0f) * 100);

                    printf("\x1b[%d;18H\x1b[KChannel %d: ", 5 + (i * 2), i + 1);
                    if (probe_res == 0) {
                        printf("\x1b[32mCONNECTED\x1b[0m | Battery: \x1b[33m%d%%\x1b[0m | ID: \x1b[36m0x%08X\x1b[0m", pct, type);
                    }
                    else {
                        printf("\x1b[31mDISCONNECTED / EMPTY\x1b[0m");
                    }
                }

                printf("\x1b[16;24H\x1b[33m[Press HOME to return to main menu]\x1b[0m");
            }

            else if (current_scenario == RUMBLE_TEST) {
                printf("\x1b[2;24H\x1b[31m--- HAPTIC ACTUATOR STRESS TEST --- \x1b[0m");
                printf("\x1b[6;21H\x1b[KPress \x1b[32m[A]\x1b[0m to toggle the rumble motor loop.");
                printf("\x1b[8;21H\x1b[KStatus: Check physical feedback on Channel 1.");

                printf("\x1b[16;24H\x1b[33m[Press HOME to return to main menu]\x1b[0m");
            }

            else if (current_scenario == NUNCHUCK_TEST) {
                u32 held = WPAD_ButtonsHeld(WPAD_CHAN_0);

                expansion_t exp;
                WPAD_Expansion(WPAD_CHAN_0, &exp);

                printf("\x1b[2;26H\x1b[34m   --- NUNCHUCK TEST --- \x1b[0m");

				// Z and C buttons status
                printf("\x1b[6;25H\x1b[K[Z]: %-3s | [C]: %-3s",
                    (held & WPAD_NUNCHUK_BUTTON_Z) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m",
                    (held & WPAD_NUNCHUK_BUTTON_C) ? "\x1b[32mON\x1b[0m" : "\x1b[30mOFF\x1b[0m");

				// Nunchuk thumbstick position
                if (exp.type == WPAD_EXP_NUNCHUK) {
                    int stick_x = exp.nunchuk.js.pos.x - exp.nunchuk.js.center.x;
                    int stick_y = exp.nunchuk.js.pos.y - exp.nunchuk.js.center.y;

                    printf("\x1b[9;22H\x1b[KStick X: \x1b[33m%+4d\x1b[0m   ", stick_x);
                    printf("\x1b[11;22H\x1b[KStick Y: \x1b[33m%+4d\x1b[0m   ", stick_y);
                }
                else {
                    printf("\x1b[9;20H\x1b[31m  Nunchuk not connected!    \x1b[0m");
                    printf("\x1b[11;20H\x1b[K                        ");
                }

                printf("\x1b[16;24H\x1b[33m[Press HOME to return to main menu]\x1b[0m");
            }

            redraw = 0;
        }

        VIDEO_WaitVSync();
    }

    // --- EXIT & SYSTEM REBOOT ---
    clear_screen();
    printf("\x1b[3;22H\x1b[31mShutting down diagnostics. Returning to Wii Menu...\x1b[0m\n");
    WPAD_Shutdown();
    SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);

    return 0;
}