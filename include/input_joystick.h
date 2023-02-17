// Joystick related modes on the zen cape
// Including volume control, BMP adjustments and mode selection
#ifndef INPUT_JOYSTICK_H
#define INPUT_JOYSTICK_H

#define gpio_up     "/sys/class/gpio/gpio26/value"
#define gpio_down   "/sys/class/gpio/gpio46/value"
#define gpio_left   "/sys/class/gpio/gpio65/value"
#define gpio_right  "/sys/class/gpio/gpio47/value"
#define gpio_mid    "/sys/class/gpio/gpio27/value"

#define EXPORT_FILE "/sys/class/gpio/export"

#define up_dir      "/sys/class/gpio/gpio26/direction"
#define down_dir    "/sys/class/gpio/gpio46/direction"
#define left_dir    "/sys/class/gpio/gpio65/direction"
#define right_dir   "/sys/class/gpio/gpio47/direction"
#define mid_dir     "/sys/class/gpio/gpio27/direction"

// Start/Stop thread that actively listens to the input of the joystick. 
// if there is input it will respond to the established functionality
void Joystick_startListening(void);
void Joystick_stopListening(void);

//call to global stop criteria
void Joystick_quit();


#endif