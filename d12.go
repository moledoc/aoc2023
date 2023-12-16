package main

import (
	"bytes"
	_ "embed"
	"fmt"
	"strconv"
	"strings"
)

//go:embed inputs/d12.in
var b []byte

// recursive from left
// 2 paths: fill in the value, don't fill in
// sum up the returns
// if satisfies the constraints, return 1, if encounters issue, return 0
// trim edges and make multi dots into single dot

func options(line string, nrs []int, lineLen int, offset int, hcount int, xsum int, xorig int, trace int) int {
	if lineLen == offset || xsum < 0 || len(nrs) == 0 {
		if xsum == 0 {
			elems := strings.Split(line, ".")
			var nnrs []int
			for _, e := range elems {
				if e == "" {
					continue
				}
				n := strings.Count(e, "#")
				nnrs = append(nnrs, n)
			}
			if len(nnrs) != len(nrs) {
				return 0
			}
			for i:=0; i<len(nrs); i++ {
				if nnrs[i] != nrs[i] {
					return 0
				}
			}
			// fmt.Println(trace, line, hcount, nnrs, nrs)
			return 1
		}
		return 0
	}

	switch line[offset] {
	case '?':
		l := []byte(line)
		ll := []byte(line)
		l[offset]='.'
		ll[offset]='#'
		uf := options(string(l), nrs, lineLen, offset, hcount, xsum, xorig, 1)
		f := options(string(ll), nrs, lineLen, offset, hcount, xsum, xorig, 2)
		return uf+f
	case '#':
		return options(line, nrs, lineLen, offset+1, hcount+1, xsum-1, xorig, trace)
	case '.':
		return options(line, nrs, lineLen, offset+1, hcount, xsum, xorig, trace)
	}
	return 0
}
		
	

func e1() int {
	lines := strings.Split(string(b), "\n")
	arrangements := 0
	for _, line := range lines {
		line = strings.Trim(line, ".")
		line = strings.ReplaceAll(line, "..", ".")
		elems := strings.Split(line, " ")
		nrBs := strings.Split(elems[1], ",")
		nsize := len(nrBs)
		nrs := make([]int, nsize)
		// fmt.Println(string(line))
		nsum := 0
		for i, nr := range nrBs {
			nrs[i],_ = strconv.Atoi(string(nr))
			nsum += nrs[i]
		}
		// break
		arrangements +=options(elems[0], nrs, len(elems[0]), 0, 0, nsum, nsum, 0)
	}
	return arrangements
}


func walk(cond []byte, csize int, coffset int, grps []int, gsize int, goffset int, seen []int) int {
	if coffset == csize && goffset - gsize <= 1 {
		for i, s := range seen {
			if s != grps[i] {
				return 0
			}
		}
		//fmt.Println("returning success 1:", string(cond), grps, gsize, seen, goffset, coffset)
 		return 1
	}

	for i, s := range seen {
		if s > grps[i] {
			return 0
		}
		if s == 0 {
			break
		}
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
				//fmt.Println("returning failure 0:", string(cond), grps, gsize, seen, goffset)
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
		csize := len(elems[0])
		cond := make([]byte, csize*repeat)
		copy(cond, elems[0])
		gsize := len(nrbs)
		grps := make([]int, gsize)
		seen := make([]int, gsize)
		gsum := 0
		for i, nr := range nrbs {
			grps[i], _ = strconv.Atoi(string(nr))
			gsum += grps[i]
		}
		// fmt.Println(grps, nsum, seen)
		arrangements += walk(elems[0], len(elems[0]), 0, grps, gsize, 0, seen)
		// break
	}
	return arrangements
}


func main() {
	fmt.Printf("ex1: %v\n", ex(1))
	fmt.Printf("e1: %v\n", e1())
}