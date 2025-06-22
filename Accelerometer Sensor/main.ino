// sth is broken with the movement logic, it doesnt stop at the left side

#include "LedControl.h"
#include "Wire.h"

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ;

LedControl lc = LedControl(12, 10, 11, 1);
unsigned long delay_time=10;

const int led_pin = 8;
const int tilt_sensor_pin = 9;

int led_matrix[8][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
};

class DOT {

  private:
    int position_x;
    int position_y;
    int x_speed;
    int y_speed;

  public:
    // Constructor
    DOT(int position_x, int position_y, int x_speed, int y_speed)
      : position_x(position_x), position_y(position_y), x_speed(x_speed), y_speed(y_speed){
    }

    // Method for realistic gravity effect
    void move(){

      // Moving logic of dot
      if ((AcZ > 2) && (position_y > 0)){
        y_speed = -1;
      } else if ((AcZ < -2) && (position_y < 7)){
        y_speed = 1;
      }
      Serial.println(AcY);
      if ((AcY > 2) && (position_x > 0)){
        x_speed = -1;
      } else if ((AcY < -2) && (position_x < 7)){
        x_speed = 1;
      }

      // Stopping logic of dot
      if ((position_y == 0) && (AcZ > 2)){
        y_speed = 0;
      } else if ((position_y == 7) && (AcZ < -2)){
        y_speed = 0;
      }
      if ((position_x == 0) && (AcY > 2)){
        x_speed = 0;
      } else if ((position_x == 7) && (AcY < -2)){
        x_speed = 0;
      }

      // If out of bounds, dont move
      if((position_x + x_speed < 0) || (position_x + x_speed > 7) ||
        (position_y + y_speed < 0) || (position_y + y_speed > 7)){
        return;
      };
      // If occupied, dont move

      // Update the position and the corresponding matrix
      led_matrix[position_y][position_x] = 0;
      position_x += x_speed;
      position_y += y_speed;
      led_matrix[position_y][position_x] = 1;

    }

    // Getter methods
    int get_x() const {return position_x;}
    int get_y() const {return position_y;}
    int get_x_speed() const {return x_speed;}
    int get_y_speed() const {return y_speed;}

    // Setter methods
    void set_position(int new_position_x, int new_position_y){
      position_x = new_position_x;
      position_y = new_position_y;
    }

    void set_x_speed(int new_x_speed){
      x_speed = new_x_speed;
    }

    void set_y_speed(int new_y_speed){
      y_speed = new_y_speed;
    }

    void accelerate_speed(int acceleration_x_speed, int acceleration_y_speed){
      x_speed += acceleration_x_speed;
      y_speed += acceleration_y_speed;
    }

};

// DOT class setup with temporary values
DOT dot_1(0, 0, 0, 0);
// DOT dot_2(0, 0, 0, 0);

void setup() {

  // Serial setup
  Serial.begin(9600);

  // LC Matrix Setup
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);

  // Tiltsensor Setup
  pinMode(led_pin, OUTPUT);
  pinMode(tilt_sensor_pin, INPUT);
  digitalWrite(tilt_sensor_pin, HIGH);

  // Randomness setup
  randomSeed(analogRead(A0));

  // DOT class value setup
  dot_1 = DOT(random(0, 8), random(0, 8), 0, 0);
  led_matrix[dot_1.get_y()][dot_1.get_x()] = 1;
  /*dot_2 = DOT(random(0, 8), random(0, 8), 0, 0);
  led_matrix[dot_2.get_y()][dot_2.get_x()] = 1;*/

  // Accelerometer setup
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero, wakes up the MPU-6050
  Wire.endTransmission(true);

}


void loop() {

  Serial.print("Position: ");
  Serial.print(dot_1.get_x());
  Serial.print(" ");
  Serial.print(dot_1.get_y());
  Serial.print(" , with speed: ");
  Serial.print(dot_1.get_x_speed());
  Serial.print(" ");
  Serial.print(dot_1.get_y_speed());
  Serial.println();
  lc.setLed(0, dot_1.get_x(), dot_1.get_y(), true);
  // lc.setLed(0, dot_2.get_x(), dot_2.get_y(), true);
  

  // Accelerometer values
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = (Wire.read()<<8 | Wire.read()) / 1638.0; // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = (Wire.read()<<8 | Wire.read()) / 1638.0; // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = (Wire.read()<<8 | Wire.read()) / 1638.0; // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  
  dot_1.move();
  // dot_2.move();

  // Reset the screen
  delay(500);
  lc.clearDisplay(0);


}
