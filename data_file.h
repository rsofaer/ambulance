#ifndef _HPS_AMBULANCE_DATA_FILE_H_
#define _HPS_AMBULANCE_DATA_FILE_H_
#include "ambulance_core.h"
#include <string>
#include <sstream>
#include <iosfwd>

namespace hps
{
namespace ambulance
{

/// <summary> Load data from the file with the given name. </summary>
bool LoadDataFile(const std::string& filename,
                  VictimList* victims,
                  HospitalAmbulanceList* hospitals);

/// <summary> Load data from the given stream. </summary>
bool LoadDataFile(std::ifstream& stream,
                  VictimList* victims,
                  HospitalAmbulanceList* hospitals);

}
using namespace ambulance;
}

#endif //_HPS_AMBULANCE_DATA_FILE_H_
