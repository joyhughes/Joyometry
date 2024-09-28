#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <V3d_View.hxx>
#include <WNT_Window.hxx>

int main(int argc, char* argv[]) {
    // Create a 3D box
    TopoDS_Shape box = BRepPrimAPI_MakeBox(100.0, 100.0, 100.0).Shape();

    // Set up OCCT's 3D visualization context
    Handle(V3d_Viewer) viewer = new V3d_Viewer(Graphic3d::InitGraphicDriver());
    Handle(AIS_InteractiveContext) context = new AIS_InteractiveContext(viewer);

    // Display the 3D box in the viewer
    Handle(AIS_Shape) aisShape = new AIS_Shape(box);
    context->Display(aisShape, true);

    // Visualization loop
    while (true) {
        // Handle user input and render the scene
    }

    return 0;
}
