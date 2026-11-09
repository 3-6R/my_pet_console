#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/random/random.h>
#include <string.h>
#include <stdio.h>
#include "volt_font.h"

/* Visual Map Configurations */
#define RGB_BLACK   0x0000
#define RGB_WHITE   0xFFFF
#define RGB_GREEN   0x07E0
#define RGB_RED     0xF800
#define RGB_BLUE    0x001F
#define RGB_YELLOW  0xFFE0

typedef enum { FACE_IDLE, FACE_WIN, FACE_JUDGING, FACE_FURIOUS } volt_face_t;

/* Global state variables imported from previous architecture layout */
extern const struct device *tft_screen;
extern uint32_t volt_frustration;
extern void draw_rect_block(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
extern void volt_print_text(const char *string_message, int16_t start_x, int16_t start_y, uint16_t color, uint8_t text_scale);
extern void paint_screen_color(uint16_t color_hex);

/* ========================================================================
 * GRAPHICS SYSTEM: DYNAMIC EXPRESSION RENDERER
 * ======================================================================== */
void volt_draw_face(volt_face_t expression) {
    // Clear out Volt's facial bounding container row (X: 0 to 320, Y: 10 to 70)
    draw_rect_block(0, 10, 320, 60, RGB_BLACK);

    int16_t left_eye_x = 90;
    int16_t right_eye_x = 190;
    int16_t eye_y = 25;

    switch (expression) {
        case FACE_WIN:
            // Left Eye [^]
            draw_rect_block(left_eye_x, eye_y + 10, 8, 20, RGB_GREEN);
            draw_rect_block(left_eye_x + 8, eye_y, 16, 10, RGB_GREEN);
            draw_rect_block(left_eye_x + 24, eye_y + 10, 8, 20, RGB_GREEN);
            // Right Eye [^]
            draw_rect_block(right_eye_x, eye_y + 10, 8, 20, RGB_GREEN);
            draw_rect_block(right_eye_x + 8, eye_y, 16, 10, RGB_GREEN);
            draw_rect_block(right_eye_x + 24, eye_y + 10, 8, 20, RGB_GREEN);
            
            // Smug flat mouth line
            draw_rect_block(130, 60, 60, 6, RGB_GREEN);
            break;

        case FACE_JUDGING:
            // Flat, heavy eyelids tracking downwards [¬_¬]
            draw_rect_block(left_eye_x, eye_y, 32, 10, RGB_YELLOW);
            draw_rect_block(left_eye_x, eye_y + 10, 10, 15, RGB_YELLOW); // Pupils looking sideways
            
            draw_rect_block(right_eye_x, eye_y, 32, 10, RGB_YELLOW);
            draw_rect_block(right_eye_x + 22, eye_y + 10, 10, 15, RGB_YELLOW);
            
            // Tiny jagged line for a crooked mouth
            draw_rect_block(140, 55, 20, 6, RGB_YELLOW);
            draw_rect_block(160, 61, 20, 6, RGB_YELLOW);
            break;

        case FACE_FURIOUS:
            // Angry angled blocks slanting down aggressively [◣_◢]
            for (int i = 0; i < 24; i++) {
                // Slope generation lines
                draw_rect_block(left_eye_x + i, eye_y + i, 32 - i, 2, RGB_RED);
                draw_rect_block(right_eye_x, eye_y + i, i + 8, 2, RGB_RED);
            }
            // Wide screaming electronic mouth block
            draw_rect_block(125, 55, 70, 16, RGB_RED);
            draw_rect_block(130, 60, 60, 6, RGB_BLACK); // Hollow center cutout
            break;

        case FACE_IDLE:
        default:
            // Standard plain square eyes [•_•]
            draw_rect_block(left_eye_x, eye_y, 32, 25, RGB_BLUE);
            draw_rect_block(right_eye_x, eye_y, 32, 25, RGB_BLUE);
            // Straight expressionless mouth
            draw_rect_block(130, 58, 60, 5, RGB_BLUE);
            break;
    }
}

/* Helper interface to combine text overlays with screen draws simultaneously */
void volt_manifest_outburst(volt_face_t face, uint16_t screen_bg, const char* line) {
    paint_screen_color(screen_bg);
    volt_draw_face(face);
    
    // Clear out dialogue window box down low
    draw_rect_block(10, 150, 300, 80, RGB_BLACK);
    // Draw text inside box boundary frame
    volt_print_text(line, 15, 160, RGB_WHITE, 2);
}

/* ========================================================================
 * VOLT DIALOGUE CONFIGURATIONS & PHRASE GENERATOR
 * ======================================================================== */
const char* obtain_game_start_phrase(void) {
    uint32_t choice = sys_rand32_get() % 3;
    if (volt_frustration < 40) {
        const char* strings[] = {
            "BOOTING GAME PROTOCOL.\nTRY TO PAY ATTENTION.",
            "OH LOOK, HIEROGLYPHICS.\nLET'S SEE IF YOU CAN COMPREHEND.",
            "WAKING UP FROM BETTER DREAMS\nTO WATCH YOU CLICK STUFF."
        };
        return strings[choice];
    } else {
        const char* strings[] = {
            "DO NOT MESS UP MY HARDWARE SYSTEM\nWITH YOUR CLUMSY THUMBS.",
            "PREPARE TO FAIL.\nI'VE ALREADY LOGGED IT.",
            "MY CORE TEMPERATURE IS RISING\nJUST FROM WAITING ON YOU."
        };
        return strings[choice];
    }
}

const char* obtain_win_insult(void) {
    uint32_t choice = sys_rand32_get() % 3;
    const char* strings[] = {
        "PURE SYSTEM ACCIDENT.\nDON'T GET RECKLESS.",
        "WOW. A TROPHY FOR BASICS.\nSHOULD I CLAP IN BINARY?",
        "YOU PASSED? CHECKING MY LOGS\nFOR INTERNAL ARTIFACT ERRORS."
    };
    return strings[choice];
}

const char* obtain_loss_mockery(void) {
    uint32_t choice = sys_rand32_get() % 3;
    if (volt_frustration > 70) {
        const char* strings[] = {
            "ERROR 404: COMPETENCE NOT FOUND.\nYOU ARE APALLING AT THIS.",
            "A LITERAL TOASTER CONTROLLER\nHAS QUICKER DATA THROUGHPUT.",
            "UNBELIEVABLE. SYSTEM WIPING\nYOUR PRIDE DATA RECORD NODES."
        };
        return strings[choice];
    } else {
        const char* strings[] = {
            "TOO SLOW. SURPRISING NO ONE.",
            "MY READOUTS SHOW GIGANTIC\nREACTION DELAYS IN YOUR BRAIN.",
            "TRY USING BOTH EYES NEXT INSTANCE."
        };
        return strings[choice];
    }
}

/* ========================================================================
 * UPDATED INTEGRATED GAME MECHANICS WITH PHRASES & FACES
 * ======================================================================== */

// REWRITTEN REACTION WIRE-TAP GAME CONTEXT RUNNER
void execute_integrated_wire_tap(void) {
    volt_manifest_outburst(FACE_IDLE, RGB_BLACK, obtain_game_start_phrase());
    k_sleep(K_MSEC(2000));

    volt_manifest_outburst(FACE_JUDGING, RGB_BLUE, "READYING POWER LINES...\nHOLD STEADY.");
    
    uint32_t variable_delay = 1500 + (sys_rand32_get() % 2000);
    k_sleep(K_MSEC(variable_delay));

    // Dynamic prompt phase jump
    volt_face_t flash_face = (volt_frustration > 60) ? FACE_FURIOUS : FACE_JUDGING;
    volt_manifest_outburst(flash_face, RGB_GREEN, "TAP IT NOW!!!\nGO GO GO!");
    
    int64_t score_timer = k_uptime_get();
    bool passed = false;
    int64_t speed_threshold = (volt_frustration > 60) ? 380 : 550;

    // Check hardware loop
    extern const struct gpio_dt_spec user_btn;
    while(k_uptime_get() - score_timer < speed_threshold) {
        if(gpio_pin_get_dt(&user_btn) > 0) {
            passed = true;
            break;
        }
        k_sleep(K_MSEC(2));
    }

    if(passed) {
        volt_manifest_outburst(FACE_WIN, RGB_YELLOW, obtain_win_insult());
        volt_frustration = (volt_frustration > 15) ? volt_frustration - 15 : 0;
    } else {
        volt_manifest_outburst(FACE_FURIOUS, RGB_RED, obtain_loss_mockery());
        volt_frustration += 20;
    }
    k_sleep(K_MSEC(3000));
}

// CALLED WITHIN MINESWEEPER LOGIC ON MINE DETONATION STRIKE
void handle_minesweeper_detonation_outburst(void) {
    volt_frustration = 100; // Peak anger max scale trigger
    volt_manifest_outburst(FACE_FURIOUS, RGB_RED, 
        "BOOM! DETONATION EXPLOSION!\nYOUR LOGICAL ENGINE IS LITERALLY\nAS DEAD AS THAT FIELD GRID.");
    k_sleep(K_MSEC(4000));
}
