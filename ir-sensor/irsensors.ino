/*
 * an arduino sketch to interface with the ir led/sensor circuit
 * of a ball mouse and report what it finds.
 */
 
/*
 * Each consecutive pair in the array 'sensor' is the pin mumber
 * for the two pins needed to read the data from each dual ir
 * sensor. A mouse sensor for a single axis has two ir photo transistors so that
 * direction can be sensed along with position along the axis. The digital
 * wave forms from each of the two sensors are 45 degrees out of
 * phase with each other so that one changes from 0 to 1 a little
 * ahead of the other. Direction is determined by calculating which
 * of the two sensors is on the leading edge when changing from 0 to 1
 * or from 1 to 0. 
 */

#define NUM_PINS 12
#define NUM_SENSORS 6

int pin = 0; // index of the pin currently being tested.
int sensor = 0; // inoex of the sensor currently being tested. 
byte sensor_pins[NUM_PINS] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
byte pin_stat[NUM_PINS];
int sensor_pos[NUM_SENSORS];
int old_sensor_pos[NUM_SENSORS];
int loop_counter = 0;
 
void setup() {
  Serial.begin(115200);
  for ( pin = 0; pin < NUM_PINS ; pin++ ) {
    pinMode ( sensor_pins[pin], INPUT );
    // Get the initial state of each pin.
    pin_stat[pin] = digitalRead(sensor_pins[pin] );
  }
  for ( sensor = 0; sensor < NUM_SENSORS ; sensor++ ) {
    // Initialise the position associated with each sensor to zero.
    sensor_pos[sensor] = 0;
    old_sensor_pos[sensor] = 0;
  }
}
 
/*
 * Get a reading from the sensors and report it back to the
 * host via the serial line.
 */
void loop() {
  // Step through each sensor pin
  for ( int pin = 0; pin < NUM_PINS; pin = pin + 2 ) {
    // Determine the index of the current sensor
    int sensor = pin / 2;
        
    // Get the state of each pin in the curent sensor
    int stat_pin0 = digitalRead(sensor_pins[pin] );    // read the input pin
    int stat_pin1 = digitalRead(sensor_pins[pin + 1] );
     
    /* Test further if there is a change in the state of either pin in the sensor
    / and increment or decrement each sensor_pos accordingly. */
     
    if ( stat_pin0 == pin_stat[pin] && stat_pin1 != pin_stat[pin + 1] ) 
    {
      if ( stat_pin1 != stat_pin0 ) {
        sensor_pos[sensor]--;  
      }
      else {
        sensor_pos[sensor]++;
      }
      pin_stat[pin + 1] = stat_pin1;      
    } 
    else if ( ( stat_pin0 != pin_stat[pin] && stat_pin1 == pin_stat[pin + 1] ) ) 
    {
      if ( stat_pin1 != stat_pin0 ) {
        sensor_pos[sensor]++;
      }
      else {
        sensor_pos[sensor]--;
      }
      pin_stat[pin] = stat_pin0;
    }
   
    /* If there is a change in the sensor_pos, send it via the serial port */
    /* Only send the data at intervals though to cut down on timing overheads that lead to */
    /* errors both in reading the sensor and in correct transmitting the data over serial */ 
    if ( sensor_pos[sensor] != old_sensor_pos[sensor] && loop_counter > 10000 ) {
      Serial.print(sensor, DEC);
      Serial.print(" : ");
      Serial.println(sensor_pos[sensor] - old_sensor_pos[sensor], DEC);
      // Store the initial sensor_pos to compare later for changes
      old_sensor_pos[sensor] = sensor_pos[sensor];
      loop_counter = 0;
    }
    loop_counter++;
  }
}
