graph_sqc_rad = 2.54; // MSRE (cm) unless otherwise noted
fuel_sqc_rad = 3.048;
pitch = 2 * fuel_sqc_rad;
num_cells = 23;
height = 162.56;

Point(1) = {-graph_sqc_rad, -graph_sqc_rad, 0, 1.0};
Point(2) = {graph_sqc_rad, -graph_sqc_rad, 0, 1.0};
Point(3) = {graph_sqc_rad, graph_sqc_rad, 0, 1.0};
Point(4) = {-graph_sqc_rad, graph_sqc_rad, 0, 1.0};
Line(1) = {4, 1};
Line(2) = {1, 2};
Line(3) = {3, 2};
Line(4) = {3, 4};
Point(5) = {-fuel_sqc_rad, -fuel_sqc_rad, 0, 1.0};
Point(6) = {-fuel_sqc_rad, fuel_sqc_rad, 0, 1.0};
Point(7) = {fuel_sqc_rad, fuel_sqc_rad, 0, 1.0};
Point(8) = {fuel_sqc_rad, -fuel_sqc_rad, 0, 1.0};
Line(5) = {6, 7};
Line(6) = {7, 8};
Line(7) = {8, 5};
Line(8) = {5, 6};
Line Loop(9) = {5, 6, 7, 8};
Line Loop(10) = {4, 1, 2, -3};
Plane Surface(11) = {9, 10};
Plane Surface(12) = {10};
fuel_surfaces[] = {11};
moder_surfaces[] = {12};
For xindex In {0:num_cells-1}
For yindex In {0:num_cells-1}
new_f_surface = Translate {xindex*pitch, yindex*pitch, 0} {
  Duplicata { Surface{11}; }
};
fuel_surfaces += new_f_surface;
new_m_surface = Translate {xindex*pitch, yindex*pitch, 0} {
  Duplicata { Surface{12}; }
};
moder_surfaces += new_m_surface;
// Printf("New f surface '%g'", new_f_surface);
// Printf("New m surface '%g'", new_m_surface);
EndFor
EndFor
For index In {0:num_cells*num_cells}
Extrude {0, 0, height} { Surface{fuel_surfaces[index]}; }
Extrude {0, 0, height} { Surface{moder_surfaces[index]}; }
EndFor
// Printf("Surfaces '%g' and '%g' and '%g'", fuel_surfaces[0], fuel_surfaces[1], fuel_surfaces[2]);
// Printf("Surfaces '%g' and '%g' and '%g'", moder_surfaces[0], moder_surfaces[1], moder_surfaces[2]);
