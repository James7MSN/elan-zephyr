# ELAN EM32F967 GPIO Test Program

This GPIO test program for the ELAN EM32F967 microcontroller tests specific GPIO pins while avoiding console and button pins.

## Test Pin Configuration

The program tests the following GPIO pins:

### Test Pins (Configured as Outputs):
- **PORTA**: PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15 (8 pins)
- **PORTB**: PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB12, PB13, PB14, PB15 (15 pins)

### Excluded Pins (Not Tested):
- **PA1, PA2**: Console UART pins (TX/RX)
- **PA6**: SW0 button input (used for test switching)
- **PB11**: SW1 button input 
- **PA0, PA3-PA7**: Not tested to avoid conflicts

## Features

The program includes 4 different test functions that can be switched using the SW0 button:

1. **Read All GPIO Values** - Reads and displays all GPIO values in binary format, highlighting test pin values
2. **Write All High** - Sets all test pins to HIGH (logic 1)
3. **Write All Low** - Sets all test pins to LOW (logic 0)  
4. **Toggle Pins** - Continuously toggles all test pins every 2 seconds

## Hardware Requirements

- ELAN EM32F967 Development Board
- SW0 button (PA6) for test switching
- Test pins PA8-PA15 and PB0-PB15 (excluding PB11) available for testing
- UART console for output display (PA1/PA2)

## Building and Running

1. Navigate to the project directory:
   ```bash
   cd samples/elan_gpio
   ```

2. Build the project:
   ```bash
   west build -b 32f967_dv
   ```

3. Flash to the board:
   ```bash
   west flash
   ```

4. Connect to the UART console to view output and press SW0 to switch between tests.

## Usage

1. The program starts with Test 1 (Read All GPIO Values)
2. Press SW0 to switch to the next test
3. Each test displays its current operation on the console
4. Tests cycle from 1 to 4 and then back to 1
5. Monitor test pins with oscilloscope, logic analyzer, or LEDs to observe the patterns

## Test Details

### Test 1: Read All GPIO Values
- Reads current state of all GPIO pins
- Displays full port values in binary and hexadecimal format
- Shows specific test pin values separately
- Useful for checking external signal inputs

### Test 2: Write All High
- Sets all test pins to logic HIGH
- PA8-PA15: 0xFF (8 pins)
- PB0-PB15: 0xF7FF (15 pins, excluding PB11)
- Verifies basic GPIO output functionality

### Test 3: Write All Low  
- Sets all test pins to logic LOW
- All test pins: 0x00
- Verifies GPIO clear functionality

### Test 4: Toggle Pins
- Continuously toggles all test pins every 2 seconds
- Creates a square wave on all test GPIO pins
- Useful for frequency and timing verification
- Shows toggle count on console

## Console Output Example

```
ELAN EM32F967 GPIO Test Program
===============================
Test pins: PA8-PA15, PB0-PB15 (excluding PB11)
Avoids: PA1,PA2 (console), PA6 (SW0), PB11 (SW1)

GPIO devices ready: GPIOA and GPIOB
Button configured at gpioa pin 6
Configuring test GPIO pins as outputs...
Test GPIO pins configured successfully
Test pins: PA8-PA15, PB0-PB10, PB12-PB15

Available Tests:
1. Read All GPIO Values
2. Write All High
3. Write All Low
4. Toggle Pins

Press SW0 to switch between tests
Starting with Test 1...

=== GPIO READ TEST ===
PORTA (PA15-PA0): 0000000001000000 (0x0040)
PORTB (PB15-PB0): 0000100000000000 (0x0800)
Test pins PA8-PA15: 00000000 (0x00)
Test pins PB0-PB15: 0000100000000000 (0x0800) [excluding PB11]
Press SW0 for next test
```

## Troubleshooting

- Ensure GPIO driver is enabled in `prj.conf`
- Verify board device tree configuration  
- Check button connections and pull-up resistors
- Monitor console output for error messages
- Use logic analyzer to verify GPIO signal integrity
- Ensure test pins are not connected to conflicting circuits
