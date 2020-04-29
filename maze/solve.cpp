#include "maze.h"
#include "path.h"
#include<queue>
#include<vector>
#include<list>
#include<tuple>
#include<utility>
#include<iostream>
#include<climits>
#include<sstream>
using namespace std;

path solve_dfs(Maze& m, int rows, int cols);
path solve_bfs(Maze& m, int rows, int cols);
path solve_dijkstra(Maze& m, int rows, int cols);
path solve_tour(Maze& m, int rows, int cols);


int main(int argc, char** argv)
{
    if(argc != 4)
    {
        cerr << "usage:\n"
             << "./maze option rows cols\n"
             << " options:\n"
             << "  -dfs: depth first search (backtracking)\n"
             << "  -bfs: breadth first search\n"
             << "  -dij: dijkstra's algorithm\n"
             << "  -tour: all corners tour\n"
             << "  -basic: run dfs, bfs, and dij\n"
             << "  -advanced: run dfs, bfs, dij and tour" << endl;
        return 0;
    }
    string opt(argv[1]);

    int rows, cols;
    stringstream s;
    s << argv[2] << " " << argv[3];
    s >> rows >> cols;

    // construct a new random maze;
    Maze m(rows, cols);

    // print the initial maze out
    cout << "Initial maze" << endl;
    m.print_maze(cout, opt == "-dij" || opt == "-tour");

    if(opt == "-dfs")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }


    if(opt == "-bfs")
    {
        cout << "\nSolved bfs" << endl;
        path p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);
    }

    if(opt == "-dij")
    {
        cout << "\nSolved dijkstra" << endl;
        path p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }

    if(opt == "-tour")
    {
        cout << "\nSolved all courners tour" << endl;
        path p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
    if(opt == "-basic")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);
    }
    if(opt == "-advanced")
    {
        cout << "\nSolved dfs" << endl;
        path p = solve_dfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved bfs" << endl;
        p = solve_bfs(m, rows, cols);
        m.print_maze_with_path(cout, p, false, false);

        cout << "\nSolved dijkstra" << endl;
        p = solve_dijkstra(m, rows, cols);
        m.print_maze_with_path(cout, p, true, false);

        cout << "\nSolved all courners tour" << endl;
        p = solve_tour(m, rows, cols);
        m.print_maze_with_path(cout, p, true, true);
    }
}

////////////////////////////////////////////////////////////////////////
//
// Base search class
//
// This is the base class. It provides a basic form for the search
// algorithms. Goal is the destination point. Course represents all the
// points that have been searched. CourseCK() checks if a point has
// already been passed through.
//
////////////////////////////////////////////////////////////////////////


class Search {
protected:
  Maze* obstacle;
  point goal;
  path course;
  bool CourseCK(point, int dir);
  virtual path TestSquare(point cur) = 0;
public:
  Search(Maze &m, int c, int r): obstacle(&m) {
    goal = make_pair(r - 1, c - 1);
  }

  Search() = default;

  virtual path Start() = 0;
};

bool Search::CourseCK(point cur, int dir) {
  point p = cur + moveIn(dir);
  for(auto k : course)
    if(p == k)
      return true;
  return false;
}

////////////////////////////////////////////////////////////////////////
//
// Depth first
//
// Depth first search class is a search class. This algorithm
// recursively calls searches all the way till all paths have been
// established. On the return paths are compared to check for the
// shortest.
//
////////////////////////////////////////////////////////////////////////

class DFS_model: private Search {
private:
  path DFS_Search(point cur);
  path TestSquare(point cur) override;
public:
  DFS_model() = default;
  DFS_model(Maze& m, int rows, int cols): Search(m, rows, cols) {}
  path Start() override;

};

path solve_dfs(Maze& m, int rows, int cols)
{
  DFS_model solve(m, rows, cols);
  return solve.Start();
}

path DFS_model::Start() {
  return DFS_Search(make_pair(0, 0));
}

path DFS_model::DFS_Search(point cur) {
  if(cur == goal)
    return path(1, cur);
  course.push_front(cur);
  path p[4], ret;

  // initiate a new recursive search for each direction
  for(int k = 0; k < 4; ++k)
    if(obstacle->can_go(k, cur.first, cur.second))
      if(!CourseCK(cur, k))
        p[k] = DFS_Search(cur + moveIn(k));

  // compare all paths for once that ended in the goal and shortest
  for(auto & k : p)
    if(k.back() == goal)
      if(ret.empty() || ret.size() > k.size())
        ret = k;

  course.pop_front();
  ret.push_front(cur);
  return ret;
}

path DFS_model::TestSquare(point cur) {
  return path();
}

////////////////////////////////////////////////////////////////////////
//
// Breadth first
//
// Breadth first search checks each point as it moves out from the
// origin. The first path to reach the goal is returned. The end of each
// route will be tested for the next point and forks will spawn a second
// route.
//
////////////////////////////////////////////////////////////////////////

class BFS_model: private Search {
private:
  path TestSquare(point cur) override;
  path BFS_Search();
  list<path> routes;
public:
  BFS_model() = default;
  BFS_model(Maze& m, int rows, int cols): Search(m, rows, cols) {}
  path Start() override;
};

path solve_bfs(Maze& m, int rows, int cols)
{
  BFS_model solve(m, rows, cols);
  return solve.Start();
}

path BFS_model::Start() {
  return BFS_Search();
}

path BFS_model::BFS_Search(){
  course.push_front(make_pair(0,0));
  routes.push_front(course);
  path tmp;                   // When a path forks tmp is all the
                              // points used to affix the end
                              // of each path.
  list<path> pile;            // All the paths get thrown into pile.

  while(true) {

    for(auto k : routes) {
      tmp = TestSquare(k.back());
      for(auto t : tmp) {
        pile.push_back(k);
        pile.back().push_back(t);
      }
      tmp.clear();
    }

    // After all the routes have been built and collected they get
    // put back into the routes variable.
    routes = pile;
    pile.clear();

    // All routes are checked for the goal, then start all over again.
    for (auto k : routes)
      if (k.back() == goal)
        return k;
  }
}

// tests a point for successive points to add to routes
path BFS_model::TestSquare(point cur) {
  course.push_back(cur);
  path tmp;

  for (int k = 0; k < 4; ++k)
    if (obstacle->can_go(k, cur.first, cur.second))
      if (!CourseCK(cur, k))
        tmp.push_back(cur + moveIn(k));


  return tmp;
}

////////////////////////////////////////////////////////////////////////
//
// Dijkstra's algorithm searches through all the connected points for
// the least costly path to connect. This implementation spiders out
// till it finds the goal and returns the first path to make that
// connection.
//
////////////////////////////////////////////////////////////////////////

class DJK_model: private Search {
private:
  path DJK_Search();
  path TestSquare(point cur) override;
  struct WeightedPath {
    int Weight;
    point Source;
    point Dest;
    WeightedPath(int w, point s, point d): Weight(w),
      Source(s), Dest(d) {}
  };
  list<WeightedPath> cue;
  list<path> graph;
  point attach();
  static bool CompWP(WeightedPath first , WeightedPath second);
  static point MoveTo(point p, int dir);
  int Messure(point p, int dir);
public:
  DJK_model() = default;
  DJK_model(Maze& m, int rows, int cols): Search(m, rows, cols) {}
  path Start() override;
};

// CompWP compares the weight of an edge for the swap function.
// The basis for this implemntation can be found at:
// http://www.cplusplus.com/reference/list/list/swap/
bool DJK_model::CompWP(WeightedPath first , WeightedPath second) {
  return (first.Weight < second.Weight);
}


path solve_dijkstra(Maze& m, int rows, int cols)
{
  DJK_model solve(m, rows, cols);
  return solve.Start();
}

path DJK_model::Start() {
  return DJK_Search();
}

// The DJK_Search will keep looping attempting to attach new points
// until it finds the goal. cur represents the must recent eadge added,
// and graph is the list of all the known paths.
path DJK_model::DJK_Search() {
  point cur(0, 0);
  course.push_front(cur);
  graph.push_front(course);

  while(true) {
    TestSquare(cur);

    cur = attach();

    for(auto k: graph)
      if(k.back() == goal)
        return k;
  }
}

// like with other usese of this function it searches for new points to ad to the
// path, but in this case it ads new ones to the cue and sorts the cue by weight
path DJK_model::TestSquare(point cur) {
  course.push_back(cur);
  course.unique();

  for (int k = 0; k < 4; ++k)
    if (obstacle->can_go(k, cur.first, cur.second))
      if (!CourseCK(cur, k))
        cue.emplace_back(Messure(cur,k),
            cur, MoveTo(cur,k));

  cue.sort(CompWP);
  return path();
}

// Attaches new points the the graph. It creats forks a path if in the middle
// other wise it just exstend the end of a known path. There was some trouble
// becuase the new paths were not being attched to the graph. That may have
// been the reason moveIn() was giving weird resuslts. 
point DJK_model::attach() {
  int mk;
  path tmp;
  point ret;
  for(auto Paths: graph) {
    mk = 0;
    for(auto Points: Paths) {
      mk++;
      if(cue.front().Source == Points) {
	tmp = Paths;
	if(cue.front().Source == Paths.back())
	  graph.remove(Paths);
	tmp.resize(mk);
	tmp.push_back(cue.front().Dest);
	graph.push_back(tmp);
	ret = cue.front().Dest;
	cue.pop_front();
	return ret;
        }
    }
  }
  return point();
}


// This was added because moveIn keept retuning weird values
point DJK_model::MoveTo(point p, int dir) {
  switch(dir)
  {
    case UP:    return make_pair(p.first - 1, p.second);
    case LEFT:  return make_pair(p.first,p.second - 1);
    case DOWN:  return make_pair(p.first + 1,p.second);
    case RIGHT: return make_pair(p.first,p.second + 1);
  }
  return make_pair(-1,-1);
}

// This was added because moveIn keept retuning weird values
int DJK_model::Messure(point p, int dir) {
  return obstacle->cost(p, dir);
}

path solve_tour(Maze& m, int rows, int cols)
{
    return list<point>();
}
