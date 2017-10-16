#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "calib_values.h"
#include "device_specific.h"

//! Holds the number of neighbors searched
unsigned char neighborsSearched;
//! The binary search step size
unsigned char calStep;
//! The lowest difference between desired and measured counter value
unsigned char bestCountDiff = 0xFF;
//! Stores the lowest difference between desired and measured counter value for the first search
unsigned char bestCountDiff_first;
//! The OSCCAL value corresponding to the bestCountDiff
unsigned char bestOSCCAL;
//! Stores the OSCCAL value corresponding to the bestCountDiff for the first search
unsigned char bestOSCCAL_first;
//! The desired counter value
uint16_t  countVal;
//! Calibration status
unsigned int calibration;
//! Stores the direction of the binary step (-1 or 1)
signed char sign;

//Functions used
void CalibrationInit(void);
void CalibrateInternalRc(void);
unsigned int Counter(void);
void BinarySearch(unsigned int ct);
void NeighborSearch(void);

/*! \brief Program entry point.
*
* Main initializes all subsystems, prepares and starts calibration
* according to the calibration method chosen, and the device specific
* oscillator characteristics. Ends in an eternal loop.
*
*/

void rc_calibration(void){
  CalibrationInit();                                            // Initiates calibration
  PREPARE_CALIBRATION();                                        // Sets initial stepsize and sets calibration state to "running"
  CalibrateInternalRc();                                        // Calibrates to selected frequency

#ifndef CALIBRATION_METHOD_SIMPLE                               // If simple search method is chosen, there is no need to do two calibrations.
#ifdef TWO_RANGES                                               // For devices with splitted OSCCAL register.
  if (bestCountDiff != 0x00)                                    // Do not do a second search if perfect match
  {
    OSCCAL = DEFAULT_OSCCAL_HIGH;                               // Sets search range to upper part of OSCCAL
    NOP();
    bestOSCCAL_first = bestOSCCAL;                              // Save OSCCAL value and count difference achieved in first calibration
    bestCountDiff_first = bestCountDiff;
    PREPARE_CALIBRATION();                                      // Search performed in lower OSCCAL range, perform search in upper OSCCAl range
    CalibrateInternalRc();                                      // Perform a second search in upper part of OSCCAL

    if (bestCountDiff > bestCountDiff_first)                    // Check which search gave the best calibration
    {
      OSCCAL = bestOSCCAL_first;                                // First calibration is more accurate and OSCCAL is written accordingly
      NOP();
    }
  }
#endif
#endif
}


/*! \brief Initializes the calibration.
*
* Computes the count value needed to compare the desired internal oscillator
* speed with the external watch crystal, and sets up the asynchronous timer.
*
*/
void CalibrationInit(void){
	countVal = ((EXTERNAL_TICKS*(uint32_t)CALIBRATION_FREQUENCY)/(((uint32_t)XTAL_FREQUENCY)*LOOP_CYCLES));
  ASSR |= (1<<ASYNC_TIMER); \
  ASYNC_TIMER_CONTROL_REGISTER = (1<<NO_PRESCALING);
  TIMER = 0;
  while (ASSR & ((1<<OUTPUT_COMPARE_UPDATE_BUSY)|(1<<TIMER_UPDATE_BUSY)|(1<<ASYNC_TIMER_CONTROL_UPDATE_BUSY)));
}

/*! \brief Calibration function
*
* Performs the calibration according to calibration method chosen.
* Compares different calibration results in order to achieve optimal results.
*
*/
void CalibrateInternalRc(void){
  unsigned int count = 0;

#ifdef CALIBRATION_METHOD_SIMPLE                                // Simple search method
  unsigned char cycles = 0x80;
 // uint32_t best_diff = 0xFFFFFFFF;
  //uint8_t best_osccal = OSCCAL;
  uint8_t prev_osccal[2] = {0,0};
  for(;cycles > 0; cycles--)
  {
	  prev_osccal[1] = prev_osccal[0];
	  prev_osccal[0] = OSCCAL;
	  //printf("OSCCAL %x \n", OSCCAL);
	  count = Counter();
	  if(cycles < 0x80-5) {
	  //printf("count %u \n", count);
	  if(count > countVal){
		  OSCCAL--;
		  NOP();
	  }else if(count < countVal){
		  OSCCAL++;
		  NOP();
	  }else{
		  cycles = 1;
	  }
	  //printf("OSCCAL %x prev0 %x prev1 %x \n", OSCCAL, prev_osccal[0], prev_osccal[1]);
	  if(OSCCAL == prev_osccal[1]) {
		  cycles = 1;
	  }
	  }
  }

/*  do{
		count = Counter();
		uint16_t diff = abs(countVal-count);
		if(diff < best_diff) {
				best_osccal = OSCCAL;
				best_diff = diff;
				if (count > countVal) {
				  OSCCAL--;  // If count is more than count value corresponding to the given frequency: decrease speed
				}else if (count < countVal){
				  OSCCAL++;// If count is less: - increase speed
				}else if (count == countVal){
				  cycles=1;
				}

				//printf("%x \n", best_osccal);
				//printf("best diff %u best osccal %x \n", best_diff, best_osccal);
		}else{
			cycles = 1;
			OSCCAL = best_osccal;
		}
		//printf("%u \n", diff);
  	 } while(--cycles);            // Calibrate using 128(0x80) calibration cycles */
//printf("best_osccal %x \n", best_osccal);
#else                                                           // Binary search with or without neighbor search
  unsigned char countDiff;
  unsigned char neighborSearchStatus = FINISHED;

  while(calibration == RUNNING){
    count = Counter();                                          // Counter returns the count value after external ticks on XTAL
    if (calStep != 0)
    {
      BinarySearch(count);                                      // Do binary search until stepsize is zero
    }
    else
    {
      if(neighborSearchStatus == RUNNING)
      {
        countDiff = ABS((signed int)count-(signed int)countVal);
        if (countDiff < bestCountDiff)                          // Store OSCCAL if higher accuracy is achieved
        {
          bestCountDiff = countDiff;
          bestOSCCAL = OSCCAL;
        }
        NeighborSearch();                                       // Do neighbor search
      }
      else                                                      // Prepare and start neighbor search
      {
#ifdef CALIBRATION_METHOD_BINARY_WITHOUT_NEIGHBOR               // No neighbor search if deselected
        calibration = FINISHED;
        countDiff = ABS((signed int)count-(signed int)countVal);
        bestCountDiff = countDiff;
        bestOSCCAL = OSCCAL;
#else
        neighborSearchStatus = RUNNING;                         // Do neighbor search by default
        neighborsSearched = 0;
        countDiff = ABS((signed int)count-(signed int)countVal);
        bestCountDiff = countDiff;
        bestOSCCAL = OSCCAL;
#endif
      }
    }
  }
#endif
}

/*! \brief The Counter function
*
* This function increments a counter for a given ammount of ticks on
* on the external watch crystal.
*
*/
unsigned int Counter(void){
  unsigned int cnt;

  cnt = 0;                                                      // Reset counter
  TIMER = 0x00;                                                 // Reset async timer/counter
  while (ASSR & ((1<<OUTPUT_COMPARE_UPDATE_BUSY)|(1<<TIMER_UPDATE_BUSY)|(1<<ASYNC_TIMER_CONTROL_UPDATE_BUSY))); // Wait until async timer is updated  (Async Status reg. busy flags).
  do{                                                           // cnt++: Increment counter - the add immediate to word (ADIW) takes 2 cycles of code.
    cnt++;                                                      // Devices with async TCNT in I/0 space use 1 cycle reading, 2 for devices with async TCNT in extended I/O space
  } while (TIMER < EXTERNAL_TICKS);                             // CPI takes 1 cycle, BRCS takes 2 cycles, resulting in: 2+1(or 2)+1+2=6(or 7) CPU cycles
  return cnt;                                                   // NB! Different compilers may give different CPU cycles!
}                                                               // Until 32.7KHz (XTAL FREQUENCY) * EXTERNAL TICKS

/*! \brief The binary search method
*
* This function uses the binary search method to find the
* correct OSSCAL value.
*
*/
void BinarySearch(unsigned int ct){

  if (ct > countVal)                                            // Check if count is larger than desired value
  {
    sign = -1;                                                  // Saves the direction
    OSCCAL -= calStep;                                          // Decrease OSCCAL if count is too high
    NOP();
  }
  else if (ct < countVal)                                       // Opposite procedure for lower value
  {
    sign = 1;
    OSCCAL += calStep;
    NOP();
  }
  else                                                          // Perfect match, OSCCAL stays unchanged
  {
    calibration = FINISHED;
  }
  calStep >>= 1;
}

/*! \brief The neighbor search method
*
* This function uses the neighbo search method to improve
* binary search result. Will always be called with a binary search
* prior to it.
*
*/
void NeighborSearch(void){

  neighborsSearched++;
  if (neighborsSearched == 4)                                   // Finish if 3 neighbors searched
  {
    OSCCAL = bestOSCCAL;
    calibration = FINISHED;
  }
  else
  {
    OSCCAL+=sign;
    NOP();
  }
}


/*! \mainpage
 * \section Intro Introduction
 * This documents data structures, functions, variables, defines, enums, and
 * typedefs in the software for application note AVR055.
 *
 * \section CI Compilation Info
 * This software was written for the IAR Embedded Workbench 4.11A.
 *
 * To make project:
 * <ol>
 * <li> Add the file calib_32kHz.c to project.
 * <li> Under processor configuration, select any device listed in the device_specific.h file.
 * <li> Memory size: Small
 * <li> Enable bit definitions in I/O include files
 * <li> High optimization on size is recommended on release target for lower code size.
 * <li> Output format: ubrof 8 (forced) for debug and intel-extended for release target.
 * </ol>
 *
 * \section DI Device Info
 * The included source code is written for all devices with with a tunable
 * internal oscillator and a timer with asynchronous operation mode. These
 * devices are listed in the device_specific.h file. \n
 * Note that when calibrating the ATCAN90 device using the STK501 top module,
 * a 32kHz crystal other than the one on the STK501 should be used in order to
 * achieve maximum accuracy. Also note that the ATmega169 and ATmega169P has
 * different oscillators, and when calibrating  the latter, modify the
 * ATmega169 device specific defines in the device_specific.h file.
 *
 * \section TDL ToDo List
 * \todo Change \ref CALIBRATION_FREQUENCY to desired frequency.
 * \todo Define either CALIBRATION_METHOD_BINARY_WITHOUT_NEIGHBOR or
 * CALIBRATION_METHOD_SIMPLE in the calib_values.h file if a other search methods than
 * the default search method with neighbor search is desired.
 * \todo If a crystal running at a different frequency than 32kHz is used,
 * modify \ref XTAL_FREQUENCY accordingly. This is not recommended.
 */
