#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepTools.hxx>
#include <TopoDS_Shape.hxx>

int main() {
    // Create a simple box of dimensions 200x150x100
    TopoDS_Shape box = BRepPrimAPI_MakeBox(200.0, 150.0, 100.0).Shape();
    
    // Export the box to a BREP file
    BRepTools::Write(box, "box.brep");
    
    return 0;
}
