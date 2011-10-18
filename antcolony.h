#ifndef _ANT_COLONY_H_
#define _ANT_COLONY_H_
#include "ambulance_core.h"
#include "edgematrix.h"
#include <vector>
#include "graph.h"

namespace hps
{
namespace ambulance
{

typedef ActionSequenceList Path;

struct AntColony {
  AntColony(const VictimList& victims_, const HospitalList& hospitals_,
                  Path* actionSequences)
  : victims(victims_),
    hospitals(hospitals_),
    bestPath(actionSequences),
    maxIterations(1000),
    antSquadSize(4),
    explorationPreference(3),
    bestPathReward(10),
    evaporationAmount(1),
    pheremoneMatrix(NULL)
  {};

  ~AntColony()
  {};

  static int numRescued(Path* path){
    int result = 0;
    for(unsigned int i = 0; i < path->size(); i++)
    {
      ActionSequence sequence = (*path)[i];
      for(unsigned int j = 0; j < sequence.size(); j++)
      {
        ActionNode node = sequence[j];
        if(node.stopType == ActionNode::StopType_Victim)
          result++;
      }
    }
    return result;
  }

  Path runAnt(){
    //Run an ant on the graph and return the ant's path for examination.
    return *bestPath;
  }
  void adjustPheremones(Path path, double amount)
  {
    // Adjust the pheremone count on every edge in the path.
    // Should be parallelizable
  }

  void decreasePheremones(Path path)
  {
    adjustPheremones(path, explorationPreference);
  }
  void increasePheremones(Path path)
  {
    adjustPheremones(path, bestPathReward);
  }

  void evaporatePheremones(){
    // reduce the pheremone count on every edge by evaporationAmount
    // should be parallelizable
  }

  void iterate(){
    Path currentAnt;
    Path localBestPath;
    int localBestFitness = 0;

    for(int i = 0; i < antSquadSize; i++)
    {
      currentAnt = runAnt();
      decreasePheremones(currentAnt);
      if(numRescued(&currentAnt) >= localBestFitness)
      {
        localBestPath = currentAnt;
      }
    }
    increasePheremones(localBestPath);
    evaporatePheremones();
  }

  void run(){
    int previousBest;
    int iterations = 0;
    while(iterations < maxIterations)
    {
      previousBest = numRescued(bestPath);
      iterate();
      if(previousBest == numRescued(bestPath))
      {
        return; // We have converged.
      }
    }
  }

  VictimList victims;
  HospitalList hospitals;
  Path *bestPath;
  
  int maxIterations;
  int antSquadSize; // The number of ants that are run before picking the best of them and increasing the pheremone count on that path.
  double explorationPreference; // The amount to reduce the pheremones on an edge after it has been traversed in order to prioritize exploration.
  double bestPathReward; // The amount to increase the pheremones on a path in order to prefer previous best paths.
  double evaporationAmount; //The amount to reduce every edge's pheremone count by during evaporation.
  EdgeMatrix<double> pheremoneMatrix;



static void Run(const VictimList& victims, const HospitalList& hospitals,
                  ActionSequenceList* actionSequences){
  AntColony colony(victims, hospitals, actionSequences);
  colony.run();
}
};

}
}

#endif //_ANT_COLONY_H_
