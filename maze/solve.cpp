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


class Search {
protected:
  Maze* obstacle;
  int rows;
  int cols;
  point goal;
  path course;
  bool CourseCK(point, int dir);
public:
  Search(Maze &m, int c, int r): obstacle(&m), rows(r), cols(c) {
    goal = make_pair(r - 1, c - 1);
  }

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
////////////////////////////////////////////////////////////////////////

class DFS_model: private Search {
private:
  path DFS_TestSquare(point cur);
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
  return DFS_TestSquare(make_pair(0,0));
}

path DFS_model::DFS_TestSquare(point cur) {
  if(cur == goal)
    return path(1, cur);
  course.push_front(cur);
  path p[4], ret;

  //
  for(int k = 0; k < 4; ++k)
    if(obstacle->can_go(k, cur.first, cur.second))
      if(!CourseCK(cur, k))
        p[k] = DFS_TestSquare(cur + moveIn(k));

  //
  for(auto & k : p)
    if(k.back() == goal)
      if(ret.empty() || ret.size() > k.size())
        ret = k;

  course.pop_front();
  ret.push_front(cur);
  return ret;
}

////////////////////////////////////////////////////////////////////////
//
// Breadth first
//
////////////////////////////////////////////////////////////////////////

class BFS_model: private Search {
private:
  path BFS_TestSquare(point cur);
  path Found(point);
public:
  BFS_model() = default;
  BFS_model(Maze& m, int rows, int cols): Search(m, rows, cols) {}
  path Start();
};

path solve_bfs(Maze& m, int rows, int cols)
{
  BFS_model solve(m, rows, cols);
  return solve.Start();
}

path BFS_model::Start() {
  return BFS_TestSquare(make_pair(0,0));
}

path BFS_model::BFS_TestSquare(point cur) {

  for(int k = 0; k < 4; ++k)
    if(obstacle->can_go(k, cur.first, cur.second))
      Found(cur + moveIn(k));


}

////////////////////////////////////////////////////////////////////////
//
// Dijkstra's algorithm
//
////////////////////////////////////////////////////////////////////////

path solve_dijkstra(Maze& m, int rows, int cols)
{
    return list<point>();
}

path solve_tour(Maze& m, int rows, int cols)
{
    return list<point>();
}


