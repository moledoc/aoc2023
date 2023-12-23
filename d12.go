package main

import (
	"bytes"
	_ "embed"
	"fmt"
	"strconv"
)

//go:embed inputs/d12.in
var b []byte

func walk(cond []byte, csize int, coffset int, grps []int, gsize int, goffset int, seen []int) int {
	if coffset == csize && goffset - gsize <= 1 {
		for i, s := range seen {
			if s != grps[i] {
				return 0
			}
		}
		// fmt.Println("returning success 1:", string(cond), grps, gsize, seen, goffset, coffset)
 		return 1
	}

	switch cond[coffset] {
	case '?':
		c := make([]byte, csize)
		cc := make([]byte, csize)
		copy(c, cond)
		copy(cc, cond)
		c[coffset]='#'
		cc[coffset]='.'
		return walk(c, csize, coffset, grps, gsize, goffset, seen) + walk(cc, csize, coffset, grps, gsize, goffset, seen)
	case '.':
		if coffset > 0 && cond[coffset-1] == '#' {
			if goffset < gsize && grps[goffset] != seen[goffset] {
				// fmt.Println("returning failure 0:", string(cond), grps, gsize, seen, goffset)
				return 0
			}
			return walk(cond, csize, coffset+1, grps, gsize, goffset+1, seen)
		} else {
			return walk(cond, csize, coffset+1, grps, gsize, goffset, seen)
		}
	case '#':
		nseen := make([]int, gsize)
		copy(nseen, seen)
		if goffset < gsize {
			nseen[goffset]++
		} else {
			return 0
		}
		return walk(cond, csize, coffset+1, grps, gsize, goffset, nseen)
	}
	return 0
}


func ex(repeat int) int {
	lines := bytes.Split(b, []byte("\n"))
	arrangements := 0
	for _, line := range lines {
		elems := bytes.Split(line, []byte(" "))
		nrbs := bytes.Split(elems[1], []byte(","))
		lElems0 := len(elems[0])
		csize := lElems0*repeat+(repeat-1)
		cond := make([]byte, csize)
		copy(cond, elems[0])
		for j:=1;j<repeat; j++ {
			cond[lElems0*j+j-1] = '?'
			for k:=0; k<lElems0; k++ {
				cond[lElems0*j+j+k] = elems[0][k]
			}
		}
		lNrbs := len(nrbs)
		gsize := lNrbs*repeat
		grps := make([]int, gsize)
		seen := make([]int, lNrbs)
		gsum := 0
		for i, nr := range nrbs {
			grps[i], _ = strconv.Atoi(string(nr))
			for j:=1; j<repeat; j++ {
				grps[lNrbs*j+i]=grps[i]
			}
			gsum += grps[i]*repeat
		}
		// fmt.Println(string(cond), grps)
		for i:=0; i<repeat; i++ {
			sidx := i*lElems0+i
			eidx := (i+1)*lElems0+(i+1)
			if i==repeat-1 {
				eidx--
			}
			partial := walk(cond[sidx:eidx], lElems0, 0, grps[i*lNrbs:(i+1)*lNrbs], lNrbs, 0, seen)
			// fmt.Println("Partial:", partial)
			arrangements += partial
		}
		// break
	}
	return arrangements
}


func main() {
	e1 := ex(1)
	fmt.Printf("e1: %v\n", e1)
}