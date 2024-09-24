#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/triangulate_hole.h>
#include <CGAL/mesh_segmentation.h>
#include <CGAL/property_map.h>
#include <CGAL/Surface_mesh/IO/PLY.h>

#include <iostream>
#include <fstream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Surface_mesh<Point> Surface_mesh;
typedef boost::graph_traits<Surface_mesh>::face_descriptor face_descriptor;
typedef boost::graph_traits<Surface_mesh>::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits<Surface_mesh>::halfedge_descriptor halfedge_descriptor;

namespace PMP = CGAL::Polygon_mesh_processing;

// Function to scale a surface mesh by a factor
void scale_surface_mesh(Surface_mesh& mesh, double scale_factor) {
    for (auto v : mesh.vertices()) {
        Point& p = mesh.point(v);
        mesh.point(v) = Point(p.x() * scale_factor, p.y() * scale_factor, p.z() * scale_factor);
    }
    std::cout << "Surface mesh scaled by a factor of " << scale_factor << std::endl;
}

// Function to fill holes in a surface mesh
void fill_holes(Surface_mesh& mesh) {
    std::size_t nb_holes = 0;
    for (halfedge_descriptor h : halfedges(mesh)) {
        if (mesh.is_border(h)) {  // Check if the halfedge is part of a hole
            ++nb_holes;
            PMP::triangulate_hole(mesh, h);
        }
    }
    std::cout << "Number of holes filled: " << nb_holes << std::endl;
}

// Function to calculate and color wall thickness in a surface mesh
void wall_thickness(Surface_mesh& mesh, double thickness_threshold) {
    // Create a property map for wall thickness
    typedef std::map<face_descriptor, double> Face_double_map;
    Face_double_map internal_map;
    boost::associative_property_map<Face_double_map> sdf_property_map(internal_map);

    // Compute SDF values (wall thickness estimation)
    std::pair<double, double> min_max_sdf = CGAL::sdf_values(mesh, sdf_property_map);

    std::cout << "Minimum wall thickness: " << min_max_sdf.first << std::endl;
    std::cout << "Maximum wall thickness: " << min_max_sdf.second << std::endl;

    // Create a property map for vertex colors
    auto vertex_color_map = mesh.add_property_map<vertex_descriptor, CGAL::Color>("v:color", CGAL::Color(255, 255, 255)).first;  // Default color is white

    // Iterate over the faces and apply color to vertices if adjacent face is below threshold
    for (face_descriptor f : faces(mesh)) {
        double thickness_value = sdf_property_map[f] * (min_max_sdf.second - min_max_sdf.first) + min_max_sdf.first;
        
        // Check if thickness is below the threshold
        if (thickness_value < thickness_threshold) {
            // Iterate over the vertices of the face and color them red
            for (vertex_descriptor v : vertices_around_face(mesh.halfedge(f), mesh)) {
                vertex_color_map[v] = CGAL::Color(255, 0, 0);  // Color red
            }
        }
    }

    std::cout << "Vertex coloring done based on wall thickness threshold of " << thickness_threshold << std::endl;
}

// Function to write a surface mesh to STL
void write_surface_mesh_to_stl(const Surface_mesh& mesh, const std::string& filename) {
    std::ofstream output(filename);
    if (!output) {
        std::cerr << "Error: Cannot open output file " << filename << std::endl;
        return;
    }

    // STL header
    output << "solid CGAL_surface_mesh\n";

    // Loop through each facet in the surface mesh
    for (face_descriptor f : faces(mesh)) {
        auto h = halfedge(f, mesh);
        Point p1 = mesh.point(target(h, mesh));
        Point p2 = mesh.point(target(next(h, mesh), mesh));
        Point p3 = mesh.point(target(next(next(h, mesh), mesh), mesh));

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
    output << "endsolid CGAL_surface_mesh\n";
    output.close();
    std::cout << "Surface mesh written to " << filename << " as an STL file.\n";
}

// Function to write a surface mesh to OFF
void write_surface_mesh_to_off(const Surface_mesh& mesh, const std::string& filename) {
    std::ofstream output(filename);
    output << mesh;  // Write the surface mesh to the file
    output.close();
}

// Function to read a surface mesh from OFF
void read_surface_mesh_from_off(Surface_mesh& mesh, const std::string& filename) {
    std::ifstream input(filename);
    if (!input || !(input >> mesh) || mesh.is_empty() || (!CGAL::is_triangle_mesh(mesh))) {
        std::cerr << "Input is not a triangle mesh" << std::endl;
        exit(0);
    }
}

void write_surface_mesh_to_ply(const Surface_mesh& mesh, const std::string& filename) {
    // Save to PLY format (which supports vertex colors)
    std::ofstream output(filename);
    CGAL::IO::write_PLY(output, mesh);
}

int main() {
    // Create and read Surface_mesh
    Surface_mesh mesh;

    // Read the surface mesh from an input OFF file
    read_surface_mesh_from_off(mesh, "models/big_bunny_repaired.off");
    scale_surface_mesh(mesh, 0.25);
    wall_thickness(mesh, 5.0);
    write_surface_mesh_to_ply(mesh, "models/bunny_thick.ply");

    return EXIT_SUCCESS;
}

