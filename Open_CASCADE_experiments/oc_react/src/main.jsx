import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import App from './App.jsx'
import './index.css'
/*import opencascade from "opencascade.js/dist/opencascade.full.js"
import opencascadeWasm from "opencascade.js/dist/opencascade.full.wasm?url"

const oc = await opencascade({
        locateFile: () => opencascadeWasm,
      })
*/
createRoot(document.getElementById('root')).render(
  <StrictMode>
    <App />
  </StrictMode>,
)
