class DFS:
    def __init__(self, graph, starting_vertex):
        # Initialise marked and edgeTo arrays
        self.marked = []
        self.edgeTo = []
        for i in range(graph.V()):
            self.marked.append(False)
            self.edgeTo.append(None)

        # Initialise starting vertex
        self.starting_vertex = starting_vertex

        # Invoke DFS on starting vertex
        self.dfs(graph, starting_vertex)

    # Depth First Search function
    def dfs(self, graph, vertex):
        # Visit current vertex
        self.marked[vertex] = True

        # Visit all adjacent vertices connected to the current vertex
        for adj_vertex in graph.adj_list[vertex]:
            # Check if adjacent vertex is already visited
            if(not self.marked[adj_vertex]):
                # Visit adjacent vertex
                self.dfs(graph, adj_vertex)
                # Update edgeTo array
                self.edgeTo[adj_vertex] = vertex

    # Function to check if there exists a path from the starting vertex to the given vertex
    def hasPathTo(self, vertex):
        return self.marked[vertex]
    
    # Function to get the path to a given vertex from the starting vertex
    def pathTo(self, vertex):
        # Check if there exists a path to the given vertex
        if(not self.hasPathTo(vertex)):
            return None
        else:
            # Initialise path and current vertex
            path = []
            current_vertex = vertex

            # Work backwards to get path to starting vertex
            while(current_vertex != self.starting_vertex):
                current_vertex = self.edgeTo[current_vertex]
                if(current_vertex != None):
                    path.append(current_vertex) 

            # Return path
            return path