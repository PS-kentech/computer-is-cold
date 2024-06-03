#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>
#include <math.h>
using namespace std;

ProcessManager::ProcessManager()
{
  this->num = 0;
}

void ProcessManager::init()
{
}

// TODO: You should implement this function if you want to change the result of the aggregation
uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
{
  uint8_t *ret, *p;
  int num, len;
  HouseData *house;
  Info *info;
  TemperatureData *tdata;
  HumidityData *hdata;
  PowerData *pdata;
  char buf[BUFLEN];
  ret = (uint8_t *)malloc(BUFLEN);
  int month, monthcycle, avg_temp, avg_humid, discomfort_index, CDD, HDD;
  time_t ts;
  struct tm *tm;

  const double Tc_target = 24.4;
  const double Th_target = 23;

  tdata = ds->getTemperatureData();
  hdata = ds->getHumidityData();
  num = ds->getNumHouseData();

  // getting the month value from the timestamp
  ts = ds->getTimestamp();
  tm = localtime(&ts);
  month = tm->tm_mon + 1;

  const double PI = 3.1415926;

  monthcycle = -cos((month-1)/12 * 2 * PI) * 1000;

  //

  avg_temp = (double) tdata->getValue();

  avg_humid = (double) hdata->getValue();

  discomfort_index = (1.8 * avg_temp - 0.55 * (1 - avg_humid) * (1.8 * avg_temp - 26) + 32) * 1000;

  //

  if (avg_temp > Tc_target){
    CDD = 1000 * (avg_temp - Tc_target);
  }
  else{
    CDD = 0;
  }

  //

  if (avg_temp < Th_target){
    HDD = 1000 * (Th_target - avg_temp);
  }
  else{
    HDD = 0;
  }

  // initializing the memory to send to the network manager
  memset(ret, 0, BUFLEN);
  *dlen = 0;
  p = ret;

  // Example) saving the values in the memory
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(monthcycle, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(discomfort_index, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(CDD, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(HDD, p);
  *dlen += 2;

  return ret;
}
