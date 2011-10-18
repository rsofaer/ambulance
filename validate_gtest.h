#ifndef _HPS_AMBULANCE_VALIDATE_GTEST_H_
#define _HPS_AMBULANCE_VALIDATE_GTEST_H_
#include "ambulance_core.h"
#include "process.h"
#include <fstream>
#ifdef WIN32
#include <time.h>
#elif __APPLE__
#include <time.h>
#else
#include <sys/time.h>
#endif

namespace hps
{

bool ValidateAmbulance(const VictimList& victims,
                       const HospitalList& hospitals,
                       const ActionSequenceList& actionSequences,
                       int* numRescued)
{
  // Get time for unique filename.
  std::stringstream ssTime;
  {
    time_t timeNow;
    time(&timeNow);
    ssTime << timeNow;
  }
  // Input file.
  const std::string inputFilename(ssTime.str() + std::string("_input"));
  {
    std::ofstream file(inputFilename.c_str());
    // Victims header.
    file << "person(xloc,yloc,rescuetime)" << std::endl;
    for (VictimList::const_iterator victim = victims.begin();
         victim != victims.end();
         ++victim)
    {
      file << victim->position.x << "," << victim->position.y << ","
           << victim->timeToLive << std::endl;
    }
    // Hospital header.
    file << std::endl << std::endl
         << "hospital(numambulance)" << std::endl;
    for (HospitalList::const_iterator hospital = hospitals.begin();
         hospital != hospitals.end();
         ++hospital)
    {
      file << hospital->ambulances << std::endl;
    }
    file << std::endl;
  }
  // Output file.
  const std::string outputFilename(ssTime.str() + std::string("_output"));
  {
    std::ofstream file(outputFilename.c_str());
    file << ActionSequenceListFormatter(victims, hospitals, actionSequences)
         << std::endl;
  }
  // Run the validator.
  Process process;
  std::stringstream ssValidatorCmd;
  ssValidatorCmd << "python validator.py "
                 << inputFilename << " " << outputFilename;
  process.Start(ssValidatorCmd.str());
  std::string validatorOut;
  process.ReadStdout(&validatorOut);
  process.Join();
  // Extract number from validator.
  // Format is 'Total saved: #."
  bool valid = false;
  *numRescued = -1;
  if (!validatorOut.empty())
  {
    std::stringstream ssExtractSaved(validatorOut);
    std::string token;
    std::getline(ssExtractSaved, token, ' ');
    std::getline(ssExtractSaved, token, ' ');
    std::getline(ssExtractSaved, token, ' ');
    token.erase(std::remove_if(token.begin(), token.end(),
                               std::bind2nd(std::less<char>(), '0')),
                token.end());
    token.erase(std::remove_if(token.begin(), token.end(),
                               std::bind2nd(std::greater<char>(), '9')),
                token.end());
    if (!token.empty())
    {
      std::stringstream ssRescued(token);
      ssRescued >> *numRescued;
      valid = true;
    }
  }
  // Remove temp files.
  remove(inputFilename.c_str());
  remove(outputFilename.c_str());

  return valid;
}

}

#endif _HPS_AMBULANCE_VALIDATE_GTEST_H_
