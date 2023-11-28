class BFS:
    def __init__(self, graph, starting_vertex):
        self.starting_vertex = starting_vertex
        self.marked = []
        self.edgeTo = []
        for i in range(graph.V()):
            self.marked.append(False)
            self.edgeTo.append(None)
        self.bfs(graph, starting_vertex)
    
    # Breadth First Search function
    def bfs(self, graph, vertex):
        # Initialise queue
        queue = []
        # Enqueue current vertex
        queue.append(vertex)
        # Mark current vertex as marked
        self.marked[vertex] = True

        # Loop through each element of the queue
        while(len(queue) != 0):
            # Dequeue element from queue
            current_vertex = queue.pop(0)
            for adj_vertex in graph.adj_list[current_vertex]:
                if(not self.marked[adj_vertex]):
                    queue.append(adj_vertex)
                    self.marked[adj_vertex] = True
                    self.edgeTo[adj_vertex] = current_vertex

    # Function to check if there exists a path from the starting vertex to the given vertex
    def hasPathTo(self, vertex):
        return self.marked[vertex]
    
    # Function to get the path to a given vertex from the starting vertex
    def pathTo(self, vertex):
        if(not self.hasPathTo(vertex)):
            return None
        else:
            path = []
            current_vertex = vertex
            while(current_vertex != self.starting_vertex):
                path.append(current_vertex)
                current_vertex = self.edgeTo[current_vertex]
            return path