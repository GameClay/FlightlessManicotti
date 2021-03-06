/* Benchmarks various fast access algorithms. (C) 2010 Niall Douglas.


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define ALLOCATIONS 4096
#define AVERAGE 8
#define ITEMSIZE 0        /* Set =page size to test TLB scaling */

#include "nedtrie.h"
#include "rbtree.h"
#include "llrbtree.h"
#include "uthash/src/uthash.h"

#ifdef __cplusplus
#include <map>
#include <unordered_map>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef unsigned __int64 usCount;
static usCount GetUsCount()
{
	static LARGE_INTEGER ticksPerSec;
	static double scalefactor;
	LARGE_INTEGER val;
	if(!scalefactor)
	{
		if(QueryPerformanceFrequency(&ticksPerSec))
			scalefactor=ticksPerSec.QuadPart/1000000000000.0;
		else
			scalefactor=1;
	}
	if(!QueryPerformanceCounter(&val))
		return (usCount) GetTickCount() * 1000000000;
	return (usCount) (val.QuadPart/scalefactor);
}
#else
#include <sys/time.h>
#include <time.h>
typedef unsigned long long usCount;
static usCount GetUsCount()
{
#ifdef CLOCK_MONOTONIC
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ((usCount) ts.tv_sec*1000000000000LL)+ts.tv_nsec*1000LL;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return ((usCount) tv.tv_sec*1000000000000LL)+tv.tv_usec*1000000LL;
#endif
}
#endif

/* Include the Mersenne twister */
#if defined(_M_X64) || defined(__x86_64__) || (defined(_M_IX86) && _M_IX86_FP>=2) || (defined(__i386__) && defined(__SSE2__))
#define HAVE_SSE2 1
#endif
#define MEXP 19937
#include "SFMT.c"


typedef struct AlgorithmInfo_t
{
  const char *name;
  usCount inserts[ALLOCATIONS], finds1[ALLOCATIONS], finds2[ALLOCATIONS], removes[ALLOCATIONS], iterates[ALLOCATIONS];
} AlgorithmInfo;

#define BENCHMARK_PREFIX(foo)                     nedtrie_##foo
#define REGION_ENTRY(type)                        NEDTRIE_ENTRY(type)
#define REGION_HEAD(name, type)                   NEDTRIE_HEAD(name, type)
#define REGION_INIT(treevar)                      NEDTRIE_INIT(treevar)
#define REGION_EMPTY(treevar)                     NEDTRIE_EMPTY(treevar)
#define REGION_GENERATE(proto, treetype, nodetype, link, cmpfunct) NEDTRIE_GENERATE(proto, treetype, nodetype, link, cmpfunct, NEDTRIE_NOBBLEZEROS(treetype))
#define REGION_INSERT(treetype, treevar, node)    NEDTRIE_INSERT(treetype, treevar, node)
#define REGION_REMOVE(treetype, treevar, node)    NEDTRIE_REMOVE(treetype, treevar, node)
#define REGION_FIND(treetype, treevar, node)      NEDTRIE_FIND(treetype, treevar, node)
#define REGION_NFIND(treetype, treevar, node)     NEDTRIE_NFIND(treetype, treevar, node)
#define REGION_MAX(treetype, treevar)             NEDTRIE_MAX(treetype, treevar)
#define REGION_MIN(treetype, treevar)             NEDTRIE_MIN(treetype, treevar)
#define REGION_NEXT(treetype, treevar, node)      NEDTRIE_NEXT(treetype, treevar, node)
#define REGION_PREV(treetype, treevar, node)      NEDTRIE_PREV(treetype, treevar, node)
#define REGION_FOREACH(var, treetype, treevar)    NEDTRIE_FOREACH(var, treetype, treevar)
#define REGION_HASNODEHEADER(treevar, node, link) NEDTRIE_HASNODEHEADER(treevar, node, link)
#define BENCHMARK_USEKEYFUNCT
#include "benchmark.c.h"
#undef BENCHMARK_USEKEYFUNCT
#undef BENCHMARK_PREFIX
#undef REGION_ENTRY
#undef REGION_HEAD
#undef REGION_INIT
#undef REGION_EMPTY
#undef REGION_GENERATE
#undef REGION_INSERT
#undef REGION_REMOVE
#undef REGION_FIND
#undef REGION_NFIND
#undef REGION_MAX
#undef REGION_MIN
#undef REGION_NEXT
#undef REGION_PREV
#undef REGION_FOREACH
#undef REGION_HASNODEHEADER

#define BENCHMARK_PREFIX(foo)                     rbtree_##foo
#define REGION_ENTRY(type)                        RB_ENTRY(type)
#define REGION_HEAD(name, type)                   RB_HEAD(name, type)
#define REGION_INIT(treevar)                      RB_INIT(treevar)
#define REGION_EMPTY(treevar)                     RB_EMPTY(treevar)
#define REGION_GENERATE(proto, treetype, nodetype, link, cmpfunct) RB_GENERATE_INTERNAL(treetype, nodetype, link, cmpfunct, proto)
#define REGION_INSERT(treetype, treevar, node)    RB_INSERT(treetype, treevar, node)
#define REGION_REMOVE(treetype, treevar, node)    RB_REMOVE(treetype, treevar, node)
#define REGION_FIND(treetype, treevar, node)      RB_FIND(treetype, treevar, node)
#define REGION_NFIND(treetype, treevar, node)     RB_NFIND(treetype, treevar, node)
#define REGION_MAX(treetype, treevar)             RB_MAX(treetype, treevar)
#define REGION_MIN(treetype, treevar)             RB_MIN(treetype, treevar)
#define REGION_NEXT(treetype, treevar, node)      RB_NEXT(treetype, treevar, node)
#define REGION_PREV(treetype, treevar, node)      RB_PREV(treetype, treevar, node)
#define REGION_FOREACH(var, treetype, treevar)    RB_FOREACH(var, treetype, treevar)
#define REGION_HASNODEHEADER(treevar, node, link) (RB_PARENT(node, link) || RB_ROOT(treevar)==(node))
#include "benchmark.c.h"
#undef BENCHMARK_PREFIX
#undef REGION_ENTRY
#undef REGION_HEAD
#undef REGION_INIT
#undef REGION_EMPTY
#undef REGION_GENERATE
#undef REGION_INSERT
#undef REGION_REMOVE
#undef REGION_FIND
#undef REGION_NFIND
#undef REGION_MAX
#undef REGION_MIN
#undef REGION_NEXT
#undef REGION_PREV
#undef REGION_FOREACH
#undef REGION_HASNODEHEADER

#define BENCHMARK_PREFIX(foo)                     hash_##foo
#define BENCHMARK_NOHEADTYPE
#define REGION_ENTRY(type)                        UT_hash_handle
#define REGION_HEAD(name, type)                   typedef struct type *name
#define REGION_INIT(treevar)                      (*(treevar)=NULL)
#define REGION_EMPTY(treevar)                     (!HASH_CNT(link, (*(treevar))))
#define REGION_GENERATE(proto, treetype, nodetype, link, cmpfunct) static struct nodetype *HashFind(treetype *head, size_t *key) { struct nodetype *ret=0; HASH_FIND(link, *head, key, sizeof(*key), ret); return ret; }
#define REGION_INSERT(treetype, treevar, node)    HASH_ADD(link, (*(treevar)), key, sizeof((node)->key), (node))
#define REGION_REMOVE(treetype, treevar, node)    HASH_DELETE(link, (*(treevar)), (node))
#define REGION_FIND(treetype, treevar, node)      HashFind(treevar, &(node)->key)
#define REGION_NFIND(treetype, treevar, node)     fail
#define REGION_MAX(treetype, treevar)             fail
#define REGION_MIN(treetype, treevar)             (*(treevar))
#define REGION_NEXT(treetype, treevar, node)      ((treetype)((node)->link.next))
#define REGION_PREV(treetype, treevar, node)      ((treetype)((node)->link.prev))
#define REGION_FOREACH(var, treetype, treevar)    fail
#define REGION_HASNODEHEADER(treevar, node, link) fail
#include "benchmark.c.h"
#undef BENCHMARK_PREFIX
#undef BENCHMARK_NOHEADTYPE
#undef REGION_ENTRY
#undef REGION_HEAD
#undef REGION_INIT
#undef REGION_EMPTY
#undef REGION_GENERATE
#undef REGION_INSERT
#undef REGION_REMOVE
#undef REGION_FIND
#undef REGION_NFIND
#undef REGION_MAX
#undef REGION_MIN
#undef REGION_NEXT
#undef REGION_PREV
#undef REGION_FOREACH
#undef REGION_HASNODEHEADER

#ifdef __cplusplus
static size_t nodekeys[ALLOCATIONS];
template<class stlcontainer> void RunTest(AlgorithmInfo *ai)
{
  stlcontainer nodes;
  typename stlcontainer::iterator it;
  int l, n, m;
  usCount start, end;
  printf("Running scalability test for %s\n", ai->name);
  init_gen_rand(1234);
  for(n=0; n<ALLOCATIONS; n++)
  {
    nodekeys[n]=gen_rand32();
    for(m=0; m<n; m++)
      if(nodekeys[n]==nodekeys[m]) { nodekeys[n]=gen_rand32(); m=-1; }
  }
  for(m=0; m<ALLOCATIONS; m++)
  {
    usCount insert=0, find1=0, find2=0, remove=0, iterate=0;
    int lmax=(nedtriebitscanr(ALLOCATIONS)-nedtriebitscanr(m)); /* Loop more when m is smaller */
    if(lmax<1) lmax=1;
    for(l=0; l<lmax; l++)
    {
      for(n=0; n<m; n++)
      {
        int ridx=gen_rand32() % (n+1);
        start=GetUsCount();
        nodes[nodekeys[n]]=78;
        end=GetUsCount();
        insert+=end-start;
        start=GetUsCount();
        it=nodes.find(nodekeys[ridx]);
        end=GetUsCount();
        if(nodes.end()==it) abort();
        find1+=end-start;
      }
      for(n=0; n<m; n++)
      {
        start=GetUsCount();
        it=nodes.find(nodekeys[n]);
        end=GetUsCount();
        if(nodes.end()==it) abort();
        find2+=end-start;
      }
      for(it=nodes.begin(); it!=nodes.end();)
      {
        start=GetUsCount();
        ++it;
        end=GetUsCount();
        iterate+=end-start;
      }
      for(n=0; n<m; n++)
      {
        start=GetUsCount();
        nodes.erase(nodes.find(nodekeys[n]));
        end=GetUsCount();
        remove+=end-start;
      }
    }
    ai->inserts[m]=(usCount)((double) insert/l);
    ai->finds1[m]=(usCount)((double)find1/l);
    ai->finds2[m]=(usCount)((double)find2/l);
    ai->removes[m]=(usCount)((double)remove/l);
    ai->iterates[m]=(usCount)((double)iterate/l);
    //if(!(m & 127)) printf("At %d = %lu, %lu, %lu, %lu, %lu\n", m, ai->inserts[m], ai->finds1[m], ai->finds2[m], ai->removes[m], ai->iterates[m]);
  }
}
#endif /* __cplusplus */


#define ALGORITHMS 6
int main(void)
{
  int n, m, algorithmslen=0;
  static AlgorithmInfo algorithms[ALGORITHMS];
  FILE *oh;
  char buffer[256];

  if(1)
  {
    /* These are the C benchmarks */
    algorithms[algorithmslen].name="nedtrie";
    nedtrie_RunTest(algorithms+algorithmslen++);
    algorithms[algorithmslen].name="rbtree";
     rbtree_RunTest(algorithms+algorithmslen++);
    algorithms[algorithmslen].name="hash";
     hash_RunTest(algorithms+algorithmslen++);
  }
#ifdef __cplusplus
  if(1)
  {
    using namespace std;
    algorithms[algorithmslen].name="trie_map<size_t>";
    RunTest<nedtries::trie_map<size_t, size_t> >(algorithms+algorithmslen++);
    algorithms[algorithmslen].name="map<size_t>";
    RunTest<map<size_t, size_t> >(algorithms+algorithmslen++);
    algorithms[algorithmslen].name="unordered_map<size_t>";
    RunTest<unordered_map<size_t, size_t> >(algorithms+algorithmslen++);
  }
#endif

  sprintf(buffer, "results%u%s.csv",
    (unsigned)(8*sizeof(void *)),
#ifdef WIN32
    "_win32");
#else
    "_posix");
#endif
  oh=fopen(buffer, "w");
  assert(oh);
  if(!oh) abort();
  for(m=0; m<algorithmslen; m++)
  {
    fprintf(oh, "\"Insert (%s)\",\"Find 0-N (%s)\",\"Find N (%s)\",\"Remove (%s)\",\"Iterate (%s)\"%c", algorithms[m].name, algorithms[m].name, algorithms[m].name, algorithms[m].name, algorithms[m].name, m==algorithmslen-1 ? '\n' : ',');
    algorithms[m].inserts[0]=algorithms[m].finds1[0]=algorithms[m].finds2[0]=algorithms[m].removes[0]=algorithms[m].iterates[0]=1;
  }
  /* Max out the CPU to try to counter SpeedStep */
  {
    usCount start=GetUsCount();
    while(GetUsCount()-start<1000000000000ULL);
  }
	for(n=0; n<ALLOCATIONS; n++)
	{
    for(m=0; m<algorithmslen; m++)
    {
      int k, added=0;
      double inserts=0, finds1=0, finds2=0, removes=0, iterates=0;
      for(k=n-AVERAGE/2; k<=n+AVERAGE/2; k++)
      {
        if(k<0 || k>=ALLOCATIONS) continue;
        inserts+=pow(algorithms[m].inserts[k]/1000000000000.0, 1.0/3);
        finds1+=pow(algorithms[m].finds1[k]/1000000000000.0, 1.0/3);
        finds2+=pow(algorithms[m].finds2[k]/1000000000000.0, 1.0/3);
        removes+=pow(algorithms[m].removes[k]/1000000000000.0, 1.0/3);
        iterates+=pow(algorithms[m].iterates[k]/1000000000000.0, 1.0/3);
        added++;
      }
      fprintf(oh, "%lf,%lf,%lf,%lf,%lf%c",
        n/(pow(inserts/added, 3)),
        n/(pow(finds1/added, 3)),
        n/(pow(finds2/added, 3)),
        n/(pow(removes/added, 3)),
        n/(pow(iterates/added, 3)),
        m==algorithmslen-1 ? '\n' : ',');
    }
	}
	fclose(oh);
  return 0;
}
