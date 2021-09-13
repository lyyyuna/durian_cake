package bao

type node struct {
	part     string
	pattern  string
	isWild   bool
	children []*node
}

// check if node's childern match the part string
func (n *node) matchChild(part string) *node {
	for _, child := range n.children {
		if child.part == part || child.isWild == true {
			return child
		}
	}

	return nil
}

// return all node's children which match the part string
func (n *node) matchChildren(part string) []*node {
	nodes := make([]*node, 0)
	for _, child := range n.children {
		if child.part == part || child.isWild == true {
			nodes = append(nodes, child)
		}
	}

	return nodes
}

// insert
func (n *node) insert(pattern string, parts []string, height int) {
	if len(parts) == height {
		n.pattern = pattern
		return
	}

	part := parts[height]
	child := n.matchChild(part)
	if child == nil {
		child = &node{
			part:   part,
			isWild: part[0] == ':' || part[0] == '*',
		}
		n.children = append(n.children, child)
	}

	child.insert(pattern, parts, height+1)
}

// search
func (n *node) search(parts []string, height int) *node {
	if len(parts) == height {
		if n.pattern == "" {
			return nil
		}
		return n
	}

	part := parts[height]
	children := n.matchChildren(part)

	for _, child := range children {
		res := child.search(parts, height+1)
		if res != nil {
			return res
		}
	}

	return nil
}
