import React from "react";
import { Route, Routes, BrowserRouter as Router } from "react-router-dom";
import "./normalize.css";
import "./default.css";
import "./main.css";
import Navbar from "./components/Navbar";
import VisualizerFiles from "./components/visualizer/VisualizerFiles";
import VisualizerBoard from "./components/visualizer/VisualizerBoard";
// import Board from "./components/Board";
import ColourChooser from "./components/liveplay/ColourChooser";
import LiveBoard from "./components/liveplay/LiveBoard";

function App() {
  return (
    <>
      <Router>
        <Navbar />
        <Routes>
          {/* Page to visualize a game (by stepping through) given a filename */}
          <Route path="/vis/:file" element={<VisualizerBoard />} />
          {/* Page to list all available latest game files */}
          <Route path="/vis/gamechoice" element={<VisualizerFiles />} />
          {/* Page to choose a colour combination to start a live game */}
          <Route path="/choose_colour_live_play" element={<ColourChooser />} />
          {/* Page to play a live game after a combination is chosen */}
          <Route path="/live_play" element={<LiveBoard />} />
          {/* Default page routes to list all available game files */}
          <Route path="*" element={<VisualizerFiles />} />
        </Routes>
      </Router>
    </>
  );
}

export default App;
