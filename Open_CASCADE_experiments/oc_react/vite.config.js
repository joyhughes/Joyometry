import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import wasm from 'vite-plugin-wasm'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    react(),
    wasm()
  ],
  server: {
    open: true, // This will open the browser automatically
    hmr: {
      clientPort: 5173,  // Ensure this matches the port you're using
      protocol: 'ws',    // WebSocket protocol (default)
      host: 'localhost', // Ensure you're using localhost
    },
  },
  optimizeDeps: {
    exclude: ["opencascade.js/dist/opencascade.full.wasm"],
    include: ['file-loader'], 
  },
  /*
  build: {
    rollupOptions: {
      output: {
        manualChunks: {
          opencascade: ['opencascade.js'],  // Bundle OpenCascade separately
        },
      },
    },
  },
*/
})