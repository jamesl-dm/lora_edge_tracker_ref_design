/*!
 * \file      main_test_tx_continuous.c
 *
 * \brief     TX continuous test implementation
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include "lr1110_tracker_board.h"
#include "lorawan_config.h"

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */
 
 /*!
 * \brief TX continuous modulated
 */
 #define TX_MODULATED true

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/*!
 * \brief Radio hardware and global parameters
 */
extern lr1110_t lr1110;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/*!
 * \brief Reset event callback
 *
 * \param [in] reset_count reset counter from the modem
 */
static void lr1110_modem_reset_event( uint16_t reset_count );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

/**
 * \brief Main application entry point.
 */
int main( void )
{
    lr1110_modem_event_t                  lr1110_modem_event;
    lr1110_modem_version_t                modem;
    lr1110_modem_response_code_t modem_response_code = LR1110_MODEM_RESPONSE_CODE_OK;

    // Init board
    hal_mcu_init( );

    hal_mcu_init_periph( );

    // Init LR1110 modem event
    lr1110_modem_event.reset = lr1110_modem_reset_event;
    lr1110_modem_board_init( &lr1110, &lr1110_modem_event );

    HAL_DBG_TRACE_MSG( "\r\n\r\n" );
    HAL_DBG_TRACE_INFO( "###### ===== LR1110 Modem TX continuous demo application ==== ######\r\n\r\n" );

    // LR1110 modem version
    lr1110_modem_get_version( &lr1110, &modem );
    HAL_DBG_TRACE_PRINTF( "LORAWAN     : %#04X\r\n", modem.lorawan );
    HAL_DBG_TRACE_PRINTF( "FIRMWARE    : %#02X\r\n", modem.firmware );
    HAL_DBG_TRACE_PRINTF( "BOOTLOADER  : %#02X\r\n\r\n", modem.bootloader );
    
    modem_response_code = lr1110_modem_set_region( &lr1110, LORAWAN_REGION_USED );

    modem_response_code = lr1110_modem_test_mode_start( &lr1110 );
    
    #if 1
    {
        //leds_blink(LED_ALL_MASK, 100, 255, true);
        leds_blink(LED_ALL_MASK, 10, 255, true);
        
        //for (int j = 0; j < 10;)
        for (int j = 0; j < 1000000;)
        {
            uint32_t frequencyMin = 915000000 + 250000;
            uint32_t frequencyMax = 928000000 - 250000;
            int8_t tx_powerMin = 12;
            int8_t tx_powerMax = 22;
            
            static uint32_t frequency = 915000000 + 500000;
            static int8_t tx_power = 12;
            
            lr1110_modem_test_tx_single(&lr1110, frequency, tx_power, LR1110_MODEM_TST_MODE_SF7, LR1110_MODEM_TST_MODE_500_KHZ, LR1110_MODEM_TST_MODE_4_5, 0);
            
            HAL_Delay(7);
            
            tx_power += 1;
            if (tx_power > tx_powerMax)
            {
                tx_power = tx_powerMin;
                
                frequency += 500000;
                if (frequency > frequencyMax)
                {
                    frequency = frequencyMin;
                    j++;
                }
            }
        }
        while (1);
    }
    #else
    {
        
        //#define TX_FREQ_HZ 902300000
        #define TX_FREQ_HZ 923000000
        #define TX_POW_DBM 18
        
        #if( TX_MODULATED )
        modem_response_code = lr1110_modem_test_tx_cont( &lr1110, TX_FREQ_HZ, TX_POW_DBM, LR1110_MODEM_TST_MODE_SF7,
                                                         LR1110_MODEM_TST_MODE_125_KHZ, LR1110_MODEM_TST_MODE_4_5, 51
                                                         );
        #else
        modem_response_code = lr1110_modem_test_tx_cw( &lr1110, TX_FREQ_HZ, TX_POW_DBM );
        #endif

        while( 1 )
        {
            lr1110_modem_event_process( &lr1110 );
        }
    }
    #endif
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

static void lr1110_modem_reset_event( uint16_t reset_count )
{
    HAL_DBG_TRACE_INFO( "###### ===== LR1110 MODEM RESET %lu ==== ######\r\n\r\n", reset_count );

    if( lr1110_modem_board_is_ready( ) == true )
    {
        // System reset
        hal_mcu_reset( );
    }
    else
    {
        lr1110_modem_board_set_ready( true );
    }
}
