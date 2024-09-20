#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/property_map.h>

#include <iostream>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef boost::graph_traits<Polyhedron>::face_descriptor face_descriptor;
typedef Polyhedron::Halfedge_handle Halfedge_handle;

namespace PMP = CGAL::Polygon_mesh_processing;

// Function to scale a polyhedron by a factor
void scale_polyhedron(Polyhedron& P, double scale_factor) {
    for (auto vit = P.points_begin(); vit != P.points_end(); ++vit) {
        *vit = Point(vit->x() * scale_factor, vit->y() * scale_factor, vit->z() * scale_factor);
    }
    std::cout << "Polyhedron scaled by a factor of " << scale_factor << std::endl;
}

// Function to fill holes in a polyhedron
void fill_holes(Polyhedron& P) {
  // Count the number of holes and fill them
  std::size_t nb_holes = 0;
  for (Halfedge_handle h : halfedges(P)) {
      if (h->is_border()) {  // Check if the halfedge is part of a hole
          ++nb_holes;
          PMP::triangulate_hole(P, h);
      }
  }
  std::cout << "Number of holes filled: " << nb_holes << std::endl;
}

void wall_thickness(Polyhedron& P) {
  
  // create a property-map
  typedef std::map<face_descriptor, double> Face_double_map;
  Face_double_map internal_map;
  boost::associative_property_map<Face_double_map> sdf_property_map(internal_map);

  // compute SDF values
  std::pair<double, double> min_max_sdf = CGAL::sdf_values(P, sdf_property_map);

  // It is possible to compute the raw SDF values and post-process them using
  // the following lines:
  // const std::size_t number_of_rays = 25;  // cast 25 rays per face
  // const double cone_angle = 2.0 / 3.0 * CGAL_PI; // set cone opening-angle
  // CGAL::sdf_values(P, sdf_property_map, cone_angle, number_of_rays, false);
  // std::pair<double, double> min_max_sdf =
  //  CGAL::sdf_values_postprocessing(P, sdf_property_map);

  // print minimum & maximum SDF values
  //std::cout << "minimum SDF: " << min_max_sdf.first
  //          << " maximum SDF: " << min_max_sdf.second << std::endl;

  std::cout << "minimum wall thickness: " << min_max_sdf.first << std::endl;

  // print SDF values
  //for(face_descriptor f : faces(P))
  //    std::cout << sdf_property_map[f] << " ";

  std::cout << std::endl;
}

void write_polyhedron_to_stl(const Polyhedron& P, const std::string& filename) {
    std::ofstream output(filename);
    if (!output) {
        std::cerr << "Error: Cannot open output file " << filename << std::endl;
        return;
    }

    // STL header
    output << "solid CGAL_polyhedron\n";

    // Loop through each facet in the polyhedron
    for (auto facet_it = P.facets_begin(); facet_it != P.facets_end(); ++facet_it) {
        // Get the normal of the facet (for STL format, which requires normals)
        auto h = facet_it->halfedge();
        Point p1 = h->vertex()->point();
        Point p2 = h->next()->vertex()->point();
        Point p3 = h->next()->next()->vertex()->point();

        // Compute the normal vector for the current facet
        Kernel::Vector_3 normal = CGAL::normal(p1, p2, p3);

        // Write the normal and the facet's vertices in STL format
        output << "  facet normal " << normal.x() << " " << normal.y() << " " << normal.z() << "\n";
        output << "    outer loop\n";
        output << "      vertex " << p1.x() << " " << p1.y() << " " << p1.z() << "\n";
        output << "      vertex " << p2.x() << " " << p2.y() << " " << p2.z() << "\n";
        output << "      vertex " << p3.x() << " " << p3.y() << " " << p3.z() << "\n";
        output << "    endloop\n";
        output << "  endfacet\n";
    }

    // STL footer
    output << "endsolid CGAL_polyhedron\n";
    output.close();
    std::cout << "Polyhedron written to " << filename << " as an STL file.\n";
}

void write_polyhedron_to_off(const Polyhedron& P, const std::string& filename) {
  std::ofstream output(filename);
  output << P;  // Write the polyhedron to the file
  output.close();
}

void read_polyhedron_from_off(Polyhedron& P, const std::string& filename) {
  std::ifstream input(filename);
  if ( !input || !(input >> P) || P.empty() || ( !CGAL::is_triangle_mesh(P)) )
  {
    std::cerr << "Input is not a triangle mesh" << std::endl;
    exit(0);
  }
}

int main()
{
  // create and read Polyhedron
  Polyhedron P;

  // Read the polyhedron from an input OFF file
  read_polyhedron_from_off(P, "models/big_bunny_repaired.off");
  scale_polyhedron(P, 0.5);
  write_polyhedron_to_stl(P, "models/medium_bunny.stl");
  write_polyhedron_to_off(P, "models/medium_bunny.off");

  return EXIT_SUCCESS;
}
