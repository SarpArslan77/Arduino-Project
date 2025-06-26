// create an array append method and add the physics_applied boolean according to gravity & randomness

// Hardware Configuration Constants
#define ACCEL_THRESHOLD 2  // Minimum acceleration value to register tilt
#define MATRIX_MIN 0       // Minimum LED matrix coordinate
#define MATRIX_MAX 7       // Maximum LED matrix coordinate
#define MATRIX_WIDTH 8   // or your actual matrix width
#define MATRIX_HEIGHT 8  // or your actual matrix height

#include "LedControl.h"
#include "Wire.h"

// MPU-6050 Accelerometer Configuration
const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ;     // Accelerometer readings

// LED Matrix Configuration
LedControl lc = LedControl(12, 10, 11, 1); // DIN, CLK, CS, # of devices
unsigned long delay_time = 10;             // Delay between updates

// LED Matrix State (8x8 grid)
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

// Coordinate structure for 2D positions
struct Coordinate {
  byte x;  // Horizontal position (0-7)
  byte y;  // Vertical position (0-7)
};

// Grid dimensions
const int GRID_SIZE = 8;                 // 8x8 grid
const int TOTAL_COORDS = GRID_SIZE * GRID_SIZE; // Total positions (64)

/**
 * @brief Generates N unique random coordinates within the 8x8 grid
 * 
 * Uses Fisher-Yates shuffle algorithm to ensure unique positions
 * 
 * @param N Number of unique coordinates to generate (1-64)
 * @param result[] Array to store generated coordinates
 * @return true if generation succeeded, false if N is invalid
 */
bool generate_random_coordinates(int N, Coordinate result[]) {
  // Validate input range
  if (N <= 0 || N > TOTAL_COORDS) {
    Serial.println("Error: N must be between 1 and 64.");
    return false;
  }

  // Create list of all possible coordinates
  Coordinate allCoords[TOTAL_COORDS];
  int index = 0;
  for (byte y = 0; y < GRID_SIZE; y++) {
    for (byte x = 0; x < GRID_SIZE; x++) {
      allCoords[index] = {x, y};
      index++;
    }
  }

  // Fisher-Yates shuffle algorithm
  for (int i = TOTAL_COORDS - 1; i > 0; i--) {
    int j = random(i + 1);  // Random index from 0 to i
    // Swap elements
    Coordinate temp = allCoords[i];
    allCoords[i] = allCoords[j];
    allCoords[j] = temp;
  }

  // Copy first N elements to result
  for (int i = 0; i < N; i++) {
    result[i] = allCoords[i];
  }

  return true;
}

/**
 * Randomly selects one valid coordinate from a 2-element array
 * Handles cases where one or both coordinates might be invalid
 * 
 * @param array The input array of 2 Coordinate elements
 * @param invalidValue The value that represents an invalid/empty coordinate (default {255,255})
 * @return The selected valid coordinate, or invalidValue if both are invalid
 */
Coordinate selectRandomCoordinate(Coordinate array[2], Coordinate invalidValue = {255, 255}) {
  // Count valid coordinates
  int validCount = 0;
  int validIndices[2]; // Stores indices of valid coordinates
  
  for (int i = 0; i < 2; i++) {
    // Check if coordinate is valid (not equal to invalidValue)
    if (!(array[i].x == invalidValue.x && array[i].y == invalidValue.y)) {
      validIndices[validCount] = i;
      validCount++;
    }
  }
  
  // Handle cases based on how many valid coordinates we found
  if (validCount == 0) {
    return invalidValue; // Both coordinates are invalid
  } 
  else if (validCount == 1) {
    return array[validIndices[0]]; // Only one valid coordinate
  } 
  else {
    // Both coordinates are valid, choose randomly
    int randomIndex = random(2); // Generates 0 or 1
    return array[randomIndex];
  }
}

/**
 * @brief DOT class represents a single moving dot on the LED matrix
 * 
 * Handles position, movement, and physics of individual dots
 */
class DOT {
  private:
    int position_x;  // Current X position
    int position_y;  // Current Y position
    int x_speed;     // Horizontal speed
    int y_speed;     // Vertical speed

  public:
    // Constructor - initializes position and speed
    DOT(int position_x, int position_y, int x_speed, int y_speed)
      : position_x(position_x), position_y(position_y), 
        x_speed(x_speed), y_speed(y_speed) {}

    /**
     * @brief Updates dot position based on accelerometer tilt
     * 
     * Implements gravity-like movement with collision detection
     */
    void fall() {
      // Calculate intended movement based on tilt
      int dx = 0, dy = 0;
      
      // Vertical movement (Z-axis tilt)
      if (AcZ > ACCEL_THRESHOLD) {
        dy = -1;  // Move down
      } else if (AcZ < -ACCEL_THRESHOLD) {
        dy = 1;   // Move up
      }
      
      // Horizontal movement (Y-axis tilt)
      if (AcY > ACCEL_THRESHOLD) {
        dx = -1;  // Move left
      } else if (AcY < -ACCEL_THRESHOLD) {
        dx = 1;   // Move right
      }

      if (led_matrix[position_y + dy][position_x + dx] == 1) {
        Coordinate possible_coordinates[2] = {{255, 255}, {255, 255}}; // Initialize as invalid
        
        // Handle vertical movement (up/down)
        if (dy != 0 && dx == 0) {
          if (position_x + 1 < MATRIX_WIDTH && !led_matrix[position_y + dy][position_x + 1]) {
            possible_coordinates[0] = {position_x + 1, position_y + dy};
          }
          if (position_x - 1 >= 0 && !led_matrix[position_y + dy][position_x - 1]) {
            possible_coordinates[1] = {position_x - 1, position_y + dy};
          }
          
          Coordinate selected = selectRandomCoordinate(possible_coordinates);
          if (selected.x != 255) dx = selected.x - position_x;
        }
        // Handle horizontal movement (left/right)
        else if (dx != 0 && dy == 0) {
          if (position_y + 1 < MATRIX_HEIGHT && !led_matrix[position_y + 1][position_x + dx]) {
            possible_coordinates[0] = {position_x + dx, position_y + 1};
          }
          if (position_y - 1 >= 0 && !led_matrix[position_y - 1][position_x + dx]) {
            possible_coordinates[1] = {position_x + dx, position_y - 1};
          }
          
          Coordinate selected = selectRandomCoordinate(possible_coordinates);
          if (selected.y != 255) dy = selected.y - position_y;
        }
      }
      
      // No movement if no significant tilt
      if (dx == 0 && dy == 0) return;

      // Calculate potential new position
      int new_x = position_x + dx;
      int new_y = position_y + dy;

      // Boundary checking
      if (new_x < MATRIX_MIN || new_x > MATRIX_MAX || 
          new_y < MATRIX_MIN || new_y > MATRIX_MAX) {
        return;
      }

      // Collision detection (check if target position is occupied)
      if (led_matrix[new_y][new_x] == 1) {
        return;
      }

      // Execute the move
      led_matrix[position_y][position_x] = 0;  // Clear old position
      position_x = new_x;                      // Update X
      position_y = new_y;                      // Update Y
      led_matrix[position_y][position_x] = 1;  // Set new position
    }

    // Accessor methods
    int get_x() const { return position_x; }
    int get_y() const { return position_y; }
    int get_x_speed() const { return x_speed; }
    int get_y_speed() const { return y_speed; }

    // Mutator methods
    void set_position(int new_position_x, int new_position_y) {
      position_x = new_position_x;
      position_y = new_position_y;
    }

    void set_x_speed(int new_x_speed) {
      x_speed = new_x_speed;
    }

    void set_y_speed(int new_y_speed) {
      y_speed = new_y_speed;
    }

    void accelerate_speed(int acceleration_x_speed, int acceleration_y_speed) {
      x_speed += acceleration_x_speed;
      y_speed += acceleration_y_speed;
    }

    /**
     * @brief (Class Method) Creates N dot instances with unique random coordinates.
     * 
     * Populates a global array with new DOT instances.
     * 
     * @param N The number of dots to create.
     */
    static void create_dots(int N); // <<<< MODIFICATION: Just declare the method here.

    /**
     * @brief Main update function for the dot
     * 
     * Called each frame to update physics and position
     */
    void main_loop() {
      fall();
    }
};

// Constants and Global Variables for managing multiple dots
const int NUM_DOTS = 20; // Define how many dots you want to create
DOT* dots[NUM_DOTS];    // Global array to hold pointers to DOT instances

/**
 * @brief (Method Definition) Creates N dot instances.
 * 
 * This is the implementation of the static method declared inside the DOT class.
 * By defining it here, it can access the global 'dots' array declared above.
 */
void DOT::create_dots(int N) {
  // Ensure N is within a valid range
  if (N <= 0 || N > NUM_DOTS || N > TOTAL_COORDS) {
    Serial.println("Error: N is out of valid range.");
    return;
  }

  // Generate N unique coordinates
  Coordinate initial_coords[N];
  if (!generate_random_coordinates(N, initial_coords)) {
    // generate_random_coordinates already prints an error
    return;
  }

  // Create a DOT instance for each coordinate
  for (int i = 0; i < N; i++) {
    byte x = initial_coords[i].x;
    byte y = initial_coords[i].y;

    // Dynamically create a new DOT object and store its pointer in the global array
    dots[i] = new DOT(x, y, 0, 0);

    // Mark the dot's position on the global led_matrix
    led_matrix[y][x] = 1;
  }
}

/**
 * @brief Initial hardware and software setup
 */
void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // LED matrix initialization
  lc.shutdown(0, false);    // Wake up display
  lc.setIntensity(0, 8);    // Set brightness (0-15)
  lc.clearDisplay(0);       // Clear display

  // Initialize random number generator
  randomSeed(analogRead(A0));

  // Create the specified number of dots and place them on the grid
  DOT::create_dots(NUM_DOTS);

  // Accelerometer initialization
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Wake up MPU-6050
  Wire.endTransmission(true);
}

/**
 * @brief Main program loop
 */
void loop() {
  // Read accelerometer data
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // Starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 6, true);  // Request 6 registers for Accel data
  
  // Normalize accelerometer readings
  AcX = (Wire.read() << 8 | Wire.read()) / 1638.0;
  AcY = (Wire.read() << 8 | Wire.read()) / 1638.0;
  AcZ = (Wire.read() << 8 | Wire.read()) / 1638.0;
  
  // Update all dot instances by iterating through the global array
  for (int i = 0; i < NUM_DOTS; i++) {
    // The main_loop updates the dot's position in the shared led_matrix
    dots[i]->main_loop();
  }

  // After all dots have moved, render the entire led_matrix to the display
  for (int col = 0; col < GRID_SIZE; col++) {
    for (int row = 0; row < GRID_SIZE; row++) {
      lc.setLed(0, col, row, led_matrix[row][col]);
    }
  }

  // Frame delay
  delay(delay_time);
}
