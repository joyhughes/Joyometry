
#include <gp_Pnt.hxx>
#include <gp_Circ.hxx>
#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <GeomFill_Trihedron.hxx>
#include <GC_MakeSegment.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepTools.hxx>
#include <StdFail_NotDone.hxx>
#include <StlAPI_Writer.hxx>
#include <iostream>

void SaveAsSTL(const TopoDS_Shape& shape, const std::string& filename) {
    if (shape.IsNull()) {
        std::cerr << "Error: Shape is null, cannot export to STL." << std::endl;
        return;
    }

    StlAPI_Writer stlWriter;
    if (stlWriter.Write(shape, filename.c_str())) {
        std::cout << "STL file written successfully to " << filename << std::endl;
    } else {
        std::cerr << "Error: Failed to write STL file." << std::endl;
    }
}

// Function to output a curve (e.g., BSpline) to a BREP file
void OutputCurveToBrepFile(const Handle(Geom_BSplineCurve)& curve, const std::string& filename) {
    // Convert the Geom_BSplineCurve to a TopoDS_Edge
    TopoDS_Edge edge = BRepBuilderAPI_MakeEdge(curve);

    // Write the edge to a BREP file
    BRepTools::Write(edge, filename.c_str());
}

// Function to output a solid (e.g., pipe) to a BREP file
void OutputShapeToBrepFile(const TopoDS_Shape& shape, const std::string& filename) {
    // Write the shape to a BREP file
    BRepTools::Write(shape, filename.c_str());
}

TopoDS_Shape CreatePipeAroundCurve(const Handle(Geom_Curve)& curve, Standard_Real pipeDiameter) {
    try {
        // Convert the Geom_Curve to a TopoDS_Edge (used as the spine for the pipe)
        TopoDS_Edge spineEdge = BRepBuilderAPI_MakeEdge(curve);

        // Convert the edge to a wire, as the pipe function requires a TopoDS_Wire
        TopoDS_Wire spineWire = BRepBuilderAPI_MakeWire(spineEdge);

        // Check if the spine wire is valid
        if (spineWire.IsNull()) {
            std::cerr << "Error: Spine wire is invalid." << std::endl;
            throw Standard_Failure("Spine wire creation failed.");
        }

        // Create a pipe shell
        BRepOffsetAPI_MakePipeShell pipeShell(spineWire);

        // Create the circular cross-section profile
        // Get the starting point and tangent direction of the curve
        Standard_Real firstParam = curve->FirstParameter();
        gp_Pnt startPoint;
        gp_Vec startTangent;
        curve->D1(firstParam, startPoint, startTangent);

        if (startTangent.Magnitude() == 0) {
            std::cerr << "Error: Start tangent is zero vector." << std::endl;
            throw Standard_Failure("Start tangent is zero.");
        }

        gp_Dir tangentDir(startTangent);

        // Define the circular cross-section of the pipe at the start point, normal to the curve
        gp_Ax2 crossSectionAxis(startPoint, tangentDir);

        Standard_Real radius = pipeDiameter / 2.0;

        // Create a circle as the cross-section
        Handle(Geom_Circle) circle = new Geom_Circle(crossSectionAxis, radius);
        TopoDS_Edge circleEdge = BRepBuilderAPI_MakeEdge(circle);
        TopoDS_Wire circleWire = BRepBuilderAPI_MakeWire(circleEdge);

        // Add the profile to the pipe shell
        pipeShell.Add(circleWire);

        // Indicate that the spine is closed
        pipeShell.SetMode(true); // Set the spine as closed

        // Build the pipe shell
        pipeShell.Build();

        if (!pipeShell.IsDone()) {
            std::cerr << "Error: Pipe shell creation failed." << std::endl;
            throw Standard_Failure("Pipe shell creation failed.");
        }

        // Close the shell to make it solid
        pipeShell.MakeSolid();

        // Return the resulting pipe shape
        TopoDS_Shape pipeShape = pipeShell.Shape();
        std::cout << "Pipe created successfully." << std::endl;
        return pipeShape;

    } catch (Standard_Failure& e) {
        std::cerr << "Error: " << e.GetMessageString() << std::endl;
        throw; // Re-throw the exception after logging
    } catch (...) {
        std::cerr << "An unknown error occurred during pipe creation." << std::endl;
        throw;
    }
}

Handle(Geom_BSplineCurve) CreateHelixCurve(Standard_Real circleDiameter, Standard_Real helixOffset, Standard_Integer helixRotations, Standard_Integer nPoints = 1000) {
    try {
        Standard_Real radius = circleDiameter / 2.0;  // Radius of the base circle
        Standard_Real totalAngle = 2 * M_PI;  // Total angle for the full number of rotations

        // Increase the number of points by 1 to ensure a periodic loop
        TColgp_Array1OfPnt points(1, nPoints + 1); // One extra point to close the loop
        Standard_Real deltaT = totalAngle / nPoints;

        for (Standard_Integer i = 1; i <= nPoints; i++) {
            Standard_Real t = deltaT * (i - 1);
            Standard_Real angleAroundCircle = t;  // Angle along the base circle

            // Helical rotation around the circle
            Standard_Real localHelixAngle = t * helixRotations;

            // Base coordinates for the circle
            Standard_Real baseX = radius * cos(angleAroundCircle);
            Standard_Real baseY = radius * sin(angleAroundCircle);

            // Helical offset added to the base circle's coordinates
            Standard_Real x = baseX + helixOffset * cos(localHelixAngle) * cos(angleAroundCircle);
            Standard_Real y = baseY + helixOffset * cos(localHelixAngle) * sin(angleAroundCircle);
            Standard_Real z = helixOffset * sin(localHelixAngle);  // Sinusoidal Z for the helix shape

            // Store the point
            points.SetValue(i, gp_Pnt(x, y, z));
        }

        // Ensure the last point is the same as the first to form a closed loop
        points.SetValue(nPoints + 1, points.Value(1));

        // Create the B-spline curve with periodicity
        GeomAPI_PointsToBSpline pointToBSpline(points, Approx_ChordLength, 3, 8, GeomAbs_C2, 1e-6);
        Handle(Geom_BSplineCurve) helixCurve = pointToBSpline.Curve();

        // Make the B-spline curve periodic
        helixCurve->SetPeriodic();

        // Success message after successful curve creation
        std::cout << "Periodic helix curve created successfully." << std::endl;

        return helixCurve;
    } catch (StdFail_NotDone& e) {
        std::cerr << "Error: B-Spline curve creation failed: " << e.GetMessageString() << std::endl;
        throw; // rethrow to ensure proper handling outside
    } catch (Standard_Failure& e) {
        std::cerr << "Error: " << e.GetMessageString() << std::endl;
        throw; // rethrow
    } catch (...) {
        std::cerr << "An unknown error occurred during curve creation." << std::endl;
        throw;
    }
}

int main() {
    // Example parameters
    Standard_Real circleDiameter = 100.0;
    Standard_Real helixOffset = 10.0;
    Standard_Integer helixRotations = 3;
    Standard_Integer nPoints = 1000;
    Standard_Real pipeDiameter = 5.0;  // Diameter of the pipe

    // Create the helix curve using the previously defined function
    Handle(Geom_BSplineCurve) helixCurve = CreateHelixCurve(circleDiameter, helixOffset, helixRotations, nPoints);

    // Create the pipe around the helix curve
    TopoDS_Shape pipeShape = CreatePipeAroundCurve(helixCurve, pipeDiameter);

    // Output the pipe geometry to a BREP file
    OutputShapeToBrepFile(pipeShape, "../results/pipe_helix.brep");
    SaveAsSTL(pipeShape, "../results/pipe_helix.stl");

    return 0;
}
