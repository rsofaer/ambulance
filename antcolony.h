#ifndef _ANT_COLONY_H_
#define _ANT_COLONY_H_

#include <vector>
//#include <omp.h>

template<typename FitnessType, typename PathType>
struct AntColony {
  AntColony()
  : maxIterations(1000),
    iterations(0),
    antSquadSize(4),
    bestPath(NULL),
    bestPathReward(10),
    evaporationAmount(1),
    explorationPreference(3)
  {};

  AntColony(int maxIterations_, int squadSize)
  : maxIterations(maxIterations_),
    iterations(0),
    antSquadSize(squadSize),
    bestPath(NULL)
  {};
  
  ~AntColony()
  {};

  PathType runAnt(){
    //Run an ant on the graph and return the ant's path for examination.
  }
  void adjustPheremones(PathType path, FitnessType amount)
  {
    // Adjust the pheremone count on every edge in the path.
    // Should be parallelizable
  }

  void decreasePheremones(PathType path)
  {
    adjustPheremones(path, explorationPreference);
  }
  void increasePheremones(PathType path)
  {
    adjustPheremones(path, bestPathReward);
  }

  void evaporatePheremones(){
    // reduce the pheremone count on every edge by evaporationAmount
    // should be parallelizable
  }

  void iterate(){
    PathType currentAnt;
    PathType localBestPath;
    FitnessType localBestFitness = 0;

    for(int i = 0; i < antSquadSize; i++)
    {
      currentAnt = runAnt();
      reducePheremones(currentAnt);
      if(currentAnt.fitness >= localBestFitness)
      {
        localBestPath = currentAnt;
      }
    }
    increasePheremones(localBestPath);
    evaporatePheremones();
  }

  void run(){
    FitnessType previousBest;
    while(iterations < maxIterations)
    {
      previousBest = bestPath.fitness;
      iterate();
      if(previousBest == bestPath.fitness)
      {
        return; // We have converged.
      }
    }
  }

  PathType bestPath;
  int maxIterations;
  int iterations;
  int antSquadSize; // The number of ants that are run before picking the best of them and increasing the pheremone count on that path.
  FitnessType explorationPreference; // The amount to reduce the pheremones on an edge after it has been traversed in order to prioritize exploration.
  FitnessType bestPathReward; // The amount to increase the pheremones on a path in order to prefer previous best paths.
  FitnessType evaporationAmount; //The amount to reduce every edge's pheremone count by during evaporation.
};

#endif //_ANT_COLONY_H_
