#include "nrf_pwm.h"
#include "nrf_drv_pwm.h"
#include "pwm.h"
#include "gpio.h"
#include "led.h"
#include "button.h"

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_individual_t last_seq_values = { -1 };

static int phase_num = 0;

enum phase_color {
    LED_R_UP = 0,
    LED_B_UP,
    LED_G_UP,

    LED_B,
    LED_BG,
    LED_G,
    LED_GR,
    LED_RGB,
    LED_R_DOWN,
    LED_B_DOWN,
    LED_G_DOWN,

    END_PHASE,
};

enum channel_to_color {
    CHANNEL_LED_1_G = 0,
    CHANNEL_LED_2_R,
    CHANNEL_LED_2_B,
    CHANNEL_LED_2_G,

    CHANNEL_LED_MAX,
};

#define TOP_VALUE 2550

static uint16_t const              m_demo1_step = 1;
static uint16_t                    step_for_led_1_green = 1;
static uint8_t                     phase_for_collor_led;
static phase_set_params_t          phase_change_choice = NO_INPUT;
static nrf_pwm_values_individual_t m_demo1_seq_values;
static nrf_pwm_sequence_t const    m_demo1_seq =
{
    .values.p_individual = &m_demo1_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_demo1_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static pmw_phase_t phase_arr_clone[] = {
    { LED_R_UP, TOP_VALUE, false, CHANNEL_LED_2_R, false, MAX_DELAY },
    { LED_G_UP, 0, false, CHANNEL_LED_2_G, false, MAX_DELAY },
    { LED_R_DOWN, TOP_VALUE, true, CHANNEL_LED_2_R, false, MAX_DELAY },
    { LED_B_UP, 0, false, CHANNEL_LED_2_B, false, MAX_DELAY },
    { LED_G_DOWN, TOP_VALUE, true, CHANNEL_LED_2_G, false, MAX_DELAY },
    { LED_R_UP, 0, false, CHANNEL_LED_2_R, false, MAX_DELAY },
    { LED_B_DOWN, TOP_VALUE, true, CHANNEL_LED_2_B, false, MAX_DELAY },
};

void pwm_phase_change_choice_set(phase_set_params_t value)
{
    CRITICAL_REGION_ENTER();
    phase_change_choice = value;
    CRITICAL_REGION_EXIT();
}

phase_set_params_t pwm_phase_change_choice_get()
{
    CRITICAL_REGION_ENTER();
    phase_set_params_t local_phase_change_choice = phase_change_choice;
    CRITICAL_REGION_EXIT();

    return local_phase_change_choice;
}

static void pwm_choice_led_color_clone(void)
{
    uint8_t channel = 0;
    bool down = 0;
    bool next_phase = false;
    static uint16_t value = 0;
    uint8_t *local_active_phase = false;
    static int delay = 0;

    down = phase_arr_clone[phase_num].down;
    channel = phase_arr_clone[phase_num].channel;

    if (!phase_arr_clone[phase_num].active_phase)
    {
        value = phase_arr_clone[phase_num].default_value;
        delay = phase_arr_clone[phase_num].delay;
    }

    phase_arr_clone[phase_num].active_phase = true;
    local_active_phase = &phase_arr_clone[phase_num].active_phase;

    uint16_t *p_channels = (uint16_t *)&m_demo1_seq_values;

    if (down)
    {
        value -= m_demo1_step;
        if (value == 0)
        {
            value = 0;
            next_phase = true;
        }
    }
    else
    {
        value += m_demo1_step;
        if (value >= TOP_VALUE)
        {
            value = TOP_VALUE;
            next_phase = true;
        }
    }
    p_channels[channel] = value;

    if (delay)
    {
        delay--;
        return;
    }

    if (next_phase)
    {
        if (++phase_num >= 7)
        {
            phase_num = 0;
        }
        *local_active_phase = false;
    }
}

static double pwm_get_step_for_led(int min, uint16_t val_channel)
{
    return (val_channel / min);
}

static int pwm_get_min_val_collor(uint16_t *pchannels)
{
    int min = TOP_VALUE;

    for (int i = 1;i < CHANNEL_LED_MAX; i++)
    {
        if (pchannels[i] < min)
            min = pchannels[i];
    }
    return min;
}

static bool pwm_prio_led_up(uint16_t *pchannels, uint16_t *pchannels_last)
{
    bool flag = true;

    if (pchannels_last[CHANNEL_LED_2_R] >= 0 && pchannels[CHANNEL_LED_2_R] != TOP_VALUE)
    {
        pchannels[CHANNEL_LED_2_R] += m_demo1_step;
        flag = false;
    }

    if (pchannels_last[CHANNEL_LED_2_B] >= 0 && pchannels[CHANNEL_LED_2_B] != TOP_VALUE)
    {
        pchannels[CHANNEL_LED_2_B] += m_demo1_step;
        flag = false;
    }

    if (pchannels_last[CHANNEL_LED_2_G] >= 0 && pchannels[CHANNEL_LED_2_G] != TOP_VALUE)
    {
       pchannels[CHANNEL_LED_2_G] += m_demo1_step;
       flag = false;
    }

    return flag;
}

static void pwm_choice_led_brightness(void)
{
    static bool down = true;

    if (last_seq_values.channel_1 == 0 &&
        last_seq_values.channel_2 == 0 &&
        last_seq_values.channel_3 == 0)
    {
        last_seq_values.channel_1 = m_demo1_seq_values.channel_1;
        last_seq_values.channel_2 = m_demo1_seq_values.channel_2;
        last_seq_values.channel_3 = m_demo1_seq_values.channel_3;
    }
    
    if (down)
    {
        uint16_t *pchannels = (uint16_t*)&m_demo1_seq_values;
        uint16_t *arr_last_seq_values = (uint16_t*)&last_seq_values;

        for (int i = 1; i < CHANNEL_LED_MAX; i++)
        {
            int min = 0;
            double step = 0;
            min = pwm_get_min_val_collor(arr_last_seq_values);
            step = pwm_get_step_for_led(min, arr_last_seq_values[i]);
            if (step == 0)
                step = 1;

            if (((int)pchannels[i] - step) <= 0)
            {
                pchannels[i] = 0;
            }
            else
                pchannels[i] -= step;

            if ((m_demo1_seq_values.channel_1 == 0) ||
            (m_demo1_seq_values.channel_2 == 0) ||
            (m_demo1_seq_values.channel_3 == 0))
            {
                m_demo1_seq_values.channel_1 = 0;
                m_demo1_seq_values.channel_2 = 0;
                m_demo1_seq_values.channel_3 = 0;
                break;
            }
        }

        if (m_demo1_seq_values.channel_1 == 0 &&
            m_demo1_seq_values.channel_2 == 0 &&
            m_demo1_seq_values.channel_3 == 0)
        {
            down = !down;
        }
    }
    else
    {

        uint16_t *pchannels = (uint16_t*)&m_demo1_seq_values;
        uint16_t *arr_last_seq_values = (uint16_t*)&last_seq_values;

        for (int i = 1; i < CHANNEL_LED_MAX; i++)
        {
            int min = 0;
            double step = 0;
            min = pwm_get_min_val_collor(arr_last_seq_values);
            step = pwm_get_step_for_led(min, arr_last_seq_values[i]);

            if (((int)pchannels[i] + step) >= arr_last_seq_values[i])
            {
                pchannels[i] = arr_last_seq_values[i];
            }
            else
                pchannels[i] += step;
        }

        if ((m_demo1_seq_values.channel_1 >= last_seq_values.channel_1) &&
            (m_demo1_seq_values.channel_2 >= last_seq_values.channel_2) &&
            (m_demo1_seq_values.channel_3 >= last_seq_values.channel_3))
        {
            down = !down;
        }
    }
}

static void pwm_save_value_last_choice(void)
{
    last_seq_values.channel_1 = m_demo1_seq_values.channel_1;
    last_seq_values.channel_2 = m_demo1_seq_values.channel_2;
    last_seq_values.channel_3 = m_demo1_seq_values.channel_3;
}

static void pwm_choice_led_saturation(void)
{
    static bool down = true;
    uint16_t *pchannels_last = NULL;

    if (last_seq_values.channel_1 == 0 &&
        last_seq_values.channel_2 == 0 &&
        last_seq_values.channel_3 == 0)
    {

        last_seq_values.channel_1 = m_demo1_seq_values.channel_1;
        last_seq_values.channel_2 = m_demo1_seq_values.channel_2;
        last_seq_values.channel_3 = m_demo1_seq_values.channel_3;
    }

    pchannels_last = (uint16_t*)&last_seq_values;

    if (down)
    {

        (m_demo1_seq_values.channel_1 != last_seq_values.channel_1) ? (m_demo1_seq_values.channel_1 -= m_demo1_step) : (m_demo1_seq_values.channel_1 = last_seq_values.channel_1);
        (m_demo1_seq_values.channel_2 != last_seq_values.channel_2) ? (m_demo1_seq_values.channel_2 -= m_demo1_step) : (m_demo1_seq_values.channel_2 = last_seq_values.channel_2);
        (m_demo1_seq_values.channel_3 != last_seq_values.channel_3) ? (m_demo1_seq_values.channel_3 -= m_demo1_step) : (m_demo1_seq_values.channel_3 = last_seq_values.channel_3);

        if ((m_demo1_seq_values.channel_1 == last_seq_values.channel_1) &&
            (m_demo1_seq_values.channel_2 == last_seq_values.channel_2) &&
            (m_demo1_seq_values.channel_3 == last_seq_values.channel_3))
        {
            down = !down;
        }
    }
    else
    {
        bool prio_led = pwm_prio_led_up((uint16_t*)&m_demo1_seq_values, pchannels_last);

        if (prio_led)
        {
            for (int i = 0; i < CHANNEL_LED_MAX; i++)
            {
                if (pchannels_last[i] == 0)
                    pchannels_last[i] += m_demo1_step;
            }
        }

        if ((m_demo1_seq_values.channel_1 >= TOP_VALUE) &&
            (m_demo1_seq_values.channel_2 >= TOP_VALUE) &&
            (m_demo1_seq_values.channel_3 >= TOP_VALUE))
        {
            down = !down;
        }
    }
}

static void pwm_led_collor_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        pwm_led_1_green_phase_handler();

        if (nrf_gpio_pin_read(BUTTON_1) != BTN_PRESSED)
            return;

        switch (pwm_phase_change_choice_get())
        {
        case NO_INPUT:
            break;
        case INPUT_COLOR:
            pwm_choice_led_color_clone();
            break;

        case INPUT_SATURATION:
            pwm_choice_led_saturation();
            break;

        case INPUT_BRIGHTNESS:
            pwm_choice_led_brightness();
            break;

        case INPUT_FINISH:
            pwm_save_value_last_choice();
        case INPUT_MAX:
            return;
        default:
            NRF_LOG_INFO("Wrong value 'phase_change_choice'");
            break;
        }
    }
}

void pwm_led_collor(void)
{
    nrf_drv_pwm_config_t const config0 =
    {
        .output_pins =
        {
            BSP_LED_0 | NRF_DRV_PWM_PIN_INVERTED, // channel 0
            BSP_LED_1 | NRF_DRV_PWM_PIN_INVERTED, // channel 1
            BSP_LED_3 | NRF_DRV_PWM_PIN_INVERTED, // channel 2
            BSP_LED_2 | NRF_DRV_PWM_PIN_INVERTED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_2MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = TOP_VALUE,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &config0, pwm_led_collor_handler));

    m_demo1_seq_values.channel_0 = 0;
    m_demo1_seq_values.channel_1 = 255;
    m_demo1_seq_values.channel_2 = 0;
    m_demo1_seq_values.channel_3 = 100;
    phase_for_collor_led         = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_demo1_seq, 1,
                                      NRF_DRV_PWM_FLAG_LOOP);
}

static pmw_phase_t phase_arr_led_1_green[] = {
    { INPUT_COLOR, 0, false, CHANNEL_LED_1_G, false },
    { INPUT_SATURATION, 0, false, CHANNEL_LED_1_G, false },
    { INPUT_BRIGHTNESS, 0, false, CHANNEL_LED_1_G, false },
    { INPUT_FINISH, 0, false, CHANNEL_LED_1_G, false },
    { INPUT_MAX, 0, false, CHANNEL_LED_1_G, false },
};

void pwm_led_1_green_phase_handler(void)
{
    uint8_t channel = 0;
    static uint8_t down = false;
    bool next_phase = false;
    static int value = 0;
    uint8_t *local_active_phase = false;
    int local_step_for_led_1_green = 0;

    for (int i = 0; i < (sizeof(phase_arr_led_1_green) / sizeof(phase_arr_led_1_green[0])); i++)
    {
        if (pwm_phase_change_choice_get() == phase_arr_led_1_green[i].phase)
        {
            channel = phase_arr_led_1_green[i].channel;

            if (!phase_arr_led_1_green[i].active_phase)
                value = phase_arr_led_1_green[i].default_value;

            phase_arr_led_1_green[i].active_phase = true;
            local_active_phase = &phase_arr_led_1_green[i].active_phase;
            break;
        }
    }

    uint16_t *p_channels = (uint16_t *)&m_demo1_seq_values;

    switch (pwm_phase_change_choice_get())
    {
    case NO_INPUT:
        break;
    case INPUT_COLOR:
        local_step_for_led_1_green = step_for_led_1_green;
        if (value >= TOP_VALUE || value <= 0)
            down = !down;
        break;

    case INPUT_SATURATION:
        local_step_for_led_1_green = step_for_led_1_green *5;
        if (value >= TOP_VALUE || value <= 0)
            down = !down;
        break;

    case INPUT_BRIGHTNESS:
        value = TOP_VALUE;
        break;

    case INPUT_FINISH:
        value = 0;
        break;

    case INPUT_MAX:
        return;
    }

    if (down)
    {
        value -= local_step_for_led_1_green;
        if (value == 0)
        {
            value = 0;
            down = !down;
        }
    }
    else
    {
        value += local_step_for_led_1_green;
        if (value == 0)
        {
            value = 0;
            down = !down;
        }
    }
    p_channels[channel] = value;
    


    if (next_phase)
    {
        *local_active_phase = false;
    }
}

void pwm_main_control_phase_choice()
{
    phase_set_params_t local_phase_change_choice = pwm_phase_change_choice_get();

    if (INPUT_FINISH == local_phase_change_choice)
    {
        local_phase_change_choice = NO_INPUT;
        nrfx_pwm_stop(&m_pwm0, true);
        nrfx_pwm_uninit(&m_pwm0);

        pwm_led_collor();
    }

    pwm_phase_change_choice_set(++local_phase_change_choice);

    switch (pwm_phase_change_choice_get())
    {
    case NO_INPUT:
        break;
    case INPUT_COLOR:
        for (int i = 0; i < sizeof(phase_arr_clone)/sizeof(phase_arr_clone[0]); i++)
        {
            phase_arr_clone[i].active_phase = false;
        }
        phase_num = 0;
        m_demo1_seq_values.channel_1 = TOP_VALUE;
        m_demo1_seq_values.channel_2 = 0;
        m_demo1_seq_values.channel_3 = 0;
        break;

    case INPUT_SATURATION:
        break;

    case INPUT_BRIGHTNESS:
        break;

    case INPUT_FINISH:
        break;

    case INPUT_MAX:
        return;
    }

    memset(&last_seq_values, 0, sizeof(last_seq_values));
}