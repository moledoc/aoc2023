package main

import (
	"fmt"
	"io"
	"os"
	"sort"
	"strings"
)

func e1(nlines []string, instrs []int) int {
	steps := 0

	nlen := len(nlines)
	// make node mapping
	nodes := make(map[string][2]string)
	for i := 0; i < nlen; i++ {
		if nlines[i] == "\n" {
			continue
		}
		nlines[i] = strings.Replace(nlines[i], "= ", "", 1)
		nlines[i] = strings.Replace(nlines[i], ",", "", 1)
		nlines[i] = strings.Replace(nlines[i], "(", "", 1)
		nlines[i] = strings.Replace(nlines[i], ")", "", 1)
		lineElems := strings.Split(nlines[i], " ")
		if len(lineElems) <= 1 {
			continue
		}
		nodes[lineElems[0]] = [2]string{lineElems[1], lineElems[2]}
	}

	instrLen := len(instrs)
	cur := "AAA"
	for i := 0; ; i++ {
		cur = nodes[cur][instrs[i]]
		steps++
		if cur == "ZZZ" {
			break
		}
		if i == instrLen-1 {
			i = -1
		}
	}
	return steps
}

func min(a int, b int) int {
	if a < b {
		return a
	}
	return b
}
func max(a int, b int) int {
	if a > b {
		return a
	}
	return b
}
func gcd(a int, b int) int {
	if min(a, b) == 0 {
		return max(a, b)
	}
	a1 := max(a, b) % min(a, b)
	return gcd(a1, min(a, b))
}
func lcm(a int, b int) int {
	return a * b / gcd(a, b)
}
func e2(nlines []string, instrs []int) int {
	steps := 0

	nlen := len(nlines)
	// make node mapping
	nodes := make(map[string][2]string)
	cur := []string{}
	for i := 0; i < nlen; i++ {
		if nlines[i] == "\n" {
			continue
		}
		nlines[i] = strings.Replace(nlines[i], "= ", "", 1)
		nlines[i] = strings.Replace(nlines[i], ",", "", 1)
		nlines[i] = strings.Replace(nlines[i], "(", "", 1)
		nlines[i] = strings.Replace(nlines[i], ")", "", 1)
		lineElems := strings.Split(nlines[i], " ")
		if len(lineElems) <= 1 {
			continue
		}
		e0 := lineElems[0]
		nodes[e0] = [2]string{lineElems[1], lineElems[2]}
		if e0[2] == 'A' {
			cur = append(cur, e0)
		}
	}
	instrLen := len(instrs)
	stepsPartial := []int{}
	for _, c := range cur {
		tmp := 0
		for {
			c = nodes[c][instrs[tmp%instrLen]]
			tmp++
			if c[2] == 'Z' {
				break
			}
		}
		stepsPartial = append(stepsPartial, tmp)
	}
	steps = stepsPartial[0]
	for i := 1; i < len(stepsPartial); i++ {
		steps = lcm(steps, stepsPartial[i])
	}

	return steps
}

func main() {
	file, _ := os.Open("./inputs/d8.in")
	b, _ := io.ReadAll(file)
	file.Close()
	lines := strings.Split(string(b), "\n")
	nlines := lines[2:]
	sort.Slice(nlines, func(i int, j int) bool { return nlines[i] < nlines[j] })

	// translate instructions to be more easy to use
	instrs := make([]int, len(lines[0]))
	for i, c := range lines[0] {
		if c == 'L' {
			instrs[i] = 0
		} else if c == 'R' {
			instrs[i] = 1
		}
	}

	fmt.Printf("e1: %v\n", e1(nlines, instrs))
	fmt.Printf("e2: %v\n", e2(nlines, instrs))
}
