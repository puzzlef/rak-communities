#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"
#include "rak.hxx"

using std::vector;
using std::swap;




// RAK-MOVE-ITERATION
// ------------------

/**
 * Move each vertex to its best community.
 * @param vcs communities vertex u is linked to (updated)
 * @param vcout total edge weight from vertex u to community C (updated)
 * @param vcom community each vertex belongs to (updated)
 * @param x original graph
 * @param vdom community each vertex belonged to
 * @returns number of changed vertices
 */
template <class G, class K, class V>
K rakMoveIteration(vector<K>& vcs, vector<V>& vcout, vector<K>& vcom, const G& x, const vector<K>& vdom) {
  K a = K();
  x.forEachVertexKey([&](auto u) {
    rakClearScan(vcs, vcout);
    rakScanCommunities(vcs, vcout, x, u, vdom);
    auto [c, w] = rakChooseCommunity(x, u, vdom, vcs, vcout);
    if (c) { vcom[u] = c; ++a; }
  });
  return a;
}




// RAK-SEQ
// -------

template <bool ASYNC=false, class G, class K>
RakResult<K> rakSeq(const G& x, const vector<K>* q, const RakOptions& o) {
  using V = typename G::edge_value_type;
  int l = 0;
  K S = x.span();
  K N = x.order();
  vector<K> vdom(S), vcom(S), vcs;
  vector<V> vcout(S);
  float t = measureDuration([&]() {
    rakInitialize(vdom, x);
    for (l=0; l<o.maxIterations;) {
      K n = rakMoveIteration(vcs, vcout, !ASYNC? vcom : vdom, x, vdom); ++l;
      if (float(n)/N <= o.tolerance) break;
      if (!ASYNC) swap(vcom, vdom);
    }
  }, o.repeat);
  return {vdom, l, t};
}




// RAK-SEQ-STATIC
// --------------

template <bool ASYNC=false, class G, class K>
inline auto rakSeqStatic(const G& x, const vector<K>* q=nullptr, const RakOptions& o={}) {
  return rakSeq<ASYNC>(x, q, o);
}
