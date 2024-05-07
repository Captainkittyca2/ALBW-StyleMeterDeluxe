/**
 * @file main.h
 * @author Captain Kitty Cat (youtube.com/@captainkittyca2)
 * @brief Read main.cpp's brief.
 * @version 1.0
 * @date 2024-05-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once
#include <display/console.h>
#include <tp/f_ap_game.h>
#include <tp/d_meter2.h>
#include <tp/d_a_alink.h>
#include <tp/control.h>
#include <tp/d_menu_ring.h>
#include <tp/d_gameover.h>
#include <tp/d_stage.h>

#include <cinttypes>

namespace mod
{
    void main();
    class Mod
    {
       public:
        Mod();
        void init();

       public:
        // Counter
        int trimer;
        // Console
        libtp::display::Console c;
        void ( *onUI)(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr) = nullptr;
        void ( *offUI)(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr) = nullptr;
        void ( *makingArrow)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        void ( *boomShoot)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        void ( *copyThisDude)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        void ( *itemRent)() = nullptr;
        void ( *itemsGone)(libtp::tp::d_gameover::dGameOver* ptr) = nullptr;
        void ( *ordonNotification)(libtp::tp::d_gameover::dGameOver* ptr) = nullptr;
        void ( *dungeonReset)(libtp::tp::d_gameover::dGameOver* ptr) = nullptr;
#ifdef PLATFORM_WII
        void ( *boomStringWii)(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1) = nullptr;
#else
        void ( *boomString)(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1, uint8_t i_action, bool param_2, bool param_3) = nullptr;
#endif
        void ( *gotAnItem)(uint8_t item) = nullptr;
        void ( *demoMstrSwrdChek)(libtp::tp::d_a_alink::daAlink* linkActrPtr, void* param_0, uint16_t param_1) = nullptr;
        //void ( *linkChanged)(libtp::tp::d_a_alink::daAlink* linkactrPtr, int32_t param_0) = nullptr;
        int32_t ( *boomCancelled)(libtp::tp::d_a_alink::daAlink* linActrPtr, libtp::tp::f_op_actor::fopAc_ac_c* i_actor) = nullptr;
        //void ( *selectSight)(libtp::tp::d_a_player::daPy_sightPacket_c* thing) = nullptr;
        int32_t ( *ammoVisibility)(libtp::tp::d_meter2::dMeter2_c* dMeterPtr) = nullptr;
        int32_t ( *messageTest)(libtp::tp::control::TControl* control, uint16_t unk3, uint16_t msgid) = nullptr;
        int32_t ( *hammerCopy)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        int32_t ( *throwIron)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        int32_t ( *ironRepresentative)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        //int32_t ( *hoookRoof)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        int32_t ( *damageMeter)(libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t a1, int32_t a2, int32_t a3, int32_t a4) = nullptr;
        int32_t ( *initMap)(void* stageDt, libtp::tp::d_stage::stage_dzr_header_entry* i_data, int32_t num, void* raw_data) = nullptr;
        int32_t ( *talkCheck)(void* actorPtr, void* actorPtr2, uint16_t priority, uint16_t flag) = nullptr;
        //int32_t ( *boomLock)(libtp::tp::d_a_alink::daAlink* linkActrPtr) = nullptr;
        //uint8_t ( *ringItemNumSilencer)(libtp::tp::d_menu_ring::dMenu_Ring_c* dMenuRing, uint8_t a1) = nullptr;
        bool ( *expiredMeter)(uint16_t item) = nullptr;
        bool ( *return_messageSmthg)(libtp::tp::control::TControl* control, const void* TProcessor, uint16_t unk3, uint16_t msgID) = nullptr;
        libtp::tp::f_op_actor::fopAc_ac_c* ( *targetLocklel)(void* attention, int32_t id) = nullptr;

        /**
         * @brief checks if heart UI is on for first frame
         * 
         * @param dMeterPtr A pointer to the current dMeter2 structure.
        */
        void UICheck(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr);
        /**
         * @brief checks if heart UI is off for first frame
         * 
         * @param dMeterPtr A pointer to the current dMeter2 structure.
        */
        void UICheck2(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr);

        /**
         * @brief Alters oil meter value
         * 
         * @param dMeterPtr A pointer to the current dMeter2 structure.
        */
        //void meterValuee(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);

        /**
         * @brief Alter Arrow value
         * 
         * @param dMeterPtr A pointer to the current dMeter2 structure.
        */
        //void arrowValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);

        /**
         * @brief Alters Bomb value
         * 
         * @param dMeterPtr A pointer to the current dMeter2 structure.
        */
        //void bombValue(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);

        void arrowMaker(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        void shootBoom(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        void copySwing(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        /**
         * @brief Navigate the console to rent an item!
        */
        void rentAnItem();

        /**
         * @brief Remove all items that utilize the meter.
         * 
        */
        void theyAreAllGone(libtp::tp::d_gameover::dGameOver* ptr);

        /**
         * @brief Notify the player about warping to Ordon.
        */
        void areYouNotified(libtp::tp::d_gameover::dGameOver* ptr);

        /**
         * @brief Spawn player outside of a dungeon after a gameover.
        */
        void outsideDungeonPlayer(libtp::tp::d_gameover::dGameOver* ptr);

        /**
         * @brief makes ammo items replenish meter with some exceptions. Also used to make items that consume meter that aren't acquired from shopping turn to rupees.
        */
        void gotIt(uint8_t items);

        /**
         * @brief makes a custom animation play where you see the meter enhance.
        */
        void mstrDemoSword(libtp::tp::d_a_alink::daAlink* linkActrPtr, void* param_0, uint16_t param_1);

        int32_t lockGone(libtp::tp::d_a_alink::daAlink* linkActrPtr, libtp::tp::f_op_actor::fopAc_ac_c* i_actor);

        //void sightSelecter(libtp::tp::d_a_player::daPy_sightPacket_c* thing);

        int32_t visibleAmmo(libtp::tp::d_meter2::dMeter2_c* dMeterPtr);

        int32_t messagesmthg(libtp::tp::control::TControl* control, uint16_t unk3, uint16_t msgid);

        int32_t rodhammer(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        int32_t ironThrower(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        int32_t representingTheIron(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        int32_t meterDamaged(libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t a1, int32_t a2, int32_t a3, int32_t a4);

        /**
         * @brief Checks info when you first enter a save file.
        */
        int32_t mapInitialized(void* stageDt, libtp::tp::d_stage::stage_dzr_header_entry* i_data, int32_t num, void* raw_data);

        /**
         * @brief Puse the meter recovery if talking
        */
        int32_t pauseTalk(void* actorPtr, void* actorPtr2, uint16_t priority, uint16_t flag);
        //int32_t roofHooking(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        //int32_t lockBoom(libtp::tp::d_a_alink::daAlink* linkActrPtr);

        //uint8_t ringSilencer(libtp::tp::d_menu_ring::dMenu_Ring_c* dMenuRing, uint8_t a1);

        bool meterExpiration(uint16_t ittem);

        bool customMessageCode(libtp::tp::control::TControl* control, const void* TProcessor, uint16_t unk3, uint16_t msgID);

#ifdef PLATFORM_WII
        void lockBoomWii(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1);

#else
        void lockBoom(libtp::tp::d_meter2_draw::dMeter2Draw_c* dMeterDrawPtr, uint8_t unk1, uint8_t i_action, bool param_2, bool param_3);

#endif

        //void changingTheLink(libtp::tp::d_a_alink::daAlink* linkActrPtr, int32_t param_0);

        libtp::tp::f_op_actor::fopAc_ac_c* lockingOnTarget(void* attention, int32_t id);
    };
}     // namespace mod
