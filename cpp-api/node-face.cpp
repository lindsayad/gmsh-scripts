#include <gmsh.h>
#include <set>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

namespace factory = gmsh::model::geo;

void generateMesh(std::size_t multiplier)
{
  gmsh::model::add("node-face-" + std::to_string(multiplier));

  double length = 1;

  auto bottom_left_point = factory::addPoint(0, 0, 0);
  auto bottom_right_point = factory::addPoint(length, 0, 0);
  auto top_right_point = factory::addPoint(length, length, 0);
  auto top_left_point = factory::addPoint(0, length, 0);
  auto bottom = factory::addPoint(length / 2, 0, 0);
  auto top = factory::addPoint(length / 2, length, 0);

  // Generate the two volume domains

  auto bottom_left = factory::addLine(bottom_left_point, bottom);
  auto left_interface = factory::addLine(bottom, top);
  auto top_left = factory::addLine(top, top_left_point);
  auto left = factory::addLine(top_left_point, bottom_left_point);

  auto right = factory::addLine(bottom_right_point, top_right_point);
  auto top_right = factory::addLine(top_right_point, top);
  auto right_interface = factory::addLine(top, bottom);
  auto bottom_right = factory::addLine(bottom, bottom_right_point);

  auto left_curve_loop = factory::addCurveLoop({bottom_left, left_interface, top_left, left});
  auto right_curve_loop = factory::addCurveLoop({right, top_right, right_interface, bottom_right});

  auto left_surface = factory::addPlaneSurface({left_curve_loop});
  auto right_surface = factory::addPlaneSurface({right_curve_loop});

  // Now generate the mortar subdomain
  auto elems_on_left = 6 * multiplier, elems_on_right = 4 * multiplier;

  auto nodes_on_left = elems_on_left + 1, nodes_on_right = elems_on_right + 1;
  std::set<double> ypoints;
  for (decltype(nodes_on_left) i = 0; i < nodes_on_left; ++i)
    ypoints.insert(i * length / (nodes_on_left - 1));
  for (decltype(nodes_on_right) i = 0; i < nodes_on_right; ++i)
    ypoints.insert(i * length / (nodes_on_right - 1));

  std::vector<int> mortar_points(ypoints.size());
  std::size_t counter = 0;
  for (const auto & element : ypoints)
    mortar_points[counter++] = factory::addPoint(length / 2, element, 0);

  // std::vector<int> mortar_interface = {factory::addSpline(mortar_points)};
  std::vector<int> mortar_interface(mortar_points.size() - 1);
  for (std::size_t i = 1; i < mortar_points.size(); ++i)
    mortar_interface[i - 1] = factory::addLine(mortar_points[i - 1], mortar_points[i]);

  // Add all the boundary ids and names

  std::size_t boundary_id_counter = 100;

  auto bottom_left_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {bottom_left}, bottom_left_boundary_id);
  gmsh::model::setPhysicalName(1, bottom_left_boundary_id, "bottom_left");

  auto bottom_right_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {bottom_right}, bottom_right_boundary_id);
  gmsh::model::setPhysicalName(1, bottom_right_boundary_id, "bottom_right");

  auto right_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {right}, right_boundary_id);
  gmsh::model::setPhysicalName(1, right_boundary_id, "right");

  auto top_right_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {top_right}, top_right_boundary_id);
  gmsh::model::setPhysicalName(1, top_right_boundary_id, "top_right");

  auto top_left_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {top_left}, top_left_boundary_id);
  gmsh::model::setPhysicalName(1, top_left_boundary_id, "top_left");

  auto left_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {left}, left_boundary_id);
  gmsh::model::setPhysicalName(1, left_boundary_id, "left");

  auto left_interface_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {left_interface}, left_interface_boundary_id);
  gmsh::model::setPhysicalName(1, left_interface_boundary_id, "left_interface");
  std::cout << "left interface boundary id " << left_interface_boundary_id << std::endl;

  auto right_interface_boundary_id = boundary_id_counter++;
  gmsh::model::addPhysicalGroup(1, {right_interface}, right_interface_boundary_id);
  gmsh::model::setPhysicalName(1, right_interface_boundary_id, "right_interface");
  std::cout << "right interface boundary id " << right_interface_boundary_id << std::endl;

  std::size_t subdomain_id_counter = 10;

  auto volume_subdomain_id = subdomain_id_counter++;
  gmsh::model::addPhysicalGroup(2, {left_surface, right_surface}, volume_subdomain_id);
  gmsh::model::setPhysicalName(2, volume_subdomain_id, "domain");

  factory::mesh::setTransfiniteCurve(bottom_left, elems_on_left / 2 + 1);
  factory::mesh::setTransfiniteCurve(bottom_right, elems_on_right / 2 + 1);
  factory::mesh::setTransfiniteCurve(top_left, elems_on_left / 2 + 1);
  factory::mesh::setTransfiniteCurve(top_right, elems_on_right / 2 + 1);

  factory::mesh::setTransfiniteCurve(left, nodes_on_left);
  factory::mesh::setTransfiniteCurve(left_interface, nodes_on_left);

  factory::mesh::setTransfiniteCurve(right, nodes_on_right);
  factory::mesh::setTransfiniteCurve(right_interface, nodes_on_right);

  factory::mesh::setTransfiniteSurface(left_surface);
  factory::mesh::setRecombine(2, left_surface);

  factory::mesh::setTransfiniteSurface(right_surface);
  factory::mesh::setRecombine(2, right_surface);

  factory::synchronize();

  gmsh::model::mesh::generate(2);

  gmsh::write("node-face-" + std::to_string(elems_on_left) + ".msh");
}

std::size_t mult_func()
{
  static std::size_t i = 1;
  std::size_t temp = i;
  i *= 2;
  return temp;
}

int main(int argc, char **argv)
{
  gmsh::initialize();

  gmsh::option::setNumber("General.Terminal", 1);

  std::vector<std::size_t> multipliers(4);
  std::generate(multipliers.begin(), multipliers.end(), mult_func);

  for (auto multiplier : multipliers)
    generateMesh(multiplier);

  gmsh::finalize();
  return 0;
}
