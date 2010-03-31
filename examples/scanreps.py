#!/usr/local/bin/python3.1

import sys
from lie import *
from functools import *
import terms

class Vector(list):
    def __repr__(self):
        return "Vector(%s)" % list.__repr__(self)
    def __str__(self):
        return "(" + ",".join([str(x) for x in self]) + ")"
    def __add__(self, other):
        return Vector([x+y for x,y in zip(self, other)])
    def __mul__(self, other):
        return Vector([x*other for x in self])
    def __rmul__(self, other): return self*other
    def __sub__(self, other): return self + (-other)
    def __neg__(self): return -1*self

def weightlist(b, r):
    units = [Vector(i*[0]+[1]+(r-i-1)*[0]) for i in range(r)]
    ws = reduce(lambda a,b: [x+y for x in a for y in b], b*[units])
    return [list(w) for w in ws]

def smallirreps(g, max_index):
    irreps = []
    b = 0
    newirreps = [g.trivial()]
    while newirreps:
        irreps = irreps + newirreps
        newirreps = []
        b = b + 1
        for w in weightlist(b, g.rank()):
            r = g.rep(w)
            if r.dynkin_index() <= max_index: newirreps.append(r)
    return irreps

def d_inds(r):
    return Vector([r.branch_n(i).dynkin_index() for i in range(len(r.grp))])

def vectorlike(r):
    return r == r.dual() and r.sym_tensor(2).coef(r.grp.trivial()) == 1

gSM  = A4
gCFT = grp(sys.argv[1])

T    = gSM.fund().alt_tensor(2).prod(gCFT.trivial())
Fb   = gSM.fund().dual().prod(gCFT.trivial())
triv = (gSM*gCFT).trivial()

max_inds = Vector([50]+[3*di for di in d_inds(gCFT.adjoint())])

numCFTfields = [2,3]
numcouplings = 2

SMreps = smallirreps(gSM, max_inds[0])
CFTreps = smallirreps(gCFT, max_inds[1])

reps = [(Vector([0,0]),0*triv)]
print("gauge group:", (gSM*gCFT).pprint())
print("max dynkin indices:", max_inds)
print("vector-like pieces:")
for u in SMreps:
    for v in CFTreps:
        if v == gCFT.trivial(): continue
        uv = u.prod(v)
        
        # we only want vectorlike theories:
        if not vectorlike(uv):

            # less than or equal to, in case uv has only an
            # antisymmetric coupling to its dual.  In such a case, we
            # need two copies for a vectorlike pair.
            if uv.hw() <= uv.dual().hw():
                uv = uv + uv.dual()
            else: continue

        uv_inds = d_inds(uv)
        if max(uv_inds - max_inds) > 0: continue

        print(uv.pprint(), " dynkin indices:", str(uv_inds))
        newreps = []
        for r_inds, r in reps:
            n = min([int((x - a)/(b+Fraction(1,10000))) for x,a,b in zip(max_inds, r_inds, uv_inds)])

            # if numCFTfields copies suffices, extra copies of r won't
            # give qualitatively different couplings
            #n = min(n, 1)
            #if n:
            for k in range(n+1): newreps.append((r_inds+k*uv_inds, r + k*uv))
        reps = newreps

def charges(r):
    if r.grp.toral_dim() == 0: return []
    if r.grp.ss_dim() == 0: return [x for x in r.hw()]
    else: return charges(r.branch_n(-1))

def to_the(x):
    return ("^%d" % int(x) if x!=1 else "")

def get_couplings_to(s, r, numfields):
    dressed, glob, nb_indices = terms.dress(r)
    couplings = terms.find_couplings_to(s, dressed.sym_tensor(numfields - 1))
    for c, cp in couplings:
        if c != 0:
            l = [r[k][1].pprint()+to_the(x) for k, x in enumerate(charges(cp)) if x > 0]
            yield " ".join(l)

Td = T.dual()
count = 0
print ("\nvectorlike matter with two 10 couplings:")
print ("representation\t\tdynkin indices\t\t3 and 4-field couplings")
for r_inds, r in reps:    
    cs = [r.sym_tensor(i).coef(Td) for i in numCFTfields]
    csums = [sum(cs[:i+1]) for i in range(len(cs))]
    if cs == [1,1] or cs == [2,0] or cs == [0,2]: #numcouplings in csums:
        count += 1
        print("%d. " % count, r.pprint(), "\td inds:", r_inds, "\t", cs)
        for l in get_couplings_to(T, r, 3):
            print("\t", T.pprint(), l)
        for l in get_couplings_to(T, r, 4):
            print("\t", T.pprint(), l)
        for l in get_couplings_to(Fb, r, 3):
            print("\t", Fb.pprint(), l)
        for l in get_couplings_to(Fb, r, 4):
            print("\t", Fb.pprint(), l)

print("\n---------\n")
