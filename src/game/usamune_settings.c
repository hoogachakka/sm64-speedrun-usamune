#include "usamune.h"
#include "usamune_settings.h"
#include "game_init.h"
#include "object_list_processor.h"
#include "level_update.h"
#include "save_file.h"
#include "memory.h"
#include "behavior_data.h"
#include <string.h>
#include <stdio.h>

extern u8 uInEditMode;

u8 uGlobalSettingsTable[88] = {0};

static void usamune_menu_render_subsetting(uMenuSetting* setting, s32 x, s32 y);
static void usamune_menu_render_editable_field(const char* str, const char* fmt, u8 dataField, u8 posField, u8 spacing, u8 numDigits, s32 startPos);
static void usamune_setting_two_options_menu(uMenuSetting* setting);
static u32 usamune_setting_two_options_render(uMenuSetting* setting);
extern void usamune_menu_change_setting(uMenuSetting* setting, u8 param_2);

extern u16 uMenuFadeTimer;
extern struct MarioState gMarioStates[1];
extern struct MarioState *gMarioState;
extern s8 sYoshiDead;

uMenuSetting uTimerSettingsTable[] = { 
  INIT_SETTING("Stop",    TIMER_STOP,    1, "None", "Grab", "*Cam", "Grab*"),
  INIT_SETTING("Display", TIMER_DISPLAY, 1, "Hide", "Grab", "*Cam", "Always"),
  INIT_SETTING("RealTmr", TIMER_REALTMR, 1, "Hide", "Grab", "*Cam", "Fade"),
  INIT_SETTING("PSSRace", TIMER_PSSRACE, 1, "Normal", "Race", "Skip", "End"),
  INIT_SETTING("FadeTmr", TIMER_FADETMR, 1, "Off", "On"),
  INIT_SETTING("SecTmr",  TIMER_SECTMR,  1, "Off", "On"),
  INIT_SETTING("RTATmr",  TIMER_RTATMR,  1, "Off", "Comb", "Grab", "Fade"),
  INIT_SETTING("TmrPosi", TIMER_TMRPOSI, 1, "Normal", "Left")
};

uMenuSetting uMiscTSettingsTable[] = {
  INIT_SETTING("MiscTmr", MISCT_MISCTMR, 1, "Off", "On"),
  INIT_SETTING("NumDisp", MISCT_NUMDISP, 1, "Off", "On"),
  INIT_SETTING("*Cam",    MISCT_XCAM,    1, "Off", "On"),
  INIT_SETTING("BSwitch", MISCT_BSWITCH, 1, "Off", "On"),
  INIT_SETTING("PSwitch", MISCT_PSWITCH, 1, "Off", "On"),
  INIT_SETTING("Ride",    MISCT_RIDE,    1, "Off", "On"),
  INIT_SETTING("Chest",   MISCT_CHEST,   1, "Off", "On"),
  INIT_SETTING("Crystal", MISCT_CRYSTAL, 1, "Off", "On"),
  INIT_SETTING("Enemy",   MISCT_ENEMY,   1, "Off", "On"),
  INIT_SETTING("ExclBox", MISCT_EXCLBOX, 1, "Off", "On"),
  INIT_SETTING("CorkBox", MISCT_CORKBOX, 1, "Off", "On"),
  INIT_SETTING("ObjGrb",  MISCT_OBJGRB,  1, "Off", "On"),
  INIT_SETTING("LdgeGrb", MISCT_LDGEGRB, 1, "Off", "On"),
  INIT_SETTING("PoleGrb", MISCT_POLEGRB, 1, "Off", "On"),
  INIT_SETTING("WK",      MISCT_WK,      1, "Off", "On"),
  INIT_SETTING("WallHit", MISCT_WALLHIT, 1, "Off", "On"),
  INIT_SETTING("Clip",    MISCT_CLIP,    1, "Off", "On"),
  INIT_SETTING("Surface", MISCT_SURFACE, 1, "Off", "On"),
  INIT_SETTING("Flip",    MISCT_FLIP,    1, "Off", "On"),
  INIT_SETTING("Rollout", MISCT_ROLLOUT, 1, "Off", "On"),
  INIT_SETTING("Dive",    MISCT_DIVE,    1, "Off", "On"),
  INIT_SETTING("LJSldk",  MISCT_LJSLDK,  1, "Off", "On"),
  INIT_SETTING("TJ",      MISCT_TJ,      1, "Off", "On"),
  INIT_SETTING("Load",    MISCT_LOAD,    1, "Off", "On"),
  INIT_SETTING("Bowser",  MISCT_BOWSER,  1, "Off", "On"),
  INIT_SETTING("Coin",    MISCT_COIN,    1, "Off", "On"),
  INIT_SETTING("1Up",     MISCT_1UP,     1, "Off", "On")
};

uMenuSetting uResetSettingsTable[] = {
  INIT_SETTING("LButton", RESET_LBUTTON, 1, "None", "LReset", "LEntry", "SSlect"),
  INIT_SETTING("L*R",     RESET_LxR,     1, "None", "LReset", "LEntry", "SSlect"),
  INIT_SETTING("InitBGM", RESET_INITBGM, 1, "Off", "On"),
  INIT_SETTING("SStates", RESET_SSTATES, 1, "Off", "On"),
  INIT_SETTING("RandStg", RESET_RANDSTG, 0, "Off", "-*70", "-120")
};

uMenuSetting uDefltSettingsTable[] = {
  INIT_SETTING("DefCoin", DEFLT_DEFCOIN, 0, "Off", "On"),
  INIT_SETTING("DefTime", DEFLT_DEFTIME, 0, "Off", "On"),
  INIT_SETTING("DefRed",  DEFLT_DEFRED,  0, "Off", "On"),
  INIT_SETTING("DefRNG",  DEFLT_DEFRNG,  0, "Off", "On"),
  INIT_SETTING("DefSwim", DEFLT_DEFSWIM, 0, "Off", "On"),
  INIT_SETTING("DefMiss", DEFLT_DEFMISS, 0, "Off", "On"),
  INIT_SETTING("DefCurs", DEFLT_DEFCURS, 0, "Off", "On")
};

uMenuSetting uHudSettingsTable[] = {
  INIT_SETTING("Life",    HUD_LIFE,    1, "Normal", "Lag", "FSpeed", "VSpeed", "IGMenu"),
  INIT_SETTING("Star",    HUD_STAR,    1, "Normal", "Lag", "FSpeed", "VSpeed", "IGMenu"),
  INIT_SETTING("Dust",    HUD_DUST,    1, "Off", "On"),
  INIT_SETTING("Input",   HUD_INPUT,   1, "Off", "OnVal", "OnVis", "OnComb"),
  INIT_SETTING("WKDisp",  HUD_WKDISP,  1, "Off", "On"),
  INIT_SETTING("Attempt", HUD_ATTEMPT, 1, "Off", "On", "Alert", "Grab"),
  INIT_SETTING("AtmpFlt", HUD_ATMPFLT, 0, "Off", "On"),
  INIT_SETTING("AtmpRst", HUD_MEMORY,  1, "Off", "Slot1", "Slot2", "Slot3", "Slot4", "Slot5")
};

uMenuSetting uDataSettingsTable[] = {
  INIT_SETTING("Cannon",  DATA_CANNON,     4, "Normal", "Close", "Open"),
  INIT_SETTING("Cap",     DATA_CAP,        4, "Normal", "On", "Off"),
  INIT_SETTING("120File", DATA_120FILE,    4, "Off", "On"),
  INIT_SETTING("For-0",   DATA_FOR0STAR,   4, "Off", "DWEnd", "FSEnd"),
  INIT_SETTING("For-16",  DATA_FOR16STAR,  4, "Off", "-*4", "-*8", "-*9", "-*15", "-*16"),
  INIT_SETTING("For-70",  DATA_FOR70STAR,  4, "Off", "-*19", "-*20", "-*34", "-*38", "-*39", "-*58"),
  INIT_SETTING("For-120", DATA_FOR120STAR, 4, "Off", "-*26", "-*33", "-*42", "-*49", "-*52", "-*59", "-*74", "-103")
};

uMenuSetting uStageSettingsTable[] = {
  INIT_SETTING("WDW",     STAGE_WDW,     1, "Normal", "Low", "Mid", "High"),
  INIT_SETTING("TTC",     STAGE_TTC,     1, "Normal", "Stop", "Slow", "Rand", "Fast"),
  INIT_SETTING("CCMPeng", STAGE_CCMPENG, 1, "Normal", "Thin"),
  INIT_SETTING("DDDSub",  STAGE_DDDSUB,  1, "Normal", "Spawn", "Despwn"),
  INIT_SETTING("DDDMant", STAGE_DDDMANT, 0, "Normal", "VisFix"),
  INIT_SETTING("JRBMist", STAGE_JRBMIST, 0, "Normal", "Force"),
  INIT_SETTING("Switch",  STAGE_SWITCH,  1, "Normal", "Unpres"),
  INIT_SETTING("Mips",    STAGE_MIPS,    1, "Normal", "15Mips", "50Mips"),
  INIT_SETTING("Toad",    STAGE_TOAD,    1, "Normal", "Spawn"),
  INIT_SETTING("Moat",    STAGE_MOAT,    0, "Normal", "Fill", "Drain"),
  INIT_SETTING("StgTxt",  STAGE_STGTXT,  0, "Normal", "Once", "Always", "Skip"),
  INIT_SETTING("StarTxt", STAGE_STARTXT, 0, "Normal", "-1Txt", "-3Txt", "-8Txt", "-30Txt", "-50Txt", "-70Txt")
};

uMenuSetting uMiscSettingsTable[] = {
  INIT_SETTING("Preset",  MISC_PRESET,    6, "Custom", "Vanila", "Prac", "StgRTA", "Sgstar", "CatExt", "NonStp"),
  INIT_SETTING("IGMenu",  MISC_IGMENU,    0, "Normal", "Auto"),
  INIT_SETTING("Music",   MISC_MUSIC,     1, "On", "Off"),
  INIT_SETTING("-Color",  MISC_STARCOLOR, 1, "Normal", "Yellow"),
  INIT_SETTING("Mode",    MISC_MODE,      7, "N64", "Emu"),
  INIT_SETTING("Intro",   MISC_INTRO,     0, "Normal", "Skip"),
  INIT_SETTING("NonStop", MISC_NONSTOP,   0, "Off", "On"),
  INIT_SETTING("STJ",     MISC_STJ,       0, "Off", "On")
};

INIT_SUBSETTING("RTARst",  TIMER_RTATMR,  0x1F, 1, "Auto", "Manual");
INIT_SUBSETTING("LoadBtn", RESET_SSTATES, 0x1A, 5, "C*4", "DUp");
INIT_SUBSETTING("Texture", HUD_INPUT,     0x52, 1, "Small", "Large");
INIT_SUBSETTING("LagDisp", HUD_LIFE,      0x4B, 1, "Normal", "1Digit");
INIT_SUBSETTING("LagDisp", HUD_STAR,      0x57, 1, "Normal", "1Digit");

uMenuSettingCategory uMenuSettingCategories[] = {
  { "Timer", 0, 8,  uTimerSettingsTable },
  { "MiscT", 0, 27, uMiscTSettingsTable },
  { "Reset", 0, 5,  uResetSettingsTable },
  { "Deflt", 0, 7,  uDefltSettingsTable },
  { "HUD",   0, 8,  uHudSettingsTable   },
  { "Data",  0, 7,  uDataSettingsTable  },
  { "Stage", 0, 12, uStageSettingsTable },
  { "Misc",  0, 8,  uMiscSettingsTable  }
};

/* APPLY FUNCTIONS */

void usamune_setting_MISC_STJ_apply(uMenuSetting* setting, UNUSED u8 prevOption, UNUSED s16 saveFile) {
  static s16 lives = -1; 

  if (*(setting->currOption) == 1) {
    if (gMarioObject == NULL) {
      lives = 4;
    }
    else {
      lives = gMarioState->numLives;
    }
    gMarioState->numLives = 100;
    gHudDisplay.lives = 100;

    gSpecialTripleJump = TRUE;
    sYoshiDead = TRUE;

    //void* addr = segmented_to_virtual(bhvYoshi);
    //TODO: implement whatever tf this function does past here

  }
  else if (*(setting->currOption) == 0 && lives != -1) {
    gMarioState->numLives = lives;
    gHudDisplay.lives = gMarioState->numLives;
    lives = -1;

    gSpecialTripleJump = FALSE;
    sYoshiDead = FALSE;
  }  
}


/* MENU FUNCTIONS */

void usamune_setting_TIMER_RTATMR_menu_bhv(uMenuSetting* setting) {
  if (*(setting->currOption) != 0) {
    usamune_setting_two_options_menu(setting);
    
  }
}

void usamune_setting_RESET_SSTATES_menu_bhv(uMenuSetting* setting) {
  if (*(setting->currOption) != 0) {
    usamune_setting_two_options_menu(setting);
  }
}

void usamune_setting_HUD_INPUT_menu_bhv(uMenuSetting* setting) {
  if (*(setting->currOption) != 0) {
    usamune_setting_two_options_menu(setting);
  }
}

void usamune_setting_HUD_LIFE_menu_bhv(uMenuSetting* setting) {
  if (*(setting->currOption) == 1) {
    usamune_setting_two_options_menu(setting);
  }
}

void usamune_setting_HUD_STAR_menu_bhv(uMenuSetting* setting) {
  if (*(setting->currOption) == 1) {
    usamune_setting_two_options_menu(setting);
  }
}


/* RENDER FUNCTIONS */

u32 usamune_setting_TIMER_RTATMR_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_subsetting(setting->subSetting, 292, 40);
  return 1;
}

u32 usamune_setting_RESET_SSTATES_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_subsetting(setting->subSetting, 292, 40);
  return 1;
}

u32 usamune_setting_RESET_RANDSTG_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_print_text(32, 27, "Press A to warp");
  return 1;
}

u32 usamune_setting_DEFLT_DEFRNG_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_editable_field("Value", "%05d", 0, 0, 8, 5, 164);
  return 1;
}

u32 usamune_setting_DEFLT_DEFMISS_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_editable_field("Mission", "%0d", 1, 0, 8, 1, 188);
  return 1;
}

u32 usamune_setting_DEFLT_DEFRED_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_editable_field("Count", "%0d", 1, 0, 8, 1, 212);
  return 1;
}

u32 usamune_setting_DEFLT_DEFSWIM_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_editable_field("Strength", "%03d", 0, 0, 8, 2, 152);
  return 1;
}

u32 usamune_setting_DEFLT_DEFCURS_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_print_text(163, 40, "Dir");
  usamune_print_text_fmt_int(202, 40, "%0d", 0);
  usamune_menu_render_editable_field("Tmr", "%02d", 0, 0, 3, 2, 229);
  return 1;
}

u32 usamune_setting_DEFLT_DEFCOIN_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_print_text(226, 40, "+");
  usamune_print_text(242, 40, "*");
  usamune_menu_render_editable_field("", "%03d", 0, 0, 2, 3, 254);
  return 1;
}



u32 usamune_setting_HUD_INPUT_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_menu_render_subsetting(setting->subSetting, 292, 40);
  return 1;
}


  
u32 usamune_setting_HUD_LIFE_render(uMenuSetting* setting) {
  if (*(setting->currOption) != 1) {
    return 0;
  }
  usamune_menu_render_subsetting(setting->subSetting, 292, 40);
  return 1;
}


  
u32 usamune_setting_HUD_STAR_render(uMenuSetting* setting) {
  if (*(setting->currOption) != 1) {
    return 0;
  }
  usamune_menu_render_subsetting(setting->subSetting, 292, 40);
  return 1;
}

/* u32 usamune_setting_STAGE_STARTXT_render(uMenuSetting* setting) { */
/*   if (*(setting->currOption) == 0) { */
/*     return 0; */
/*   } */
/*   usamune_print_text(127, 40, "Cur"); */
/*   usamune_print_text_fmt_int(166, 40, "%03d", 0); */
/*   usamune_print_text(217, 40, "Cnt"); */
/*   usamune_print_text_fmt_int(256, 40, "%03d", 0); */
/*   if (!uInEditMode) { */
/*     usamune_print_text(124, 22, "Press A to edit"); */
/*   } */
/*   else { */
/*     s32 x = 0; */
/*     if (DAT_80417c6a < 3) { */
/*       x = 12 * (2 - DAT_80417c6a) + 256; */
/*     } */
/*     usamune_print_text(x, 26, "^"); */
/*   } */
/*   return 1; */
/* } */

u32 usamune_setting_MISC_PRESET_render(uMenuSetting* setting) {
  if (*(setting->currOption) == 0) {
    return 0;
  }
  usamune_print_text(32, 27, "Press A to overwrite");
  return 1;
}

static void usamune_setting_two_options_menu(uMenuSetting* setting) {
  if (gPlayer1Controller->buttonPressed & U_CBUTTONS) {
    usamune_menu_change_setting(setting->subSetting, 1);
  }
  else if (gPlayer1Controller->buttonPressed & D_CBUTTONS) {
    usamune_menu_change_setting(setting->subSetting, 2);
  }
}

static u32 usamune_setting_two_options_render(uMenuSetting* setting) {
  usamune_menu_render_subsetting(setting->subSetting, 292, 40);
  return 1;
}

static void usamune_menu_render_editable_field(const char* str, const char* fmt, u8 dataField, u8 posField, u8 spacing, u8 numDigits, s32 startPos) {
  s32 writeCursor = startPos;
  usamune_print_text(writeCursor, 40, str);
  writeCursor += 12 * strlen(str) + spacing;
  usamune_print_text_fmt_int(writeCursor, 40, fmt, dataField);
  if (!uInEditMode) {
    usamune_print_text(112, 22, "Press A to edit");
  }
  else {
    s32 x = writeCursor;
    if (posField < numDigits) {
      x += 12 * ((numDigits - 1) - posField);
    }
    usamune_print_text(x, 26, "^");
  }
}

static void usamune_menu_render_subsetting(uMenuSetting* setting, s32 x, s32 y) {
  u8 arrowIndex = *(setting->currOption);
  for (int i = 0; i < setting->numOptions; i++) {
    const char* str = setting->options[i];
    s32 xPos = x;
    if (*str != 0) {
      xPos -= (12 * strlen(str));
    }
    if (i != arrowIndex) {
      gDPSetEnvColor(gDisplayListHead++, 165, 165, 165, uMenuFadeTimer);
    }
    else {
      gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, uMenuFadeTimer);
      usamune_print_text(xPos - 15, y, ">");
    }
    usamune_print_text(xPos, y, str);
    y -= 18;
  }
  gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, uMenuFadeTimer);
}

