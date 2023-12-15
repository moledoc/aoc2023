package main

import (
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
		
	

func main() {
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
	fmt.Printf("e1: %v\n", arrangements)
}