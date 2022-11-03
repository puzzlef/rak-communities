#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"
#include "rak.hxx"

using std::tuple;
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
template <class G, class K, class V, class FA, class FP>
K rakMoveIteration(vector<K>& vcs, vector<V>& vcout, vector<K>& vcom, const G& x, const vector<K>& vdom, FA fa, FP fp) {
  K a = K();
  x.forEachVertexKey([&](auto u) {
    if (!fa(u)) return;
    K d = vdom[u];
    rakClearScan(vcs, vcout);
    rakScanCommunities(vcs, vcout, x, u, vdom);
    auto [c, w] = rakChooseCommunity<true>(x, u, vdom, vcs, vcout);
    if (c && c!=d) { vcom[u] = c; ++a; fp(u); }
  });
  return a;
}




// RAK-SEQ
// -------

template <bool ASYNC=false, class G, class K, class FA, class FP>
RakResult<K> rakSeq(const G& x, const vector<K>* q, const RakOptions& o, FA fa, FP fp) {
  using V = typename G::edge_value_type;
  int l = 0;
  K S = x.span();
  K N = x.order();
  vector<K> vdom(S), vcom(S), vcs;
  vector<V> vcout(S);
  float t = measureDuration([&]() {
    rakInitialize(vdom, x);
    for (l=0; l<o.maxIterations;) {
      K n = rakMoveIteration(vcs, vcout, !ASYNC? vcom : vdom, x, vdom, fa, fp); ++l;
      PRINTFD("rakSeq(): l=%d, n=%d, N=%d, n/N=%f\n", l, n, N, float(n)/N);
      if (float(n)/N <= o.tolerance) break;
      if (!ASYNC) swap(vcom, vdom);
    }
  }, o.repeat);
  return {vdom, l, t};
}
template <bool ASYNC=false, class G, class K, class FA>
inline RakResult<K> rakSeq(const G& x, const vector<K>* q, const RakOptions& o, FA fa) {
  auto fp = [](auto u) {};
  return rakSeq<ASYNC>(x, q, o, fa, fp);
}
template <bool ASYNC=false, class G, class K>
inline RakResult<K> rakSeq(const G& x, const vector<K>* q, const RakOptions& o) {
  auto fa = [](auto u) { return true; };
  return rakSeq<ASYNC>(x, q, o, fa);
}




// RAK-SEQ-STATIC
// --------------

template <bool ASYNC=false, class G, class K>
inline RakResult<K> rakSeqStatic(const G& x, const vector<K>* q=nullptr, const RakOptions& o={}) {
  return rakSeq<ASYNC>(x, q, o);
}




// RAK-SEQ-DYNAMIC-DELTA-SCREENING
// -------------------------------

template <bool ASYNC=false, class G, class K, class V>
inline RakResult<K> rakSeqDynamicDeltaScreening(const G& x, const vector<tuple<K, K>>& deletions, const vector<tuple<K, K, V>>& insertions, const vector<K>* q, const RakOptions& o={}) {
  K S = x.span();
  const vector<K>& vcom = *q;
  auto vaff = rakAffectedVerticesDeltaScreening(x, deletions, insertions, vcom);
  auto fa   = [&](auto u) { return vaff[u]==true; };
  return rakSeq<ASYNC>(x, q, o, fa);
}




// RAK-SEQ-DYNAMIC-FRONTIER
// ------------------------

template <bool ASYNC=false, class G, class K, class V>
inline RakResult<K> rakSeqDynamicFrontier(const G& x, const vector<tuple<K, K>>& deletions, const vector<tuple<K, K, V>>& insertions, const vector<K>* q, const RakOptions& o={}) {
  K S = x.span();
  const vector<K>& vcom = *q;
  auto vaff = rakAffectedVerticesFrontier(x, deletions, insertions, vcom);
  auto fa = [&](auto u) { return vaff[u]==true; };
  auto fp = [&](auto u) { x.forEachEdgeKey(u, [&](auto v) { vaff[v] = true; }); };
  return rakSeq<ASYNC>(x, q, o, fa, fp);
}
