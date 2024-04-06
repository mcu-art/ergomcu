
#include <stdint.h>
#include <stdbool.h>
#include <single_led_error_indicator.h>
#include "single_led_error_indicator_settings.h"

volatile uint8_t current_err = SLEI_OFF;

static const uint32_t short_pulse_len = 250;
static const uint32_t medium_pulse_len = 500;
static const uint32_t long_pause_len = 2000;

volatile bool change_program = false;

void DisplayError(uint8_t err)
{
    if (current_err != err)
    {
        current_err = err;
        change_program = true;
    }
}

void _displayNoError(void)
{
    static uint32_t current_tick = 0;
    static bool is_led_on = false;

    if (change_program)
    {
        change_program = false;
        is_led_on = true;
        SL_INDICATOR_LED_ON;
        current_tick = 0;
        return;
    }
    current_tick++;
    if (current_tick >= medium_pulse_len)
    {
        current_tick = 0;
        if (is_led_on)
        {
            is_led_on = false;
            SL_INDICATOR_LED_OFF;
        }
        else
        {
            is_led_on = true;
            SL_INDICATOR_LED_ON;
        }
    }
}

void _displayErrorCode(void)
{
    static uint32_t current_tick = 0;
    static uint32_t current_pulse = 0;
    static bool is_led_on = false;

    if (change_program)
    {
        change_program = false;
        is_led_on = true;
        SL_INDICATOR_LED_ON;
        current_tick = 0;
        current_pulse = 0;
        return;
    }

    current_tick++;
    if (current_pulse < current_err)
    {
        if (current_tick >= short_pulse_len)
        {
            current_tick = 0;
            if (is_led_on)
            {
                is_led_on = false;
                SL_INDICATOR_LED_OFF;
            }
            else
            {
                is_led_on = true;
                SL_INDICATOR_LED_ON;
                current_pulse++;
                if (current_pulse >= current_err)
                {
                    // Start final pause
                    is_led_on = false;
                    SL_INDICATOR_LED_OFF;
                }
            }
        }
    }
    else
    { // Display pause
        if (current_tick >= long_pause_len)
        {
            // restart from beginning
            is_led_on = true;
            SL_INDICATOR_LED_ON;
            current_tick = 0;
            current_pulse = 0;
            return;
        }
    }
}

void SingleLedErrorIndicator_OnOneMillisecondTimer(void)
{
    switch (current_err)
    {
    case SLEI_OFF:
        SL_INDICATOR_LED_OFF;
        return;
    case SLEI_NO_ERROR:
#if defined(SL_INDICATOR_NO_ERROR_PERMANENTLY_ON)
        SL_INDICATOR_LED_ON;
#elif defined(SL_INDICATOR_NO_ERROR_PERMANENTLY_OFF)
        SL_INDICATOR_LED_OFF;
#else
        _displayNoError();
#endif
        return;
    default:
        _displayErrorCode();
    }
}
