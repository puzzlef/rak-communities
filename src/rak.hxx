#pragma once
#include <utility>
#include <vector>
#include "_main.hxx"

using std::pair;
using std::vector;
using std::make_pair;
using std::move;




// RAK-OPTIONS
// -----------

struct RakOptions {
  int   repeat;
  float tolerance;
  int   maxIterations;

  RakOptions(int repeat=1, float tolerance=0.05, int maxIterations=500) :
  repeat(repeat), tolerance(tolerance), maxIterations(maxIterations) {}
};




// RAK-RESULT
// ----------

template <class K>
struct RakResult {
  vector<K> membership;
  int   iterations;
  float time;

  RakResult(vector<K>&& membership, int iterations=0, float time=0) :
  membership(membership), iterations(iterations), time(time) {}

  RakResult(vector<K>& membership, int iterations=0, float time=0) :
  membership(move(membership)), iterations(iterations), time(time) {}
};




// RAK-INITIALIZE
// --------------

/**
 * Initialize communities such that each vertex is its own community.
 * @param vcom community each vertex belongs to (updated)
 * @param x original graph
 */
template <class G, class K>
inline void rakInitialize(vector<K>& vcom, const G& x) {
  x.forEachVertexKey([&](auto u) { vcom[u] = u; });
}




// RAK-CHOOSE-COMMUNITY
// --------------------

/**
 * Scan an edge community connected to a vertex.
 * @param vcs communities vertex u is linked to (updated)
 * @param vcout total edge weight from vertex u to community C (updated)
 * @param u given vertex
 * @param v outgoing edge vertex
 * @param w outgoing edge weight
 * @param vcom community each vertex belongs to
 */
template <bool SELF=false, class K, class V>
inline void rakScanCommunity(vector<K>& vcs, vector<V>& vcout, K u, K v, V w, const vector<K>& vcom) {
  if (!SELF && u==v) return;
  K c = vcom[v];
  if (!vcout[c]) vcs.push_back(c);
  vcout[c] += w;
}


/**
 * Scan communities connected to a vertex.
 * @param vcs communities vertex u is linked to (updated)
 * @param vcout total edge weight from vertex u to community C (updated)
 * @param x original graph
 * @param u given vertex
 * @param vcom community each vertex belongs to
 */
template <bool SELF=false, class G, class K, class V>
inline void rakScanCommunities(vector<K>& vcs, vector<V>& vcout, const G& x, K u, const vector<K>& vcom) {
  x.forEachEdge(u, [&](auto v, auto w) { rakScanCommunity<SELF>(vcs, vcout, u, v, w, vcom); });
}


/**
 * Clear communities scan data.
 * @param vcs total edge weight from vertex u to community C (updated)
 * @param vcout communities vertex u is linked to (updated)
 */
template <class K, class V>
inline void rakClearScan(vector<K>& vcs, vector<V>& vcout) {
  for (K c : vcs)
    vcout[c] = V();
  vcs.clear();
}


/**
 * Choose connected community with most weight.
 * @param x original graph
 * @param u given vertex
 * @param vcom community each vertex belongs to
 * @param vcs communities vertex u is linked to
 * @param vcout total edge weight from vertex u to community C
 * @returns [best community, best edge weight to community]
 */
template <bool SELF=false, class G, class K, class V>
inline pair<K, V> rakChooseCommunity(const G& x, K u, const vector<K>& vcom, const vector<K>& vcs, const vector<V>& vcout) {
  K d = vcom[u];
  K cmax = K();
  V wmax = V();
  for (K c : vcs) {
    if (!SELF && c==d) continue;
    if (vcout[c]>wmax) { cmax = c; wmax = vcout[c]; }
  }
  return make_pair(cmax, wmax);
}
