import React, { useEffect, useState, useRef } from 'react';
import './App.css'
import "@google/model-viewer";
import { visualizeShapes } from "./visualize.jsx";
import opencascade from "opencascade.js/dist/opencascade.full.js"
import opencascadeWasm from "opencascade.js/dist/opencascade.full.wasm?url"

async function initOpenCascade() {
  const oc = await opencascade({
    locateFile: () => opencascadeWasm,
  });
  return oc;
}

function createCutShape(oc, sphereSize) {
  const box = new oc.BRepPrimAPI_MakeBox_2(1, 1, 1);
  const sphere = new oc.BRepPrimAPI_MakeSphere_5(new oc.gp_Pnt_3(0.5, 0.5, 0.5), sphereSize);
  const cut = new oc.BRepAlgoAPI_Cut_3(box.Shape(), sphere.Shape(), new oc.Message_ProgressRange_1());
  cut.Build(new oc.Message_ProgressRange_1());
  return cut.Shape();
}

function App() {
  const [modelUrl, setModelUrl] = useState('');
  const [sphereSize, setSphereSize] = useState(0.65);
  const [loading, setLoading] = useState(true); // Manage loading state
  const ocRef = useRef(null); // Use useRef to store OpenCascade instance

  // Initialize OpenCascade once on mount
  useEffect(() => {
    async function initializeOC() {
      const oc = await initOpenCascade();
      ocRef.current = oc; // Store OpenCascade instance in ref
      updateModel(oc, sphereSize); // Generate initial model
      setLoading(false); // Set loading to false when OpenCascade is ready
    }

    initializeOC();
  }, []); // Run only once on mount

  // Function to update the model
  const updateModel = (oc, sphereSize) => {
    const shape = createCutShape(oc, sphereSize);
    const modelUrl = visualizeShapes(oc, shape);
    setModelUrl(modelUrl);
  };

  // Recalculate model whenever sphereSize changes
  useEffect(() => {
    if (ocRef.current) {
      updateModel(ocRef.current, sphereSize); // Use the initialized OpenCascade instance
    }
  }, [sphereSize]);

  // Handle slider change
  const handleSliderChange = (event) => {
    setSphereSize(parseFloat(event.target.value));
  };

  return (
    <div style={{ textAlign: 'center' }}>
      {/* Show "Loading..." until OpenCascade is ready */}
      {loading ? (
        <p>Loading...</p>
      ) : (
        <>
          {/* The 3D model viewer */}
          <model-viewer src={modelUrl} style={{ width: '80vw', height: '80vh' }} camera-controls enable-pan />

          {/* The slider to adjust the sphere size */}
          <div style={{ marginTop: '20px' }}>
            <input 
              type="range" 
              min="0.5" 
              max="0.87" 
              step="0.01" 
              value={sphereSize} 
              onChange={handleSliderChange} 
              style={{ width: '50%' }} 
            />
            <p>Sphere Size: {sphereSize}</p>
          </div>
        </>
      )}
    </div>
  );
}

export default App;

/*import './App.css'
import "@google/model-viewer";
import { visualizeShapes } from "./visualize.jsx";
import { useState, useEffect } from 'react';
import opencascade from "opencascade.js/dist/opencascade.full.js"
import opencascadeWasm from "opencascade.js/dist/opencascade.full.wasm?url"

// Initialize OpenCascade (using async/await)
const oc = await opencascade({
  locateFile: () => opencascadeWasm,
});

// Function to create the cut shape
function createCutShape(oc, sphereSize) {
  const box = new oc.BRepPrimAPI_MakeBox_2(1, 1, 1);
  const sphere = new oc.BRepPrimAPI_MakeSphere_5(new oc.gp_Pnt_3(0.5, 0.5, 0.5), sphereSize);
  const cut = new oc.BRepAlgoAPI_Cut_3(box.Shape(), sphere.Shape(), new oc.Message_ProgressRange_1());
  cut.Build(new oc.Message_ProgressRange_1());
  return cut.Shape();
}

function App() {
  // State for the sphere size, with default value 0.65
  const [sphereSize, setSphereSize] = useState(0.65);

  // State for the model URL
  const [modelUrl, setModelUrl] = useState('');

  // Recalculate the model URL whenever sphereSize changes
  useEffect(() => {
    const myShape = createCutShape(oc, sphereSize);
    const newModelUrl = visualizeShapes(oc, myShape);
    setModelUrl(newModelUrl);
  }, [sphereSize]);

  // Handle slider change
  const handleSliderChange = (event) => {
    setSphereSize(parseFloat(event.target.value)); // Update sphere size state
  };

  return (
    <div style={{ textAlign: 'center' }}>
      <model-viewer src={modelUrl} style={{ width: '80vw', height: '80vh' }} camera-controls enable-pan />
      
      <div style={{ marginTop: '20px' }}>
        <input 
          type="range" 
          min="0.5" 
          max="0.87" 
          step="0.01" 
          value={sphereSize} 
          onChange={handleSliderChange} 
          style={{ width: '50%' }} 
        />
        <p>Sphere Size: {sphereSize}</p>
      </div>
    </div>
  );
}

export default App;
*/