#!/usr/local/bin/python3.1

from functools import reduce
from lie import *

def nthweight(n, m):
    l = m*[0]
    l[n] = 1
    return l

def nbrep(n, r):
    rs = [(grp(1, c-1).fund() if k == n else grp(1, c-1).trivial()) \
          for k, (c, _) in enumerate(r) if c > 1]
    return reduce(rep.prod, rs, T0.rep(pol(1)))
def abrep(n, r):
    return grp(0, len(r)).rep(nthweight(n, len(r)))
def globrep(n, r):
    return nbrep(n, r).prod(abrep(n,r))

def dress(r):
    nbglob = reduce(grp.__mul__, [grp(1, c-1) for c,_ in r if c > 1], T0)
    abglob = grp(0,len(r))
    glob = nbglob * abglob
    nb_indices = []
    s = 0*(r.grp*glob).trivial()
    for k, (c, i) in enumerate(r):
        if c > 1: nb_indices.append(k)
        s += i.prod(globrep(k, r))
    return s, glob, nb_indices

def find_couplings_to(r, dressed):
    s = slice(len(r.grp), None)
    glob = dressed.grp[s]
    cs = 0*glob.trivial()
    for _, irr in dressed:
        if irr.branch_n(slice(0,len(r.grp))).coef(r.dual()) > 0:
            cs = cs + irr.branch_n(s)/r.dim()
    return cs
