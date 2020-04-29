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
  // These place holders could probably be removed if I Have time.
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
// Dijkstra's algorithm
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
public:
  DJK_model() = default;
  DJK_model(Maze& m, int rows, int cols): Search(m, rows, cols) {}
  path Start() override;
};

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

path DJK_model::TestSquare(point cur) {
  course.push_back(cur);
  course.unique();

  for (int k = 0; k < 4; ++k)
    if (obstacle->can_go(k, cur.first, cur.second))
      if (!CourseCK(cur, k))
        cue.emplace_back(obstacle->cost(cur,k),
            cur, cur + moveIn(k));

  cue.sort(CompWP);
  return path();
}

point DJK_model::attach() {
  int mk;
  path tmp;
  point ret;
  for(auto Paths: graph) {
    mk = 0;
    for(auto Points: Paths) {
      mk++;
      if(cue.front().Source == Points)
        if(Points == Paths.back()) {
          Paths.push_back(cue.front().Dest);
          ret = cue.front().Dest;
          cue.pop_front();
          return ret;
        }
        else {
          tmp = Paths;
          tmp.resize(mk);
          tmp.push_back(cue.front().Dest);
          ret = cue.front().Dest;
          cue.pop_front();
          return ret;
        }
    }
  }
  return point();
}

path solve_tour(Maze& m, int rows, int cols)
{
    return list<point>();
}
