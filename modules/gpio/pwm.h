
#define MAX_DELAY 100

typedef struct pmw_phase_t
{
    uint16_t phase;
    uint16_t default_value;
    bool down;
    uint16_t channel;
    uint8_t active_phase;
    int delay;
} pmw_phase_t;

typedef enum phase_set_params_t {
    NO_INPUT = 0,
    INPUT_COLOR,
    INPUT_SATURATION,
    INPUT_BRIGHTNESS,
    INPUT_FINISH,

    INPUT_MAX,
} phase_set_params_t;

void pwm_led_collor(void);
void pwm_led_1_green_phase_handler(void);
phase_set_params_t pwm_phase_change_choice_get();
void pwm_phase_change_choice_set(phase_set_params_t value);
void pwm_main_control_phase_choice();