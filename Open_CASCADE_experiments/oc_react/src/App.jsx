import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import "@google/model-viewer";
import { visualizeShapes } from "./visualize.jsx";
import opencascade from "opencascade.js/dist/opencascade.full.js"
import opencascadeWasm from "opencascade.js/dist/opencascade.full.wasm?url"

const oc = await opencascade({
        locateFile: () => opencascadeWasm,
      })

function createCutShape( oc, sphereSize ) {
  const box = new oc.BRepPrimAPI_MakeBox_2(1, 1, 1);
  const sphere = new oc.BRepPrimAPI_MakeSphere_5(new oc.gp_Pnt_3(0.5, 0.5, 0.5), sphereSize);
  const cut = new oc.BRepAlgoAPI_Cut_3(box.Shape(), sphere.Shape(), new oc.Message_ProgressRange_1());
  cut.Build(new oc.Message_ProgressRange_1());
  return cut.Shape();
}
const myShape = createCutShape(oc, 0.65);
const modelUrl = visualizeShapes(oc, myShape);

function App() {
  return (
    // Now we can simply use the URL with model-viewer.
    <model-viewer src={modelUrl} camera-controls enable-pan />
  );
}

/*
function App() {
  const [count, setCount] = useState(0)

  return (
    <>
      <div>
        <a href="https://vitejs.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>Vite + React</h1>
      <div className="card">
        <button onClick={() => setCount((count) => count + 1)}>
          count is {count}
        </button>
        <p>
          Edit <code>src/App.jsx</code> and save to test HMR
        </p>
      </div>
      <p className="read-the-docs">
        Click on the Vite and React logos to learn more
      </p>
    </>
  )
}
  */

export default App
