#include <wormArc.h>
#include <wormCollision.h>

/**
 * Simple key-value storage struct to broadcast to slaves.
 */
struct GameState {
  std::vector<WormArc> wormArcs;
  std::vector<WormCollision> wormCollisions;
};
