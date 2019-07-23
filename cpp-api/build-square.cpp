// This file reimplements gmsh/tutorial/t1.geo in C++.

// For all the elementary explanations about the general philosphy of entities
// in Gmsh, see the comments in the .geo file. Comments here focus on the
// specifics of the C++ API.

// The Gmsh API is entirely defined in the <gmsh.h> header. Read this file: it
// contains the documentation for all the functions in the API.
#include <gmsh.h>

namespace factory = gmsh::model::geo;

int main(int argc, char **argv)
{
  // Before using any functions in the C++ API, Gmsh must be initialized.
  gmsh::initialize();

  // By default Gmsh will not print out any messages: in order to output
  // messages on the terminal, just set the standard Gmsh option
  // "General.Terminal" (same format and meaning as in .geo files) using
  // gmsh::option::setNumber():
  gmsh::option::setNumber("General.Terminal", 1);

  // This adds a new model, named "t1". If gmsh::model::add() is not called, a
  // new default (unnamed) model will be created on the fly, if necessary.
  gmsh::model::add("square");

  // The C++ API provides direct access to the internal CAD kernels. The
  // built-in CAD kernel was used in t1.geo: the corresponding API functions
  // live in the "factory" namespace. To create geometrical points with
  // the built-in CAD kernel, one thus uses factory::addPoint():
  //
  // - the first 3 arguments are the point coordinates (x, y, z)
  //
  // - the next (optional) argument is the target mesh size close to the point
  //
  // - the last (optional) argument is the point tag
  double lc = 1e-1;
  factory::addPoint(0, 0, 0, lc, 1);
  factory::addPoint(1, 0,  0, lc, 2);
  factory::addPoint(1, 1, 0, lc, 3);
  factory::addPoint(0, 1, 0, lc, 4);

  // The API to create lines with the built-in kernel follows the same
  // conventions: the first 2 arguments are point tags, the last (optional one)
  // is the line tag.

  auto bottom = factory::addLine(1, 2, 1);
  auto right = factory::addLine(3, 2, 2);
  auto top = factory::addLine(3, 4, 3);
  auto left = factory::addLine(4, 1, 4);

  // The philosophy to construct curve loops and surfaces is similar: the first
  // argument is now a vector of integers.
  factory::addCurveLoop({4, 1, -2, 3}, 1);
  int domain = factory::addPlaneSurface({1}, 1);

  // Physical groups are defined by providing the dimension of the group (0 for
  // physical points, 1 for physical curves, 2 for physical surfaces and 3 for
  // phsyical volumes) followed by a vector of entity tags. The last (optional)
  // argument is the tag of the new group to create.

  // bottom
  gmsh::model::addPhysicalGroup(1, {1}, 101);
  gmsh::model::setPhysicalName(1, 101, "bottom");
  // right
  gmsh::model::addPhysicalGroup(1, {2}, 102);
  gmsh::model::setPhysicalName(1, 102, "right");
  // top
  gmsh::model::addPhysicalGroup(1, {3}, 103);
  gmsh::model::setPhysicalName(1, 103, "top");
  // left
  gmsh::model::addPhysicalGroup(1, {4}, 104);
  gmsh::model::setPhysicalName(1, 104, "left");

  // domain
  gmsh::model::addPhysicalGroup(2, {1}, 11);
  gmsh::model::setPhysicalName(2, 11, "domain");

  // Let's make this structured
  int n_elem =  10;
  auto n_nodes = n_elem + 1;

  factory::mesh::setTransfiniteCurve(bottom, n_nodes);
  factory::mesh::setTransfiniteCurve(right, n_nodes);
  factory::mesh::setTransfiniteCurve(top, n_nodes);
  factory::mesh::setTransfiniteCurve(left, n_nodes);
  factory::mesh::setTransfiniteSurface(domain);
  factory::mesh::setRecombine(2, domain);

  // Before it can be meshed, the internal CAD representation must be
  // synchronized with the Gmsh model, which will create the relevant Gmsh data
  // structures. This is achieved by the factory::synchronize() API
  // call for the built-in CAD kernel. Synchronizations can be called at any
  // time, but they involve a non trivial amount of processing; so while you
  // could synchronize the internal CAD data after every CAD command, it is
  // usually better to minimize the number of synchronization points.
  factory::synchronize();

  // We can then generate a 2D mesh...
  gmsh::model::mesh::generate(2);

  // ... and save it to disk
  gmsh::write("square.msh");

  // This should be called at the end:
  gmsh::finalize();
  return 0;
}
