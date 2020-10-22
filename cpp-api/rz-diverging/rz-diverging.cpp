#include <gmsh.h>
#include <set>
#include <iostream>

namespace factory = gmsh::model::geo;

int
main(int argc, char ** argv)
{
  gmsh::initialize();

  gmsh::model::add("channel");

  double large_rad = 2;
  double small_rad = 1;

  // The length over which we let flow fully develop
  double dev_l = 10;

  // Determinant of element size
  double lc = .1;

  // The axial length over which we adjust the channel radius
  double div_l = 1;

  // Set the points
  auto bottom_left = factory::addPoint(0, 0, 0, lc);
  auto bottom_right = factory::addPoint(small_rad, 0, 0, lc);
  auto end_entry_channel = factory::addPoint(small_rad, dev_l, 0, lc);
  auto end_div = factory::addPoint(large_rad, dev_l + div_l, 0, lc);
  auto top_right = factory::addPoint(large_rad, 2 * dev_l + div_l, 0, lc);
  auto top_left = factory::addPoint(0, 2 * dev_l + div_l, 0, lc);

  // Create the boundaries
  auto bottom = factory::addLine(bottom_left, bottom_right);

  auto right_entry = factory::addLine(bottom_right, end_entry_channel);
  auto right_div = factory::addLine(end_entry_channel, end_div);
  auto right_exit = factory::addLine(end_div, top_right);

  auto top = factory::addLine(top_right, top_left);

  auto left = factory::addLine(top_left, bottom_left);

  // Create the volume
  auto curve_loop = factory::addCurveLoop({bottom, right_entry, right_div, right_exit, top, left});
  auto volume = factory::addPlaneSurface({curve_loop});

  // Add all the boundary ids and names

  size_t boundary_id_counter = 100;

  auto bottom_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {bottom}, bottom_boundary_id);
  gmsh::model::setPhysicalName(1, bottom_boundary_id, "bottom");

  auto right_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {right_entry, right_div, right_exit}, right_boundary_id);
  gmsh::model::setPhysicalName(1, right_boundary_id, "right");

  auto top_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {top}, top_boundary_id);
  gmsh::model::setPhysicalName(1, top_boundary_id, "top");

  auto left_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {left}, left_boundary_id);
  gmsh::model::setPhysicalName(1, left_boundary_id, "left");

  // Add the subdomain ids and names

  size_t subdomain_id_counter = 10;

  auto volume_subdomain_id = subdomain_id_counter++;
  gmsh::model::addPhysicalGroup(2, {volume}, volume_subdomain_id);
  gmsh::model::setPhysicalName(2, volume_subdomain_id, "domain");

  factory::synchronize();

  gmsh::model::mesh::generate(2);

  gmsh::write("diverging.msh");

  gmsh::finalize();
  return 0;
}
