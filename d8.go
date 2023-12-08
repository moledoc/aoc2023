package main

import (
	"fmt"
	"io"
	"os"
	"sort"
	"strings"
)

type node struct {
	val   string
	left  *node
	right *node
}

func e1(nodes map[string][2]string, instrs []int) int {
	instrLen := len(instrs)
	cur := "AAA";
	steps := 0
	for i := 0;; i++{
		cur = nodes[cur][instrs[i]]
		steps++;
		if cur == "ZZZ" {
			break;
		}
		if i == instrLen-1{ 
			i = -1
		}
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
	nlen := len(nlines)

	// make node mapping
	nodes := make(map[string][2]string)
	for i := 0; i < nlen; i++ {
		if (nlines[i] == "\n") {
			continue;
		}
		nlines[i] = strings.Replace(nlines[i], "= ", "", 1)
		nlines[i] = strings.Replace(nlines[i], ",", "", 1)
		nlines[i] = strings.Replace(nlines[i], "(", "", 1)
		nlines[i] = strings.Replace(nlines[i], ")", "", 1)
		lineElems := strings.Split(nlines[i], " ")
		if len(lineElems) == 0 {
			continue;
		}
		nodes[lineElems[0]] = [2]string{lineElems[1], lineElems[2]}
	}

	// translate instructions to be more easy to use
	instrs := make([]int, len(lines[0]))
	for i,  c := range lines[0] {
		if c == 'L' {
			instrs[i] = 0;
		} else if c == 'R' {
			instrs[i] = 1;
		}
	}

	fmt.Printf("e1: %v\n", e1(nodes, instrs))
}
