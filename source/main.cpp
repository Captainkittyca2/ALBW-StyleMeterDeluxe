/**
 * @file main.cpp
 * @author Captain Kitty Cat (youtube.com/@captainkittyca2)
 * @brief Overhaul the oil meter to function similarly to ALBW.
 * @version 1.0
 * @date 2024-05-06
 *
 * @copyright Copyright (c) 2024
 *
 */
//#include <display/console.h>     // Contains a very neat helper class to print to the console
#include <main.h>
#include <patch.h>     // Contains code for hooking into a function
#include <cstdio>
#include <cstring>
//#include <tp/f_ap_game.h>
#include <tp/JFWSystem.h>
#include <gc_wii/OS.h>
#include <gc_wii/OSTime.h>
#include <tp/d_pane_class.h>
#include <tp/TColor.h>
//#include <tp/d_meter2.h>
#include <tp/d_meter2_info.h>
#include <tp/d_meter_HIO.h>
#include <tp/d_com_inf_game.h>
//#include <tp/d_a_alink.h>
#include <tp/f_op_actor_mng.h>
#include <tp/d_menu_window.h>
#include <tp/d_menu_window_HIO.h>
#include <tp/processor.h>
#include <tp/m_do_audio.h>
#include <tp/JKRArchivePub.h>
#include <tp/d_item.h>
#include <tp/d_item_data.h>
#include <tools.h>

#ifdef PLATFORM_WII
#include <tp/m_re_controller_pad.h>
#else
#include <tp/m_do_controller_pad.h>
#endif

#define MSG_COLOR(id) "\x1A\x06\xFF\x00\x00" id
#define MSG_COLOR_UNK "\x00"
#define MSG_COLOR_RED "\x01"
#define MSG_COLOR_GREEN "\x02"
#define MSG_COLOR_BLUE "\x03"
#define MSG_COLOR_YELLOW "\x04"
#define MSG_COLOR_PURPLE "\x06"
#define MSG_COLOR_WHITE "\x07"
#define MSG_BULLET "\x1a\x05\x06\x00\x0a"
#define MSG_UNK1 ""
#ifdef PLATFORM_WII
#define SPINNERPAUSE 0x804a4170             //USA 1.2
/*#define SPINNERPAUSE 0x804bcb48*/         //USA 1.0
#else
#define SPINNERPAUSE 0x80430158
#endif
#define SPINNERSPINN 0x5370696e

namespace mod
{
    Mod* gMod = nullptr;
    uint32_t kanteraVisible = reinterpret_cast<uint32_t>(libtp::tp::d_meter2::alphaAnimeKantera);
    uint32_t ammoSilence = reinterpret_cast<uint32_t>(libtp::tp::d_meter2::alphaAnimeButton);
    uint32_t spinnerSpin;
    bool altWarpVerify = false;
    bool altWarpBlink = false;
    uint8_t procAltWarpBlink = 0;
#ifdef PLATFORM_WII
    char altWarp[] = "Press and Hold C to warp to Ordon";
    uint32_t ammoForgor = reinterpret_cast<uint32_t>(libtp::tp::d_meter2::alphaAnimeButtonCross);
    uint32_t ButtonAlphaThing = reinterpret_cast<uint32_t>(libtp::tp::d_meter2_draw::setAlphaButtonCrossAnimeMax);
#else
    char altWarp[] = "Press and Hold Z to warp to Ordon";
    uint32_t ButtonAlphaThing = reinterpret_cast<uint32_t>(libtp::tp::d_meter2_draw::setAlphaButtonAnimeMax);
#endif

    void main()
    {
        Mod* mod = new Mod();
        mod->init();
    }

    void exit() {}

    Mod::Mod() : c( 0 )
    {
        trimer = 0;
    }
    libtp::tp::jfw_system::SystemConsole* sysConsolePtr = libtp::tp::jfw_system::systemConsole;
    void meterValuee(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);
    void arrowValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);
    void bombValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);
    void pachinkoValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);

    bool arrowMake = false;
    bool outlinePhase = false;
    uint8_t outlineR = 0;
    uint8_t outlineG = 0;

    void colorOilMeter(libtp::tp::d_pane_class::CPaneMgr* panePtr, libtp::tp::JUtility::TColor* awesome, libtp::tp::JUtility::TColor* wholesome) {
        if (arrowMake) {
            awesome->set(0, 0, 0, 255);
            libtp::tp::d_pane_class::setBlackWhite(libtp::tp::d_meter2_info::g_meter2_info.mMeterClass->mpMeterDraw->mpMagicBase, awesome, awesome);
            awesome->set(201, 164, 255, 255);
            wholesome->set(205, 0, 255, 255);
            
            libtp::tp::d_pane_class::setBlackWhite(panePtr, awesome, wholesome);
        }
        else {
            if (outlinePhase == false && (outlineR != 0xAF || outlineG != 0x22)) {
                if (outlineR < 0xAF) outlineR += 8;
                else if (outlineR > 0xAF) outlineR = 0xAF;
                if (outlineG < 0x22) outlineG += 4;
                else if (outlineG > 0x22) outlineG = 0x22;
                awesome->set(outlineR, outlineG, 0x00, 0xFF);
                if (outlineR == 0xAF && outlineG == 0x22) outlinePhase = true;
            } else if (outlinePhase && (outlineR != 0 || outlineG != 0)) {
                if (outlineR > 8) outlineR -= 8;
                else if (outlineR > 0xAF) outlineR = 0;
                if (outlineG > 4) outlineG -= 4;
                else if (outlineG > 0x22) outlineG = 0;
                awesome->set(outlineR, outlineG, 0x00, 0xFF);
                if (outlineR <= 0x08 && outlineG <= 0x08) outlinePhase = false;
            }

            libtp::tp::d_pane_class::setBlackWhite(libtp::tp::d_meter2_info::g_meter2_info.mMeterClass->mpMeterDraw->mpMagicBase, awesome, awesome);
        }
    }

    void silentAmmo(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) {
        for (int i = 0; i < 4; i++) {
            //libtp::tp::d_meter2_draw::setButtonIconAlpha(dMeterPtr->mpMeterDraw, i, dMeterPtr->mItemStatus[i * 2], dMeterPtr->mStatus, dMeterPtr->field_0x128 == 0 ? true : false);

            if (dMeterPtr->field_0x128 == 0 && libtp::tp::d_meter2_info::g_meter2_info.mMiniGameItemSetFlag != 1 &&
                dMeterPtr->mItemStatus[i * 4] == 0x76)
            {
                libtp::tp::d_meter2_draw::drawItemNum(dMeterPtr->mpMeterDraw, i, 1.0f);
            }
        }
    }

    bool Armor = false;
    bool preservationCommence = false;
    uint32_t rupeePreservation = 0;


    void silentArmor() {
        if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentForm == 0)
        {
            Armor = true;
            if (arrowMake == false && preservationCommence == false) {
                rupeePreservation = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees;
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees = 0;
                preservationCommence = true;
            }
        }
    }

    /*void silentArmor3(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        if (arrowMake == false) {
            libtp::tp::d_a_alink::setMagicArmorBrk(0);
            //libtp::tp::d_a_alink::seStartOnlyReverb(linkActrPtr, 0x200D7);
            (void)linkActrPtr;
        }
    }*/

    bool boomThrow = false;

    int32_t Mod::meterDamaged(libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t a1, int32_t a2, int32_t a3, int32_t a4) {
        if (arrowMake && libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.equipment[0] == 0x30) boomThrow = true;
        return damageMeter(linkActrPtr, a1, a2, a3, a4);
    }

    bool meterOn = false;
    bool mstrSwrdPlld = false;
    uint8_t delayingTheMoment = 0;

    void meterVisibility(libtp::tp::d_meter2_draw::dMeter2Draw_c* boi) {
        if (meterOn || (mstrSwrdPlld == false && libtp::tp::d_save::isCollect(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_collect, 1, 0x1) && delayingTheMoment >= 25))
            libtp::tp::d_meter2_draw::setAlphaKanteraAnimeMax(boi);
        else
            libtp::tp::d_meter2_draw::setAlphaKanteraAnimeMin(boi);
    }

    int32_t downNope() {
        return false;
    }

    void* getZel00BmgInf()
    {
        uint32_t infPtrRaw = reinterpret_cast<uint32_t>(libtp::tp::JKRArchivePub::JKRArchivePub_getGlbResource(
            0x524F4F54, // ROOT
            "zel_00.bmg",
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mMsgDtArchive[0]));

        // getGlbResource gets a pointer to MESGbmg1, but we need a pointer to INF1, which is just past MESGbmg1, and MESGbmg1
        // has a size of 0x20
        return reinterpret_cast<void*>(infPtrRaw + 0x20);
    }

    void* getInf1Ptr(const char* file)
    {
        uint32_t infPtrRaw = reinterpret_cast<uint32_t>(
            libtp::tp::JKRArchivePub::JKRArchivePub_getGlbResource(/* ROOT */ 0x524F4F54, file, nullptr));

        // getGlbResource gets a pointer to MESGbmg1, but we need a pointer to INF1, which is just past MESGbmg1, and MESGbmg1
        // has a size of 0x20
        return reinterpret_cast<void*>(infPtrRaw + 0x20);
    }

    bool meterinc = false;
    bool rentingTime = false;
    bool itemshopping[12];
    //char slingShop[] = ;
    /*char boomShop[] = ;
    char bowShop[] = ;
    char bombShop[] = ;
    char wbombShop[] = ;
    char mbombShop[] = ;
    char clawShop[] = ;
    char dclawShop[] = ;
    char spinShop[] = ;
    char sussyShop[] = ;
    char ballandShop[] = ;
    char marmorShop[] = ;*/
    char bufferOrSmthgIdkIPreferPythonForStringsTbh[20];
    const char *shopSelections[12] = {"Slingshot", "Boomerang", "Bow", "Bombs", 
                                    "Water Bombs", "Bomblings", "Clawshot", "Double Clawshot", 
                                    "Spinner", "Dominion Rod", "Ball & Chain", "Armor"};
    uint16_t rentingPrice[12] = {10, 100, 30, 50, 100, 50, 100, 200, 40, 20, 200, 500};
    uint8_t itemHexValues[12] = {0x4b, 0x40, 0x43, 0x70, 0x71, 0x72, 0x44, 0x47, 0x41, 0x46, 0x42, 0x30};
    int8_t nowLine = 0;
    int8_t java;

    void insertCustomMessage(libtp::tp::control::TControl* control, const void* TProcessor, uint16_t unk3, uint16_t msgID)
    {
        auto setMessageText = [=](const char* text)
        {
            if (text)
            {
                control->msg = text;
                control->wMsgRender = text;
            }
        };
        const void* unk = libtp::tp::processor::getResource_groupID(TProcessor, unk3);
        if (!unk) return;
        const void* currentInf1 = *reinterpret_cast<void**>(reinterpret_cast<uint32_t>(unk) + 0xC);

        if (currentInf1 == getZel00BmgInf())
        {
#ifdef PLATFORM_WII
            uint16_t bowDescription = 0xc68;

            uint16_t slingShotDescription = 0xc70;

            uint16_t NBombDescription = 0xc95;

            uint16_t WBombDescription = 0xc96;

            uint16_t MBombDescription = 0xc97;
#else
            uint16_t bowDescription = 0x2a7;

            uint16_t slingShotDescription = 0x2af;

            uint16_t NBombDescription = 0x2d4;

            uint16_t WBombDescription = 0x2d5;

            uint16_t MBombDescription = 0x2d6;
#endif


            if (msgID == bowDescription || msgID == slingShotDescription || msgID == NBombDescription || msgID == WBombDescription || msgID == MBombDescription) {
                const char* newMessage;
                if (msgID == bowDescription) newMessage = "\x1a\x09\x03\x00\x00\x00\x00\x0c\x68It's said that this Goron treasure was left\nbehind by a hero of the past.\n\n\x1a\x05\x06\x00\x0aPress and hold \x1a\x05\x00\x00\x10 or \x1a\x05\x00\x00\x0f, use \x1a\x05\x00\x00\x13 to \n\x1a\x05\x06\x00\x0btarget, and release to fire.\n" MSG_BULLET "Consumes " MSG_COLOR(MSG_COLOR_PURPLE) "1/5 " MSG_COLOR(MSG_COLOR_UNK) "off the meter.";
                else if (msgID == slingShotDescription) newMessage = "\x1a\x09\x03\x00\x00\x00\x00\x0c\x70\n" MSG_BULLET "Hold down \x1a\x05\x00\x00\x10 or \x1a\x05\x00\x00\x0f, target with \x1a\x05\x00\x00\x13, and\n\x1a\x05\x06\x00\x0brelease to shoot.\n" MSG_BULLET "If you shoot while targeting with \x1a\x05\x00\x00\x0d, your\n\x1a\x05\x06\x00\x0bshot will be more accurate.\n" MSG_BULLET "Consumes " MSG_COLOR(MSG_COLOR_PURPLE) "1/4 " MSG_COLOR(MSG_COLOR_UNK) "off the meter.";
                else if (msgID == NBombDescription) newMessage = "\x1a\x09\x03\x00\x00\x00\x00\x0c\x95Normal bombs. Consumes " MSG_COLOR(MSG_COLOR_PURPLE) "1/3 " MSG_COLOR(MSG_COLOR_UNK) "off meter.\n" MSG_BULLET "Lift one with \x1a\x05\x00\x00\x10 or \x1a\x05\x00\x00\x0f and press \x1a\x05\x00\x00\x0a to\n\x1a\x05\x06\x00\x0bset it down. Press \x1a\x05\x00\x00\x0a while running\n\x1a\x05\x06\x00\x0bto throw.\n" MSG_BULLET "Set the " MSG_COLOR(MSG_COLOR_RED) "Hero's Bow" MSG_COLOR(MSG_COLOR_UNK) " and use \x1a\x05\x00\x00\x0e on the\n\x1a\x05\x06\x00\x0bItems screen to make " MSG_COLOR(MSG_COLOR_RED) "bomb arrows" MSG_COLOR(MSG_COLOR_UNK) ".\n";
                else if (msgID == WBombDescription) newMessage = "\x1a\x09\x03\x00\x00\x00\x00\x0c\x96Underwater bombs. Consumes " MSG_COLOR(MSG_COLOR_PURPLE) "1/3 " MSG_COLOR(MSG_COLOR_UNK) "off meter.\n" MSG_BULLET "Lift one with \x1a\x05\x00\x00\x10 or \x1a\x05\x00\x00\x0f and press \x1a\x05\x00\x00\x0a\n\x1a\x05\x06\x00\x0bto set it down. Press \x1a\x05\x00\x00\x0a while\n\x1a\x05\x06\x00\x0brunning to throw.\n" MSG_BULLET "Set the " MSG_COLOR(MSG_COLOR_RED) "Hero's Bow" MSG_COLOR(MSG_COLOR_UNK) " and use \x1a\x05\x00\x00\x0e on the\n\x1a\x05\x06\x00\x0bItems screen to make " MSG_COLOR(MSG_COLOR_RED) "bomb arrows" MSG_COLOR(MSG_COLOR_UNK) ".\n";
                else if (msgID == MBombDescription) newMessage = "\x1a\x09\x03\x00\x00\x00\x00\x0c\x97Mobile explosives. Consumes " MSG_COLOR(MSG_COLOR_PURPLE) "1/3 " MSG_COLOR(MSG_COLOR_UNK) "off meter.\n" MSG_BULLET "Lift one with \x1a\x05\x00\x00\x10 or \x1a\x05\x00\x00\x0f and press \x1a\x05\x00\x00\x0a\n\x1a\x05\x06\x00\x0bto set it down. Press \x1a\x05\x00\x00\x0a while\n\x1a\x05\x06\x00\x0brunning to throw.\n" MSG_BULLET "Set the " MSG_COLOR(MSG_COLOR_RED) "Hero's Bow" MSG_COLOR(MSG_COLOR_UNK) " and use \x1a\x05\x00\x00\x0e on the\n\x1a\x05\x06\x00\x0bItems screen to make " MSG_COLOR(MSG_COLOR_RED) "bomb arrows" MSG_COLOR(MSG_COLOR_UNK) ".\n";
                setMessageText(newMessage);
                return;
            }
        } else
        {
            auto checkForSpecificMsg =
                [=](uint32_t desiredMsgId, int32_t room, const char* stage, const void* currentInf1, const char* desiredFile)
            {
                // Check if the message ids are the same
                if (msgID != desiredMsgId)
                {
                    return false;
                }

                // Check if the stage and room are correct
                if (!libtp::tools::playerIsInRoomStage(room, stage))
                {
                    return false;
                }

                // Check if the desired file is being used
                return currentInf1 == getInf1Ptr(desiredFile);
            };

            constexpr uint32_t itemRevival = 0x658;
            if (checkForSpecificMsg(itemRevival, 1, "F_SP103", currentInf1, "zel_01.bmg")) {
                //const char* newMessage;
                //newMessage = "Would you like to " MSG_COLOR(MSG_COLOR_RED) "rent " MSG_COLOR(MSG_COLOR_WHITE) "an\nitem? If you die, \nyou'll lose all of them.";
                libtp::display::setConsoleColor(0, 0, 0, 180);
                // mute the buttons
                //libtp::patch::hookFunction(libtp::tp::d_menu_window::dMw_A_TRIGGER, downNope); libtp::patch::hookFunction(libtp::tp::d_menu_window::dMw_B_TRIGGER, downNope); libtp::patch::hookFunction(libtp::tp::d_menu_window::dMw_START_TRIGGER, downNope);
                for (uint8_t itm = 0x40; itm < 0x73; itm++) {
                    if (!libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(itm)) {
                        if (itm == 0x40) itemshopping[1] = true;
                        else if (itm == 0x41) itemshopping[8] = true;
                        else if (itm == 0x42) itemshopping[10] = true;
                        else if (itm == 0x43) itemshopping[2] = true;
                        else if (itm == 0x44) {itemshopping[6] = true; itm++;}
                        else if (itm == 0x46) itemshopping[9] = true;
                        else if (itm == 0x47) {itemshopping[7] = true; itm += 3;}
                        else if (itm == 0x4b) {itemshopping[0] = true; itm += 0x24;}
                        else if (itm == 0x70) itemshopping[3] = true;
                        else if (itm == 0x71) itemshopping[4] = true;
                        else if (itm == 0x72) itemshopping[5] = true;
                    }
                } java = 7; sysConsolePtr->consoleLine[6].showLine = true;
                if (!libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x30)) itemshopping[11] = true;
                for (int python = 0; python < 12; python++) {
                    if (itemshopping[python] == true) {strcpy( sysConsolePtr->consoleLine[java].line, shopSelections[python]); java++;}
                    //if (python == 11) nowLine = java;
                }
                if (java < 19)
                    for (uint8_t varThatExist = java; varThatExist < 19; varThatExist++) strcpy(sysConsolePtr->consoleLine[varThatExist].line, "");
                //sysConsolePtr->consoleLine[java].showLine = false;
                strcpy(bufferOrSmthgIdkIPreferPythonForStringsTbh, sysConsolePtr->consoleLine[7].line);
                sprintf(sysConsolePtr->consoleLine[7].line, "-> %s", bufferOrSmthgIdkIPreferPythonForStringsTbh);
                for (uint8_t pricePrinter = 0; pricePrinter < 12; pricePrinter++) {
                    if (strcmp(bufferOrSmthgIdkIPreferPythonForStringsTbh, shopSelections[pricePrinter]) == 0) sprintf(sysConsolePtr->consoleLine[6].line, "            Price: %d", rentingPrice[pricePrinter]);
                }

                nowLine = 7;
                rentingTime = true;
                //setMessageText(newMessage);
                //return;
            }
        }
    }

    bool arrowMade = false;

    void Mod::gotIt(uint8_t items) {
        if (items >= 1 && items < 8 && Armor) {
            return;
        }
        else if (items == 0x12 || items == 0x11) libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount += 5450;
        else if ((items > 0x9 && items < 0x11) || (items > 0x15 && items < 0x1e)) libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount += 10900;

        else if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "F_SP103") != 0) {
            if (items == 0x30) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 500;
            else if (items == 0x40 || items == 0x44 || items == 0x71)libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 100;
            else if (items == 0x41) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 40;
            else if (items == 0x42 || items == 0x47) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 200;
            else if (items == 0x43) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 30;
            else if (items == 0x46) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 20;
            else if (items == 0x4b) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 10;
            else if (items == 0x70 || items == 0x72) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees += 50;
            else return gotAnItem(items);
        }
        else return gotAnItem(items);
    }

    int32_t oilMaxVar = 10900;

    void Mod::mstrDemoSword(libtp::tp::d_a_alink::daAlink* linkActrPtr, void* param_0, uint16_t param_1) {
        if (mstrSwrdPlld == false && libtp::tp::d_save::isCollect(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_collect, 1, 0x1) && oilMaxVar < 21800) {
            if  (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount < 10900) libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount = 10900;
            if (delayingTheMoment < 40) delayingTheMoment++;
            else {
                oilMaxVar += 100; libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount += 100;
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentLanternOil = oilMaxVar;
                //libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.maxLanternOil -= 50;
                libtp::tp::m_Do_Audio::mDoAud_seStartLevel(0x22, NULL, 0, 0);
                if (oilMaxVar >= 21800) {
                    oilMaxVar = 21800;
                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.maxLanternOil = 21800;
                    mstrSwrdPlld = true;
                }
            }
        }
        return demoMstrSwrdChek(linkActrPtr, param_0, param_1);
    }

    int32_t rodOn(libtp::tp::d_a_alink::daAlink* linkActrPtr)
    {
        return (libtp::tp::d_save::isEventBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.mEvent, 0x2580) && !(linkActrPtr->mProcID == 0xFF && linkActrPtr->mProcVar3.field_0x300e.x != 0));
    }

    void silentAmmo2() {

    }

    bool bossOver = false;

    void warpOutOfIt() {
        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mDemo.mDemoType = 5;
        libtp::tp::d_a_alink::commonProcInit(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, libtp::tp::d_a_alink::PROC_DUNGEON_WARP);
        libtp::tp::d_a_alink::procCoWarp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer);
        bossOver = true;
#ifdef PLATFORM_WII
        libtp::patch::writeBranchBL(ammoForgor + 0x170, ButtonAlphaThing);
#else
        libtp::patch::writeBranchBL(ammoSilence + 0x148, ButtonAlphaThing);
#endif
    }

    void Mod::init()
    {
        libtp::display::setConsoleColor(200, 21, 148, 0);
        gMod = this;
        //uint32_t kanteraYes = reinterpret_cast<uint32_t>(libtp::tp::d_meter2::alphaAnimeKantera);
        uint32_t ammoSilence2 = reinterpret_cast<uint32_t>(libtp::tp::d_menu_ring::drawItem);
        uint32_t ammoSilence3 = reinterpret_cast<uint32_t>(libtp::tp::d_menu_ring::drawItem2);
        uint32_t kanterapoggy = reinterpret_cast<uint32_t>(libtp::tp::d_meter2_draw::drawKanteraScreen);
        uint32_t armorSilence = reinterpret_cast<uint32_t>(libtp::tp::d_a_alink::execute_);
        uint32_t armorSilence2 = reinterpret_cast<uint32_t>(libtp::tp::d_a_alink::setDamagePoint);
        //uint32_t armorSilence3 = reinterpret_cast<uint32_t>(libtp::tp::d_a_alink::changeLink);
        #ifdef PLATFORM_WII
        libtp::patch::writeBranchBL(kanterapoggy + 0xDC, colorOilMeter);
        libtp::patch::writeBranchBL(ammoSilence + 0x3E0, silentAmmo);               //USA 1.2
        /*libtp::patch::writeBranchBL(ammoSilence + 0x3DC, silentAmmo);*/           //USA 1.0
        libtp::patch::writeBranchBL(ammoSilence + 0x3F4, silentAmmo);               //USA 1.2
        /*libtp::patch::writeBranchBL(ammoSilence + 0x3F0, silentAmmo);*/           //USA 1.0
        libtp::patch::writeBranchBL(ammoSilence2 + 0x39C, silentAmmo2);
        libtp::patch::writeBranchBL(ammoSilence3 + 0x280, silentAmmo2);
        libtp::patch::writeBranchBL(armorSilence + 0x20FC, silentArmor);
        libtp::patch::writeBranch(armorSilence2 + 0x120, armorSilence2 + 0x170);
        #else
        libtp::patch::writeBranchBL(kanterapoggy + 0xE4, colorOilMeter);
        libtp::patch::writeBranchBL(ammoSilence + 0x344, silentAmmo);
        libtp::patch::writeBranchBL(ammoSilence + 0x358, silentAmmo);
        libtp::patch::writeBranchBL(ammoSilence2 + 0x380, silentAmmo2);
        //libtp::patch::writeBranchBL(ammoSilence2 + 0x40C, silentAmmo2);
        libtp::patch::writeBranchBL(ammoSilence3 + 0x240, silentAmmo2);
        //libtp::patch::writeBranchBL(ammoSilence3 + 0x2CC, silentAmmo2);
        //libtp::patch::writeBranchBL(ammoSilence + 0x380, silentAmmo2);
        //libtp::patch::writeBranchBL(ammoSilence + 0x394, silentAmmo2);
        libtp::patch::writeBranchBL(armorSilence + 0x2034, silentArmor);
        //libtp::patch::writeBranchBL(kanteraYes + 0x40, silentAmmo2);
        //libtp::patch::writeBranchBL(armorSilence + 0x2044, silentArmor3);
        //libtp::patch::writeBranchBL(armorSilence + 0x2044, armorSilence + 0x20E0);
        libtp::patch::writeBranch(armorSilence2 + 0x100, armorSilence2 + 0x14c);
        //libtp::patch::writeBranch(armorSilence3 + 0x354, armorSilence3 + 0x3b4);
        //libtp::patch::writeBranch(armorSilence3 + 0x364, armorSilence3 + 0x3b4);
        #endif
        libtp::tp::d_meter_hio::g_drawHIO.mOxygenMeterPosY = 30.0f;
        for (uint8_t temppppp = 14; temppppp < 19; temppppp++) {
            //if (temppppp >= 48)
                //libtp::tp::d_item_data::item_resource[temppppp] = {"F_gD_rupy", 0x0004, (int16_t)0xFFFF, (int16_t)0xFFFF, 0x0007, (int16_t)0xFFFF, 0x0, 0xFF, 0x002D, 0x64, 0x0000};
            if (temppppp >= 17)
                libtp::tp::d_item_data::field_item_res[temppppp] = {"O_mD_red", 0x0003, (int16_t)0xFFFF, (int16_t)0xFFFF, (uint8_t)0xFF, (uint8_t)NULL, (uint16_t)0x1000, (int16_t)NULL};
            else libtp::tp::d_item_data::field_item_res[temppppp] = {"O_mD_gren", 0x0003, (int16_t)0xFFFF, (int16_t)0xFFFF, (uint8_t)0xFF, (uint8_t)NULL, (uint16_t)0x1000, (int16_t)NULL};
            //if (temppppp == 18) temppppp += 29; else if (temppppp == 48) temppppp += 15; else if (temppppp == 68) temppppp++; else if (temppppp == 71 || temppppp == 76) temppppp += 3;
        }
        strcpy( sysConsolePtr->consoleLine[20].line, "Navigation: Press D-Pad Up or Down");
        strcpy( sysConsolePtr->consoleLine[21].line, "Rent: Press A          Cancel: Press B");
        strcpy( sysConsolePtr->consoleLine[23].line, "youtube.com/@captainkittyca2" );
        sysConsolePtr->consoleLine[6].showLine = false;

        onUI =
            libtp::patch::hookFunction(libtp::tp::d_meter2_draw::setAlphaLifeAnimeMax ,[](libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr){ return gMod->UICheck(dMeterDrawPtr);});
        offUI =
            libtp::patch::hookFunction(libtp::tp::d_meter2_draw::setAlphaLifeAnimeMin ,[](libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr){ return gMod->UICheck2(dMeterDrawPtr);});
        makingArrow =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::makeArrow, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->arrowMaker(linkActrPtr);});
        ammoVisibility =
            libtp::patch::hookFunction(libtp::tp::d_meter2::_create, [](libtp::tp::d_meter2::dMeter2_c* dMeterPtr){ return gMod->visibleAmmo(dMeterPtr);});
        //boomLock =
            //libtp::patch::hookFunction(libtp::tp::d_a_alink::procBoomerangSubject, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->lockBoom(linkActrPtr);});
        //selectSight =
            //libtp::patch::hookFunction(libtp::tp::d_a_player::setSight, [](libtp::tp::d_a_player::daPy_sightPacket_c* linkActrPtr){ return gMod->sightSelecter(linkActrPtr);});
        boomShoot =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::throwBoomerang, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->shootBoom(linkActrPtr);});
        expiredMeter =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::checkCastleTownUseItem, [](uint16_t item){ return gMod->meterExpiration(item);});
        return_messageSmthg =
            libtp::patch::hookFunction(libtp::tp::control::setMessageCode_inSequence, [](libtp::tp::control::TControl* control, const void* TProcessor, uint16_t unk3, uint16_t msgID){ return gMod->customMessageCode(control, TProcessor, unk3, msgID);});
        copyThisDude =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::throwCopyRod, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->copySwing(linkActrPtr);});
        itemRent =
            libtp::patch::hookFunction(libtp::tp::f_ap_game::fapGm_Execute, [](){ return gMod->rentAnItem();});
        itemsGone =
            libtp::patch::hookFunction(libtp::tp::d_gameover::dispWait_init, [](libtp::tp::d_gameover::dGameOver* ptr){ return gMod->theyAreAllGone(ptr);});
        ordonNotification =
            libtp::patch::hookFunction(libtp::tp::d_gameover::saveMove_proc, [](libtp::tp::d_gameover::dGameOver* ptr){ return gMod->areYouNotified(ptr);});
        dungeonReset =
            libtp::patch::hookFunction(libtp::tp::d_gameover::deleteWait_init, [](libtp::tp::d_gameover::dGameOver* ptr){ return gMod->outsideDungeonPlayer(ptr);});
        hammerCopy =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::procCopyRodSwingInit, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->rodhammer(linkActrPtr);});
        throwIron =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::procIronBallThrowInit, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->ironThrower(linkActrPtr);});
        ironRepresentative =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::checkUpperItemActionIronBall, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->representingTheIron(linkActrPtr);});
        damageMeter =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::setDamagePoint, [](libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t a1, int32_t a2, int32_t a3, int32_t a4){ return gMod->meterDamaged(linkActrPtr, a1, a2, a3, a4);});
        initMap =
            libtp::patch::hookFunction(libtp::tp::d_stage::dStage_playerInit, [](void* stageDt, libtp::tp::d_stage::stage_dzr_header_entry* i_data, int32_t num, void* raw_data){ return gMod->mapInitialized(stageDt, i_data, num, raw_data);});
        talkCheck =
            libtp::patch::hookFunction(libtp::tp::f_op_actor_mng::fopAcM_orderTalkEvent, [](void* linkActrPtr, void* a1, uint16_t a2, uint16_t a3){ return gMod->pauseTalk(linkActrPtr, a1, a2, a3);});
        //hoookRoof =
            //libtp::patch::hookFunction(libtp::tp::d_a_alink::procHookshotRoofWait, [](libtp::tp::d_a_alink::daAlink* linkActrPtr){ return gMod->roofHooking(linkActrPtr);});
        //ringItemNumSilencer =
            //libtp::patch::hookFunction(libtp::tp::d_menu_ring::getItemNum, [](libtp::tp::d_menu_ring::dMenu_Ring_c* dMenuRing, uint8_t a1){ return gMod->ringSilencer(dMenuRing, a1);});
#ifdef PLATFORM_WII
        boomStringWii =
            libtp::patch::hookFunction(libtp::tp::d_meter2_draw::drawButtonNunZ, [](libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1){ return gMod->lockBoomWii(dMeterDrawPtr, unk1);});
#else
        boomString =
            libtp::patch::hookFunction(libtp::tp::d_meter2_draw::drawButtonR, [](libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1, uint8_t i_action, bool param_2, bool param_3){ return gMod->lockBoom(dMeterDrawPtr, unk1, i_action, param_2, param_3);});
#endif
        gotAnItem =
            libtp::patch::hookFunction(libtp::tp::d_item::execItemGet, [](uint8_t items){ return gMod->gotIt(items);});
        demoMstrSwrdChek =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::setDemoBodyBck, [](libtp::tp::d_a_alink::daAlink* linkActrPtr, void* param_0, uint16_t param_1){ return gMod->mstrDemoSword(linkActrPtr, param_0, param_1);});
        //linkChanged =
            //libtp::patch::hookFunction(libtp::tp::d_a_alink::changeLink, [](libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t a1){ return gMod->changingTheLink(linkActrPtr, a1);});
        //boomCancelled =
            //libtp::patch::hookFunction(libtp::tp::d_a_alink::checkBoomerangCarry, [](libtp::tp::d_a_alink::daAlink* linkActrPtr, libtp::tp::f_op_actor::fopAc_ac_c* i_actor){ return gMod->lockGone(linkActrPtr, i_actor);});
        targetLocklel =
            libtp::patch::hookFunction(libtp::tp::d_a_alink::LockOnTarget, [](void* attention, int32_t id){ return gMod->lockingOnTarget(attention, id);});
            libtp::patch::hookFunction(libtp::tp::d_menu_window::dMw_DOWN_TRIGGER, downNope);
        
        // Oil Meter Consumption functions
        libtp::patch::hookFunction(libtp::tp::d_a_alink::checkCopyRodTopUse, rodOn);
        libtp::patch::hookFunction(libtp::tp::d_meter2::moveKantera, meterValuee);
        libtp::patch::hookFunction(libtp::tp::d_meter2::moveArrowNum, arrowValue);
        libtp::patch::hookFunction(libtp::tp::d_meter2::moveBombNum, bombValue);
        libtp::patch::hookFunction(libtp::tp::d_meter2::movePachinkoNum, pachinkoValue);
        libtp::patch::hookFunction(libtp::tp::d_a_alink::kantera_iconDraw, silentAmmo2);
    }

    //bool heldArrowButton = false;

    //uint32_t buttonDetector = 0;


    void Mod::arrowMaker(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        if (arrowMake) 
        {
            /*arrowMade = true;
#ifndef PLATFORM_WII
            if ((libtp::tp::m_do_controller_pad::cpadInfo[0].mButtonFlags & libtp::tp::m_do_controller_pad::Button_X) == libtp::tp::m_do_controller_pad::Button_X) buttonDetector = libtp::tp::m_do_controller_pad::Button_X;
            else if ((libtp::tp::m_do_controller_pad::cpadInfo[0].mButtonFlags & libtp::tp::m_do_controller_pad::Button_Y) == libtp::tp::m_do_controller_pad::Button_Y) buttonDetector = libtp::tp::m_do_controller_pad::Button_Y;
#endif
            heldArrowButton = true;*/
            return makingArrow(linkActrPtr);
        }
    }

    /*void Mod::arrowDeleter(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        if (arrowMade && heldArrowButton) {arrowMade = false; heldArrowButton = false;} return deletingArrow(linkActrPtr);
    }*/
    int boomX = 0;
    //char lockCounter[9];
    bool lockCertification = false;
    //bool lockOut = false;
    libtp::tp::f_op_actor::fopAc_ac_c* actorSlot1 = nullptr;
    libtp::tp::f_op_actor::fopAc_ac_c* actorSlot2 = nullptr;
    libtp::tp::f_op_actor::fopAc_ac_c* actorSlot3 = nullptr;
    libtp::tp::f_op_actor::fopAc_ac_c* actorSlot4 = nullptr;
    libtp::tp::f_op_actor::fopAc_ac_c* actorSlot5 = nullptr;

    void Mod::UICheck(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr) {
        if (meterOn == false) {
            meterOn = true;
            #ifdef PLATFORM_WII
            libtp::patch::writeBranchBL(kanteraVisible + 0x130, meterVisibility);
            #else
            libtp::patch::writeBranchBL(kanteraVisible + 0x174, meterVisibility);
            #endif
        }
        //if (libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & libtp::tp::m_do_controller_pad::Button_R) {boomX++; libtp::tp::m_Do_Audio::mDoAud_seStart(0x3c, 0, 0, 0);}
        if (lockCertification/* && libtp::tp::d_a_alink::checkUpperItemActionBoomerang(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer)*/) {
#ifdef PLATFORM_WII
            if (libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x2000) boomX++;
#else
            if (libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & libtp::tp::m_do_controller_pad::Button_R) boomX++;
#endif
        }
        //sprintf(lockCounter, "boomX: %d", boomX);
        //strcpy( sysConsolePtr->consoleLine[5].line, lockCounter);
        if (boomX != 0) {
            if (!libtp::tp::d_a_alink::checkBoomerangReadyAnime(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer) && !libtp::tp::d_a_alink::checkBoomerangThrowAnime(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer)) {boomX = 0; lockCertification = false; actorSlot1 = nullptr; actorSlot2 = nullptr; actorSlot3 = nullptr; actorSlot4 = nullptr; actorSlot5 = nullptr;}
        }
        return onUI(dMeterDrawPtr);
    }

    void Mod::UICheck2(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr) {
        if (meterOn) {
            meterOn = false;
            #ifdef PLATFORM_WII
            libtp::patch::writeBranchBL(kanteraVisible + 0x130, meterVisibility);
            #else
            libtp::patch::writeBranchBL(kanteraVisible + 0x174, meterVisibility);
            #endif
        }
        return offUI(dMeterDrawPtr);
    }

    int32_t Mod::visibleAmmo(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) {

        const int32_t fun = ammoVisibility(dMeterPtr);

        if (fun) {
#ifdef PLATFORM_WII
            for (int i = 0; i < 4; i++)
#else
            for (int i = 0; i < 2; i++)
#endif
            {
                if (dMeterPtr->field_0x128 == 0) {
#ifdef PLATFORM_WII
                    if (dMeterPtr->mItemStatus[i * 4] != 0x76) libtp::tp::d_meter2_draw::drawItemNum(dMeterPtr->mpMeterDraw, i, 0.0f);
#else
                    if (dMeterPtr->mItemStatus[i * 2] != 0x76) libtp::tp::d_meter2_draw::drawItemNum(dMeterPtr->mpMeterDraw, i, 0.0f);
#endif
                }
            }
        }
        return fun;
    }

    /*void Mod::sightSelecter(libtp::tp::d_a_player::daPy_sightPacket_c* thing) {
        if (thing->mDrawFlg) {
        boomX++;
        libtp::tp::m_Do_Audio::mDoAud_seStart(0x3c, 0, 0, 0);
        }
        return selectSight(thing);
    }*/

#ifdef PLATFORM_WII
    void Mod::lockBoomWii(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1)
#else
    void Mod::lockBoom(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1, uint8_t i_action, bool param_2, bool param_3)
#endif
    {

       if (lockCertification && libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x40) lockCertification = false;
       else if (lockCertification == false && libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x40) lockCertification = true;
        /*if ((lockCertification && libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & libtp::tp::m_do_controller_pad::Button_R) || ) {
            libtp::tp::m_Do_Audio::mDoAud_seStart(0x3c, 0, 0, 0);
            boomX++;
        }*/
#ifdef PLATFORM_WII
        return boomStringWii(dMeterDrawPtr, unk1);
#else
        return boomString(dMeterDrawPtr, unk1, i_action, param_2, param_3);
#endif
    }

    void Mod::shootBoom(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        boomThrow = true;
        return boomShoot(linkActrPtr);
    }

    libtp::tp::f_op_actor::fopAc_ac_c* Mod::lockingOnTarget(void* attention, int32_t id) {

        libtp::tp::f_op_actor::fopAc_ac_c* targettttt = targetLocklel(attention, id);

        if (libtp::tp::d_a_alink::checkBoomerangReadyAnime(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer) && targettttt != nullptr && boomX < 5) {
            if (actorSlot1 == nullptr && targettttt != actorSlot2 && targettttt != actorSlot3 && targettttt != actorSlot4 && targettttt != actorSlot5) {
                //if (targettttt == actorSlot2 || targettttt == actorSlot3 || targettttt == actorSlot4 || targettttt == actorSlot5) return targettttt;
                actorSlot1 = targettttt; boomX++;
            } else if (actorSlot2 == nullptr && targettttt != actorSlot3 && targettttt != actorSlot4 && targettttt != actorSlot5 && targettttt != actorSlot1) {
                actorSlot2 = targettttt; boomX++;
            } else if (actorSlot3 == nullptr && targettttt != actorSlot4 && targettttt != actorSlot5 && targettttt != actorSlot1 && targettttt != actorSlot2) {
                actorSlot3 = targettttt; boomX++;
            } else if (actorSlot4 == nullptr && targettttt != actorSlot5 && targettttt != actorSlot1 && targettttt != actorSlot2 && targettttt != actorSlot3) {
                actorSlot4 = targettttt; boomX++;
            } else if (actorSlot5 == nullptr && targettttt != actorSlot1 && targettttt != actorSlot2 && targettttt != actorSlot3 && targettttt != actorSlot4) {
                actorSlot5 = targettttt; boomX++;
            }
        }
        return targettttt;
    }

    /*int32_t Mod::lockGone(libtp::tp::d_a_alink::daAlink* linkActrPtr, libtp::tp::f_op_actor::fopAc_ac_c* i_actor) {

        lockCertification = false;
        boomX = 0;
        return boomCancelled(linkActrPtr, i_actor);
    }*/

    void Mod::copySwing(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        if (arrowMake) boomThrow = true;
        return copyThisDude(linkActrPtr);
    }

    int32_t Mod::rodhammer(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        arrowMade = true;
        return hammerCopy(linkActrPtr);
    }

    bool bombAmmo2 = false;
    bool woah = false;

    int32_t Mod::representingTheIron(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        woah = true;
        return ironRepresentative(linkActrPtr);
    }
    
    int32_t Mod::ironThrower(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        bombAmmo2 = true;
        woah = false;
        return throwIron(linkActrPtr);
    }

    bool firstTime = true;
    char titleMap[8] = "F_SP102";
    char videoMap[8] = "S_MV000";
    uint8_t bossOver2 = 0;

    int32_t Mod::mapInitialized(void* stageDt, libtp::tp::d_stage::stage_dzr_header_entry* i_data, int32_t num, void* raw_data) {

        if (libtp::tp::d_save::getItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, 0x12, false) != 0xFF) {
            libtp::tp::d_save::setItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, 0x12, 0xFF);
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.mSave->temp_flags.mDungeonItem &= ~(uint8_t)(1 << 6);
        }
        if (firstTime && strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, titleMap) != 0 && strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, videoMap) != 0 )
        {
            for (uint8_t abcd = 0; abcd < 0x1e; abcd++) {
                if (!libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(abcd))
                    libtp::tp::d_save::onFirstBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_get_item, abcd);
            }
            strcpy(sysConsolePtr->consoleLine[10].line, libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage);
            firstTime = false;
            // check what the max meter length should be based on if you unlocked master sword
            if (libtp::tp::d_save::isCollect(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_collect, 1, 0x1) || libtp::tp::d_save::isCollect(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_collect, 1, 0x3)) {oilMaxVar = 21800; mstrSwrdPlld = true;} else oilMaxVar = 10900;
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentLanternOil = 50;
            //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount = oilMaxVar;
            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x43)) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo = 2;
            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x4B)) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo = 2;
            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x70) || libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x71) || libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x72)) {libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[0] = 2;
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[1] = 2;
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[2] = 2;}
            if (oilMaxVar == 10900) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.maxLanternOil = 21800;
        } else if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, titleMap) == 0) {firstTime = true; mstrSwrdPlld = false; bossOver2 = 0;
#ifdef PLATFORM_WII
        libtp::patch::writeBranchBL(ammoForgor + 0x170, ButtonAlphaThing);
#else
        libtp::patch::writeBranchBL(ammoSilence + 0x148, ButtonAlphaThing);
#endif
        }
        return initMap(stageDt, i_data, num, raw_data);
    }

    int32_t Mod::pauseTalk(void* actrThing1, void* actrThing2, uint16_t priority, uint16_t flag) {

        meterinc = true;
        return talkCheck(actrThing1, actrThing2, priority, flag);
    }

    /*uint8_t Mod::ringSilencer(libtp::tp::d_menu_ring::dMenu_Ring_c* thing, uint8_t a1) {
        (void)thing; (void)a1;
        //uint8_t ret = 0;
        return 0;
    }*/

    /*void Mod::changingTheLink(libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t param_0) {
        if (arrowMake) {
            libtp::tp::d_a_alink::setMagicArmorBrk(linkActrPtr, 1);
        } else libtp::tp::d_a_alink::setMagicArmorBrk(linkActrPtr, 0);
        return linkChanged(linkActrPtr, param_0);
    }*/

    /*int32_t Mod::roofHooking(libtp::tp::d_a_alink::daAlink* linkActrPtr) {
        if (arrowMake) {
            meterinc = true;
        } else libtp::tp::d_a_alink::procFallInit(linkActrPtr, 1, 1.0f);

        return hoookRoof(linkActrPtr);
    }*/

    int64_t tiiimer = 0;
    bool Mod::meterExpiration(uint16_t item) {

        if (arrowMake == false) {
            if (item == 0x40 || item == 0x41 || item == 0x42 || item == 0x44 ||
                item == 0x47 || item == 0x48 || item == 0x4b || item == 0x70 || item == 0x71 ||
                item == 0x72 || item == 0xF8)
                {
                    if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x4b) libtp::tp::d_a_alink::itemUnequip(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, item, 1.0f);
                    else if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x40) libtp::tp::d_a_alink::itemUnequip(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, item, 1.0f);
                    else if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x42) libtp::tp::d_a_alink::itemUnequip(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, item, 1.0f);
                    else if ((libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x44 || libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x47)/* && (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mItemMode == 0 || libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mItemMode == 1)*/) libtp::tp::d_a_alink::itemUnequip(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, item, 1.0f);
                    else if ((libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x48 || libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0xF8)) libtp::tp::d_a_alink::itemUnequip(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, item, 1.0f);
                    return false;
                }
        }
        if (meterinc) {meterinc = false; tiiimer = libtp::gc_wii::os_time::OSGetTime();}

        //if (preservationCommence && libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees > 0) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees = 0;

        if (arrowMake == true && preservationCommence == true) {
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees = rupeePreservation;
            rupeePreservation = 0;
            preservationCommence = false;
        }
        
        return expiredMeter(item);
    }

    const char* dungeonsExceptCastle[] = {"D_MN01", "D_MN04", "D_MN05", "D_MN06", "D_MN07", 
                                        "D_MN10", "D_MN11", "D_MN01A", "D_MN01B", "D_MN04A",
                                        "D_MN04B", "D_MN05A", "D_MN05B", "D_MN06A", "D_MN06B",
                                        "D_MN07A", "D_MN07B", "D_MN10A", "D_MN10B", "D_MN11A",
                                        "D_MN11B"};//, "D_MN08", "D_MN08A", "D_MN08B", "D_MN08C", "D_MN08D"};

    void Mod::theyAreAllGone(libtp::tp::d_gameover::dGameOver* ptr) {
        //uint8_t itmSlots = 0x0;
        //uint8_t itmIDs = 0x0;

        for (uint8_t stageWalkthrough = 0; stageWalkthrough < 21; stageWalkthrough++) {
            if (strcmp(dungeonsExceptCastle[stageWalkthrough], libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage) == 0) {altWarpVerify = true; strcpy(sysConsolePtr->consoleLine[3].line, altWarp); sysConsolePtr->consoleLine[3].showLine = false;}
        }

        if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN08") != 0 && strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN08A") != 0 && strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN08B") != 0 && strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN08C") != 0 && strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN08D") != 0) {
                // checking each item slot
                for (uint8_t itmSlots = 0x0; itmSlots < 0x18; itmSlots++) {

                // checking each item in each item slot
                for (uint8_t itmIDs = 0x40; itmIDs < 0x73; itmIDs++) {
                    if (libtp::tp::d_save::getItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, itmSlots, false) == itmIDs) {
                        libtp::tp::d_save::setItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, itmSlots, 0xFF);
                        //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemSlot = itmSlots;
                        //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemID = 0xFF;
                        libtp::tp::d_save::offFirstBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_get_item, itmIDs);
                    }
                    // jump from clawshot to rod
                    if (itmIDs == 0x44) itmIDs++;
                    // jump from double clawshot to slingshot
                    if (itmIDs == 0x47) itmIDs += 3;
                    // jump from "COPY_ROD_2" to bombs
                    else if (itmIDs == 0x4c) itmIDs += 0x23;
                }
            }
            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x30)) {
                libtp::tp::d_a_alink::setClothesChange(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, 0);
                if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x2f))
                    libtp::tp::d_com_inf_game::setSelectEquipClothes(0x2f);
                else libtp::tp::d_com_inf_game::setSelectEquipClothes(0x2e);
                libtp::tp::d_save::offFirstBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_get_item, 0x30);
            }
            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x44) || libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x47)) {
                libtp::tp::d_save::offFirstBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_get_item, 0x44);
                libtp::tp::d_save::offFirstBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_get_item, 0x47);
            }
        }
        //libtp::tp::d_save::setItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, 0x12, 0x25);
        //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mRoomNo = 22;
        return itemsGone(ptr);
    }

    void Mod::areYouNotified(libtp::tp::d_gameover::dGameOver* ptr) {
        if (altWarpVerify)
        {
            if (altWarpBlink && procAltWarpBlink == 20) {
                procAltWarpBlink = 0;
                altWarpBlink = false;
                sysConsolePtr->consoleLine[3].showLine = false;

            } else if (altWarpBlink == false && procAltWarpBlink == 10) {
                procAltWarpBlink = 0;
                altWarpBlink = true;
                sysConsolePtr->consoleLine[3].showLine = true;
            }
            procAltWarpBlink++;
        }
        return ordonNotification(ptr);
    }

    void Mod::outsideDungeonPlayer(libtp::tp::d_gameover::dGameOver* ptr) {

            if (altWarpVerify) {
                altWarpVerify = false;
                altWarpBlink = false;
                procAltWarpBlink = 0;
                sysConsolePtr->consoleLine[3].showLine = true;
                strcpy(sysConsolePtr->consoleLine[3].line, "");
                if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentForm == 1) libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentForm = 0;
                //if (stageWalkthrough >= 8) //bossOver = true;
                //else if (stageWalkthrough == 4) libtp::tp::d_stage::stageLoader()
                /*else*/ //if (stageWalkthrough < 8) {
                    //libtp::tp::d_stage::changeScene(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStageData.mStagInfo->field_0x10 >> 24, 0.0f,
                                                //5, -1, libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mBodyAngle.y, -1);
#ifdef PLATFORM_WII
                    libtp::patch::writeBranchBL(ammoForgor + 0x170, warpOutOfIt);
                    if (libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mButtonFlags & 0x4000)
#else
                    libtp::patch::writeBranchBL(ammoSilence + 0x148, warpOutOfIt);
                    if (libtp::tp::m_do_controller_pad::cpadInfo[0].mButtonFlags & 0x10)
#endif
                        bossOver2 = 1;
                    else if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, dungeonsExceptCastle[4]) == 0 || strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, dungeonsExceptCastle[15]) == 0 || strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, dungeonsExceptCastle[16]) == 0 )
                        bossOver2 = 2;
            }

        return dungeonReset(ptr);
    }

    void customDungeonThingy(uint8_t bossssover) {
        if (bossssover == 1)
        {
            strcpy(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mStage, "F_SP103");
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mRoomNo = 1;
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mPoint = 0;
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mLayer = 0xFF;
        } else if (bossssover == 2)
        {
            strcpy(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mStage, "F_SP115");
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mRoomNo = 0;
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mPoint = 40;
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mNextStage.mLayer = 0xFF;
        }
    }

    __attribute__((used, visibility("default"))) bool Mod::customMessageCode(libtp::tp::control::TControl* control, const void* TProcessor, uint16_t unk3, uint16_t msgID)
    {
        const bool ret = return_messageSmthg(control, TProcessor, unk3, msgID);

        if (ret) {
            insertCustomMessage(control, TProcessor, unk3, msgID);
        }
        return ret;
    }

    int32_t minusValue = 0;
    int bombs;
    bool startTiiimer = false;
    bool hook1Frame = false;

    //Armors Quick-Toggle variables
    bool bomba = false;
    bool bamba = false;
    uint32_t countyer = 0;
    uint32_t trimer = 0;
    uint32_t bombytimer = 0;

    void meterValuee(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) {
        bool draw_kantera;
        if (meterOn) {
            if (dMeterPtr->mNowOil > oilMaxVar) {dMeterPtr->mNowOil = oilMaxVar; libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount = oilMaxVar;}
#ifdef PLATFORM_WII
            if ((libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x31) || libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x30)) && bomba == false && (libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x00000002) && countyer < 2 && libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentForm == 0)
#else
            if ((libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x31) || libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x30)) && bomba == false && (libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & libtp::tp::m_do_controller_pad::Button_DPad_Down) && countyer < 2 && libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentForm == 0)
#endif
            {
                bamba = true;
                countyer++;
            }

            if (bamba) {
                trimer++;
                if (trimer >= 15) {
                    bamba = false;
                    bomba = true;
                    trimer = 0;
                    bombytimer = 0;
                    libtp::tp::d_a_alink::setClothesChange(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, 0);
                    libtp::tp::m_Do_Audio::mDoAud_seStart(0x4f, 0, 0, 0);
                    if (countyer == 1) {
                        if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.equipment[0] != 0x31 && libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x31)) libtp::tp::d_com_inf_game::setSelectEquipClothes(0x31);
                        else
                        {
                            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x2f))
                            libtp::tp::d_com_inf_game::setSelectEquipClothes(0x2f);
                            else libtp::tp::d_com_inf_game::setSelectEquipClothes(0x2e);
                        }
                    } else if (countyer == 2 && libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x30)) {
                        if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.equipment[0] != 0x30 && libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x30)) libtp::tp::d_com_inf_game::setSelectEquipClothes(0x30);
                        else
                        {
                            if (libtp::tp::d_com_inf_game::dComIfGs_isItemFirstBit(0x2f))
                            libtp::tp::d_com_inf_game::setSelectEquipClothes(0x2f);
                            else libtp::tp::d_com_inf_game::setSelectEquipClothes(0x2e);
                        }
                    }
                    countyer = 0;
                }
            }
            if (bomba == true) {
                bombytimer++;
                if (bombytimer >= 20) {
                    bomba = false;
                    bombytimer = 0;
                }
            }
            if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount <= oilMaxVar) {
                if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount >= 0) {
                    if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mItemMode == 3 && (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x44 || libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x47) && hook1Frame == false) {boomThrow = true; hook1Frame = true; meterinc = true;}
                    if (arrowMade)
                    {
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 5450;
                        tiiimer = libtp::gc_wii::os_time::OSGetTime();
                        startTiiimer = true;
                        arrowMade = false;
                    } else if (bombAmmo2)
                    {
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 10900;
                        tiiimer = libtp::gc_wii::os_time::OSGetTime();
                        startTiiimer = true;
                        bombAmmo2 = false;
                    } else if (boomThrow)
                    {
                        if (boomX > 0) {
                            if (boomX > 5) boomX = 5;
                            boomX = boomX * 2725;
                        } else boomX = 2725;
                        if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN07A") == 0) boomX = 2000;
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= boomX;
                        tiiimer = libtp::gc_wii::os_time::OSGetTime();
                        startTiiimer = true;
                        boomThrow = false;
                        boomX = 0;
                        lockCertification = false;
                    } else if (woah)
                    {
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 200;
                        woah = false;
                        tiiimer = libtp::gc_wii::os_time::OSGetTime();
                        startTiiimer = true;
                    } else if (Armor)
                    {
                        if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.equipment[0] != 0x30 || libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentForm != 0 || meterinc != false) Armor = false;
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 200;
                    } else if (libtp::tp::d_a_alink::checkSpinnerRide(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer))
                    {
                        spinnerSpin = *reinterpret_cast<uint32_t*>(SPINNERPAUSE);
                        if (spinnerSpin != SPINNERSPINN) {
                            if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "D_MN10A") == 0) {
                                if (libtp::tp::d_a_alink::checkSpinnerTriggerAttack(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer)) {
                                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 28;
                                }
                                libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 15;
                            } else if (strcmp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mStage, "F_SP121") == 0 && libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStartStage.mRoomNo == 12) {
                                if (libtp::tp::d_a_alink::checkSpinnerTriggerAttack(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer)) {
                                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 40;
                                }
                                else libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 45;
                            } else {
                                if (libtp::tp::d_a_alink::checkSpinnerTriggerAttack(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer)) {
                                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 50;
                                }
                                libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount -= 65;
                            }
                            tiiimer = libtp::gc_wii::os_time::OSGetTime();
                            startTiiimer = true;
                            if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount < 1) libtp::tp::d_a_alink::procSmallJumpInit(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, 2);
                        }
                    }
                }
                if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount < 0) {
                    arrowMake = false;
                    minusValue = libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount;
                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount = 0;
                    outlinePhase = false;
                    outlineR = 0;
                    outlineG = 0;
                    if (libtp::tp::d_a_alink::getCopyRodControllActor(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer) != NULL)
                        libtp::tp::d_a_alink::returnCopyRod(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer);
                    libtp::tp::d_save::offEventBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.mEvent, 0x2580);
                }
                if (startTiiimer && meterinc == false) {
                    if ((libtp::gc_wii::os_time::OSGetTime() - tiiimer) >= static_cast<int64_t>(libtp::gc_wii::os::__OSBusClock/4)) startTiiimer = false;
                }
                else if (minusValue < 0 && meterinc == false && libtp::tp::d_a_alink::getCopyRodControllActor(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer) == NULL) {
                    minusValue += 50;
                }
                else if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount < oilMaxVar && meterinc == false && libtp::tp::d_a_alink::getCopyRodControllActor(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer) == NULL) {
                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount += 50;
                    if (arrowMake == false) {
                        libtp::tp::d_save::onEventBit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.mEvent, 0x2580); arrowMake = true;
                    }
                }
                if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount > oilMaxVar) {libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount = oilMaxVar;}

            }
            //if (mstrSwrdPlld == false && libtp::tp::d_save::isCollect(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_collect, 1, 0x1)) {libtp::tp::d_meter2_draw::drawKantera(dMeterPtr->mpMeterDraw, oilMaxVar, dMeterPtr->mNowOil, libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosX, libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosY); libtp::tp::d_meter2_draw::setAlphaKanteraAnimeMax(dMeterPtr->mpMeterDraw); return;}
            draw_kantera = true;
            if (hook1Frame == true && libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mItemMode != 3) hook1Frame = false;
            if ((libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x47 || libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mEquipItem == 0x44) && minusValue < 0 && (!libtp::tp::d_bg_s_acch::ChkWaterIn(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mLinkAcch) && !libtp::tp::d_bg_s_acch::ChkWaterHit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mLinkAcch) && !libtp::tp::d_bg_s_acch::ChkGroundHit(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mLinkAcch)) && libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mItemMode == 0) libtp::tp::d_a_alink::procFallInit(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, 1, 1.0f);
        }

        if (dMeterPtr->field_0x246 != dMeterPtr->mMaxLife) {
            dMeterPtr->field_0x246 = dMeterPtr->mMaxLife;
            draw_kantera = true;
        }

        if (dMeterPtr->mLanternMeterScale != libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterScale) {
            dMeterPtr->mLanternMeterScale = libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterScale;
            draw_kantera = true;
        }

        if (dMeterPtr->mLanternMeterPosX != libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosX) {
            dMeterPtr->mLanternMeterPosX = libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosX;
            draw_kantera = true;
        }

        if (dMeterPtr->mLanternMeterPosY != libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosY) {
            dMeterPtr->mLanternMeterPosY = libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosY;
            draw_kantera = true;
        }

        if (draw_kantera == true)
        {
            dMeterPtr->mNowOil = libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount;
            libtp::tp::d_meter2_draw::drawKantera(dMeterPtr->mpMeterDraw, oilMaxVar, dMeterPtr->mNowOil, libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosX, libtp::tp::d_meter_hio::g_drawHIO.mLanternMeterPosY);
        }
        libtp::tp::d_meter2::alphaAnimeKantera(dMeterPtr);
        libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentLanternOil = libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemOilCount;
    }

    void bombValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) {
        uint8_t temp_r28;
        uint8_t temp_r31;

        for (int i = 0; i < 3; i++) {
            int16_t var_r22;
            temp_r31 = libtp::tp::d_save::getItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, (uint8_t)(i + 0xF), true);
            temp_r28 = libtp::tp::d_save::getItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, (uint8_t)(i + 0xF), false);

            if (temp_r31 != 0xFF && temp_r31 != 0x50) {
                if (libtp::tp::d_menu_window_hio::getBombFlag(&libtp::tp::d_menu_window_hio::g_mwHIO) ||
                    (libtp::tp::d_meter2_info::g_meter2_info.mMiniGameItemSetFlag == 1 && i == libtp::tp::d_meter2_info::g_meter2_info.mRentalBombBag))
                {
                    if (libtp::tp::d_save::getBombNum(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max, temp_r28) != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i]) {
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] = libtp::tp::d_save::getBombNum(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max, temp_r28);
                    }

                    if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] < 0) {
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] = 0;
                    }
                }

                if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] != 0 || dMeterPtr->mBombNum[i] != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i] ||
                    dMeterPtr->mBombMax[i] != libtp::tp::d_save::getBombNum(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max, temp_r28))
                {
                    var_r22 = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i] + libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i];
                    bombAmmo2 = true;
                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] = 0;

                    if (var_r22 < 0) {
                        var_r22 = 0;
                    }

                    if (var_r22 > libtp::tp::d_save::getBombNum(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max, temp_r28)) {
                        var_r22 = libtp::tp::d_save::getBombNum(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max, temp_r28);
                    }

                    if (var_r22 == 0) {
                        if (temp_r31 == 0x59) {
                            for (int j = 0; j < 2; j++) {
                                if (i + 0xF == libtp::tp::d_save::getSelectItemIndex(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a, j) ||
                                    i + 0xF == libtp::tp::d_com_inf_game::dComIfGs_getMixItemIndex(j))
                                {
                                    libtp::tp::d_com_inf_game::dComIfGs_setMixItemIndex(j, 0xFF);
                                    libtp::tp::d_com_inf_game::dComIfGs_setSelectItemIndex(j, 4);
                                    libtp::tp::d_com_inf_game::dComIfGp_setSelectItem(j);
                                }
                            }
                        }
                        //libtp::tp::d_save::setItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, 2+ 0xF, 0x50);
                        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemSlot = i + 0xF;
                        //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemID = 0x50;
                        for (int j = 0; j < 2; j++) {
                            if (i + 0xF == libtp::tp::d_com_inf_game::dComIfGs_getSelectMixItemNoArrowIndex(j)) {
                                libtp::tp::d_com_inf_game::dComIfGp_setSelectItem(j);
                            }
                        }
                    }

                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i] = 2;
                    dMeterPtr->mBombMax[i] = libtp::tp::d_save::getBombNum(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max, temp_r28);

                    if (temp_r31 != 0x59) {
                        for (int j = 0; j < 2; j++) {
                            if (i + 0xF == libtp::tp::d_com_inf_game::dComIfGs_getSelectMixItemNoArrowIndex(j)) {
                                libtp::tp::d_meter2_draw::setItemNum(dMeterPtr->mpMeterDraw, j, libtp::tp::d_com_inf_game::dComIfGp_getSelectItemNum(j), libtp::tp::d_com_inf_game::dComIfGp_getSelectItemMaxNum(j));
                            }
                        }
                    }
                }
            } else {
                if (temp_r31 != 0x50) {
                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i] = 0;
                }

                if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] != 0) {
                    libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemBombNumCount[i] = 0;
                }
            }

            if (dMeterPtr->mBombNum[i] != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i]) {
                dMeterPtr->mBombNum[i] = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bomb_bag_ammo[i];
            }
        }

        for (int i = 0; i < 2; i++) {
            if (dMeterPtr->mItemMaxNum[i] != libtp::tp::d_save::getSelectItemIndex(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a, i)) {
                for (int j = 0; j < 3; j++) {
                    if (j + 0xF == libtp::tp::d_save::getSelectItemIndex(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a, i)) {
                        libtp::tp::d_meter2_draw::setItemNum(dMeterPtr->mpMeterDraw, i, libtp::tp::d_com_inf_game::dComIfGp_getSelectItemNum(i), libtp::tp::d_com_inf_game::dComIfGp_getSelectItemMaxNum(i));
                    }
                }

                dMeterPtr->mItemMaxNum[i] = libtp::tp::d_save::getSelectItemIndex(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a, i);
            }
        }
    }

    void arrowValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) {
        int16_t var_r6;
        int32_t var_r28;
        int32_t i;
        uint8_t var_r27;
        uint8_t var_r6_2;

        if (libtp::tp::d_menu_window_hio::getArrowFlag(&libtp::tp::d_menu_window_hio::g_mwHIO) || libtp::tp::d_meter2_info::g_meter2_info.mMiniGameItemSetFlag == 1) {
            if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0] != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo) {
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemArrowNumCount = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0];
            }

            if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemArrowNumCount < 0) {
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemArrowNumCount = 0;
            }
        }

        if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemArrowNumCount != 0 || dMeterPtr->mItemMaxNum[2] != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0] ||
            dMeterPtr->mArrowNum != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo)
        {
            var_r6 = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo + libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemArrowNumCount;
            //if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo != 2 || dMeterPtr->mArrowNum != 2) {libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo = 2; dMeterPtr->mArrowNum = 2;}
            arrowMade = true;
            libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemArrowNumCount = 0;

            if (var_r6 < 0) {
                var_r6 = 0;
            }

            if (var_r6 > libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0]) {
                var_r6 = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0];
            }

            libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo = 2;
            dMeterPtr->mItemMaxNum[2] = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0];

            if (dMeterPtr->mArrowNum < libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo) {
                dMeterPtr->mArrowNum++;
                libtp::tp::d_meter2::onArrowSoundBit(dMeterPtr, 2);

                if (libtp::tp::d_meter2::isArrowSoundBit(dMeterPtr, 2)) {
                    if (dMeterPtr->mArrowNum != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo) {
                        if (!libtp::tp::d_meter2::isArrowSoundBit(dMeterPtr, 0) && libtp::tp::d_meter2::isArrowEquip(dMeterPtr) && libtp::tp::d_meter2_draw::isButtonVisible(dMeterPtr->mpMeterDraw)) {
                            libtp::tp::d_meter2::onArrowSoundBit(dMeterPtr, 0);
                            libtp::tp::m_Do_Audio::mDoAud_seStart(0x1b, NULL, 0, 0);
                        } else {
                            libtp::tp::d_meter2::offArrowSoundBit(dMeterPtr, 0);
                        }
                    } else {
                        if (libtp::tp::d_meter2::isArrowEquip(dMeterPtr) && libtp::tp::d_meter2_draw::isButtonVisible(dMeterPtr->mpMeterDraw)) {
                            libtp::tp::m_Do_Audio::mDoAud_seStart(0x1c, NULL, 0, 0);
                        }
                        libtp::tp::d_meter2::offArrowSoundBit(dMeterPtr, 2);
                        libtp::tp::d_meter2::offArrowSoundBit(dMeterPtr, 0);
                    }
                }
            } else if (dMeterPtr->mArrowNum > libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.bow_ammo) {
                dMeterPtr->mArrowNum--;
            }

            i = 0;
            var_r28 = 0;
            for (; i < 2; i++, var_r28 += 2) {
                if (dMeterPtr->mItemStatus[var_r28] == 0x43 || dMeterPtr->mItemStatus[var_r28] == 0x53 ||
                    dMeterPtr->mItemStatus[var_r28] == 0x54 || dMeterPtr->mItemStatus[var_r28] == 0x55 ||
                    dMeterPtr->mItemStatus[var_r28] == 0x56 || dMeterPtr->mItemStatus[var_r28] == 0x5a)
                {
                libtp::tp::d_meter2_draw::setItemNum(dMeterPtr->mpMeterDraw, i, dMeterPtr->mArrowNum, libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0]);
                } else if (dMeterPtr->mItemStatus[var_r28] == 0x4b) {
                    libtp::tp::d_meter2_draw::setItemNum(dMeterPtr->mpMeterDraw, i, dMeterPtr->field_0x1e1, 50);
                } else if (dMeterPtr->mItemStatus[var_r28] == 0x59) {
                    var_r27 = libtp::tp::d_com_inf_game::dComIfGp_getSelectItemNum(i);
                    var_r6_2 = libtp::tp::d_com_inf_game::dComIfGp_getSelectItemMaxNum(i);

                    if (var_r27 > dMeterPtr->mArrowNum) {
                        var_r27 = dMeterPtr->mArrowNum;
                    }

                    if (var_r6_2 < libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0]) {
                        var_r6_2 = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_max.mItemMax[0];
                    }

                    setItemNum(dMeterPtr->mpMeterDraw, i, var_r27, var_r6_2);
                }
            }
        }
    }

    void pachinkoValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) {
        if (libtp::tp::d_menu_window_hio::getPachinkoFlag(&libtp::tp::d_menu_window_hio::g_mwHIO)) {
            if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo != 50) {
                libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemPachinkoNumCount = 50;
            }
            if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemPachinkoNumCount < 0) libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemPachinkoNumCount = 0;
        }

        if (libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemPachinkoNumCount == 0 && dMeterPtr->mItemMaxNum[3] == 50 && dMeterPtr->mPachinkoNum == libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo) return;

        int16_t slingNum = libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo + libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemPachinkoNumCount;
        arrowMade = true;
        libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mItemPachinkoNumCount = 0;
        if (slingNum < 0) slingNum = 0;
        if (50 < slingNum) slingNum = 50;
        libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo = 2;
        dMeterPtr->mItemMaxNum[3] = 50;

        if (dMeterPtr->mPachinkoNum < libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo) {
            dMeterPtr->mPachinkoNum++; libtp::tp::d_meter2::onArrowSoundBit(dMeterPtr, 2);

            if (libtp::tp::d_meter2::isArrowSoundBit(dMeterPtr, 2)) {
                if (dMeterPtr->mPachinkoNum != libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo) {
                    if (!libtp::tp::d_meter2::isArrowSoundBit(dMeterPtr, 0) && libtp::tp::d_meter2::isPachinkoEquip(dMeterPtr) && libtp::tp::d_meter2_draw::isButtonVisible(dMeterPtr->mpMeterDraw)) {
                        libtp::tp::d_meter2::onArrowSoundBit(dMeterPtr, 0);
                        libtp::tp::m_Do_Audio::mDoAud_seStart(0x1b, 0, 0, 0);
                    } else libtp::tp::d_meter2::offArrowSoundBit(dMeterPtr, 0);
                } else {
                    if (libtp::tp::d_meter2::isPachinkoEquip(dMeterPtr) && libtp::tp::d_meter2_draw::isButtonVisible(dMeterPtr->mpMeterDraw)) libtp::tp::m_Do_Audio::mDoAud_seStart(0x1c, 0, 0, 0);
                    libtp::tp::d_meter2::offArrowSoundBit(dMeterPtr, 2);
                    libtp::tp::d_meter2::offArrowSoundBit(dMeterPtr, 0);
                }
            }
        } else if (dMeterPtr->mPachinkoNum > libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item_record.slingshot_ammo) dMeterPtr->mPachinkoNum--;
        for (int i = 0; i < 2; i++) if (dMeterPtr->mItemStatus[i * 2] == 75) setItemNum(dMeterPtr->mpMeterDraw, i, dMeterPtr->mPachinkoNum, 50);
    }

    uint8_t paddingtime = 0;

    void Mod::rentAnItem() {
        //if (libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & 0x800) {
            //libtp::tp::d_save::setItem(&libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_item, 0x12, 0x25);
            //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mDemo.mDemoType = 0x2d;
            //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->field_0x32cc = 
            //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mProcVar2.field_0x300c = 0;
            //libtp::tp::d_a_player::onResetFlg0(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, 0x80000000);
            //libtp::tp::d_stage::changeScene(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStageData.mStagInfo->field_0x10 >> 24, 0.0f, 18, -1, libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mBodyAngle.y, -1);
            //libtp::tp::d_meter2_info::warpOutProc(&libtp::tp::d_meter2_info::g_meter2_info);
            //libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mProcVar3.field_0x300e.z = 1;
            //libtp::tp::d_a_alink::commonProcInit(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, libtp::tp::d_a_alink::PROC_DUNGEON_WARP);
            //libtp::tp::d_a_alink::procCoWarpInit(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer, libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mDemo.mParam0, libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mDemo.mParam1);
            //libtp::tp::d_a_alink::procCoWarp(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer);
            //bossOver = true;
        //}
        if (bossOver) {
            paddingtime++;
#ifdef PLATFORM_WII
        libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags = 0;
        libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mButtonFlags = 0;
#else
            libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags = 0;
            libtp::tp::m_do_controller_pad::cpadInfo[0].mButtonFlags = 0;
#endif
            if (paddingtime == 30) {bossOver = false; libtp::tp::d_stage::changeScene(libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mStageData.mStagInfo->field_0x10 >> 24, 0.0f, 12, -1, libtp::tp::d_com_inf_game::dComIfG_gameInfo.play.mPlayer->mBodyAngle.y, -1); customDungeonThingy(bossOver2); bossOver2 = 0; paddingtime = 0;}
        }
        if (rentingTime)
        {
#ifdef PLATFORM_WII
            if ((libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x4) && nowLine < java-1)
#else
            if ((libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & 0x4) && nowLine < java-1)
#endif
            {
                strcpy(sysConsolePtr->consoleLine[nowLine].line, bufferOrSmthgIdkIPreferPythonForStringsTbh);
                strcpy(bufferOrSmthgIdkIPreferPythonForStringsTbh, sysConsolePtr->consoleLine[nowLine+1].line);
                //strcat(sysConsolePtr->consoleLine[nowLine+1].line, "-> ", bufferOrSmthgIdkIPreferPythonForStringsTbh)
                sprintf(sysConsolePtr->consoleLine[nowLine+1].line, "-> %s", bufferOrSmthgIdkIPreferPythonForStringsTbh);
                libtp::tp::m_Do_Audio::mDoAud_seStart(0xA1, 0, 0, 0);
                for (uint8_t pricePrinter = 0; pricePrinter < 12; pricePrinter++) {
                    if (strcmp(bufferOrSmthgIdkIPreferPythonForStringsTbh, shopSelections[pricePrinter]) == 0) sprintf(sysConsolePtr->consoleLine[6].line, "            Price: %d", rentingPrice[pricePrinter]);
                }
                nowLine++;
            }
#ifdef PLATFORM_WII
            else if ((libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x8) && nowLine > 7)
#else
            else if ((libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & 0x8) && nowLine > 7)
#endif
            {
                strcpy(sysConsolePtr->consoleLine[nowLine].line, bufferOrSmthgIdkIPreferPythonForStringsTbh);
                strcpy(bufferOrSmthgIdkIPreferPythonForStringsTbh, sysConsolePtr->consoleLine[nowLine-1].line);
                sprintf(sysConsolePtr->consoleLine[nowLine-1].line, "-> %s", bufferOrSmthgIdkIPreferPythonForStringsTbh);
                libtp::tp::m_Do_Audio::mDoAud_seStart(0xA1, 0, 0, 0);
                for (uint8_t pricePrinter = 0; pricePrinter < 12; pricePrinter++) {
                    if (strcmp(bufferOrSmthgIdkIPreferPythonForStringsTbh, shopSelections[pricePrinter]) == 0) sprintf(sysConsolePtr->consoleLine[6].line, "            Price: %d", rentingPrice[pricePrinter]);
                }
                nowLine--;
            }

#ifdef PLATFORM_WII
            if ((libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x800) || (libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x400))
#else
            if ((libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & 0x100) || (libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & 0x200))
#endif
            {
#ifdef PLATFORM_WII
                if (libtp::tp::m_re_controller_pad::mReCPd::m_pad[0].mPressedButtonFlags & 0x800)
#else
                if (libtp::tp::m_do_controller_pad::cpadInfo[0].mPressedButtonFlags & 0x100)
#endif
                {
                    /*for (int somethingsomething = 0; somethingsomething < 12; somethingsomething++) {
                        if (bufferOrSmthgIdkIPreferPythonForStringsTbh == shopSelections[somethingsomething] && )
                    }*/
                    //bool acquiredItem = false;
                    bool didYouGetIt = false;
                    for (uint8_t rentAffect = 0; rentAffect < 12; rentAffect++) {
                        if (strcmp(bufferOrSmthgIdkIPreferPythonForStringsTbh, shopSelections[rentAffect]) == 0 && libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees >= rentingPrice[rentAffect])
                        {
                            didYouGetIt = true;
                            libtp::tp::m_Do_Audio::mDoAud_seStart(0x18, 0, 0, 0);
                            libtp::tp::d_item::execItemGet(itemHexValues[rentAffect]);
                            libtp::tp::d_com_inf_game::dComIfG_gameInfo.save.save_file.player.player_status_a.currentRupees -= rentingPrice[rentAffect];
                        }
                    }
                    if (didYouGetIt == false) libtp::tp::m_Do_Audio::mDoAud_seStart(0x4A, 0, 0, 0);
                }
                libtp::display::setConsoleColor(0, 0, 0, 0);
                strcpy(bufferOrSmthgIdkIPreferPythonForStringsTbh, "");
                //sysConsolePtr->consoleLine[java].showLine = true;
                sysConsolePtr->consoleLine[6].showLine = false;
                rentingTime = false;

                for (int thingything = 0; thingything < 12; thingything++) itemshopping[thingything] = false;
            }
        }
        return itemRent();
    }
}     // namespace mod