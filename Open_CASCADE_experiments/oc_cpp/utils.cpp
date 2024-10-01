#include <BRepAlgoAPI_Fuse.hxx>

// Useful routines I generated, but I'm not using at the moment

// Function to fuse all shapes in a compound into a single shape
TopoDS_Shape FuseShapesInCompound(const TopoDS_Compound& compound) {
    // Check if the compound is empty
    TopExp_Explorer explorer;
    explorer.Init(compound, TopAbs_SHAPE);
    if (!explorer.More()) {
        std::cerr << "Error: Compound is empty." << std::endl;
        return TopoDS_Shape();  // Return a null shape if compound is empty
    }

    try {
        // Initialize the first shape to start the fusion
        TopoDS_Shape fusedShape = explorer.Current();
        explorer.Next();

        // Fuse all other shapes in the compound with the current fusedShape
        for (; explorer.More(); explorer.Next()) {
            TopoDS_Shape currentShape = explorer.Current();
            if (!currentShape.IsNull()) {
                BRepAlgoAPI_Fuse fuseOp(fusedShape, currentShape);
                fuseOp.Build();

                if (!fuseOp.IsDone()) {
                    std::cerr << "Error: Fusion operation failed." << std::endl;
                    throw Standard_Failure("Fusion operation failed.");
                }

                // Update fusedShape with the result of the fusion
                fusedShape = fuseOp.Shape();
            }
        }

        std::cout << "Fusion of all shapes in the compound completed successfully." << std::endl;
        return fusedShape;

    } catch (Standard_Failure& e) {
        std::cerr << "Error: " << e.GetMessageString() << std::endl;
        return TopoDS_Shape();  // Return a null shape on failure
    } catch (...) {
        std::cerr << "An unknown error occurred during the fusion." << std::endl;
        return TopoDS_Shape();  // Return a null shape on failure
    }
}

// Function to create a composite of rotated copies of a shape
TopoDS_Shape CreateRotatedComposite(const TopoDS_Shape& originalShape, Standard_Integer numCopies, Standard_Real angleOffset) {
    if (originalShape.IsNull()) {
        std::cerr << "Error: Original shape is null." << std::endl;
        return TopoDS_Shape();  // Return a null shape if the original is null
    }

    if (numCopies < 1) {
        std::cerr << "Error: Number of copies must be at least 1." << std::endl;
        return TopoDS_Shape();  // Return a null shape if the number of copies is invalid
    }

    try {
        // Create a compound to hold the copies
        TopoDS_Compound compound;
        BRep_Builder builder;
        builder.MakeCompound(compound);

        // Define the axis of rotation around the Z-axis
        gp_Ax1 zAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));

        for (Standard_Integer i = 0; i < numCopies; ++i) {
            // Calculate the rotation angle for this copy
            Standard_Real currentAngle = i * angleOffset;

            // Create a transformation for rotation about the Z-axis
            gp_Trsf rotation;
            rotation.SetRotation(zAxis, currentAngle);

            // Apply the transformation to the original shape
            TopoDS_Shape rotatedShape = BRepBuilderAPI_Transform(originalShape, rotation).Shape();

            // Add the rotated shape to the compound
            builder.Add(compound, rotatedShape);
        }

        std::cout << "Rotated composite created successfully with " << numCopies << " copies." << std::endl;
        return compound;

    } catch (Standard_Failure& e) {
        std::cerr << "Error: " << e.GetMessageString() << std::endl;
        return TopoDS_Shape();  // Return a null shape on failure
    } catch (...) {
        std::cerr << "An unknown error occurred during composite creation." << std::endl;
        return TopoDS_Shape();  // Return a null shape on failure
    }
}
