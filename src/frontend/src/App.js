// import React, { useState } from "react";
import React from "react";
import { Route, Routes, BrowserRouter as Router } from "react-router-dom";
import "./normalize.css";
import "./default.css";
import "./main.css";
import Navbar from "./components/Navbar";
import VisualizerFiles from "./components/visualizer/VisualizerFiles";
import VisualizerBoard from "./components/visualizer/VisualizerBoard";
// import Board from "./components/Board";
// import ColourChooser from "./components/liveplay/ColourChooser";
// import LiveBoard from "./components/liveplay/LiveBoard";

// import { GrpcWebClientBase } from "grpc-web";
// import { GreeterClient } from "./generated/service_grpc_web_pb";
// import { HelloRequest } from "./generated/service_pb";

// const client = new GreeterClient("http://localhost:8080"); // Envoy proxy URL

function App() {
  return (
    <>
      <Router>
        <Navbar />
        <Routes>
          <Route path="/vis/:file" element={<VisualizerBoard />} />
          <Route path="/vis/gamechoice" element={<VisualizerFiles />} />
          {/* <Route path="/choose_colour_live_play" element={<ColourChooser />} /> */}
          {/* <Route path="/live_play" element={<LiveBoard />} /> */}
          <Route path="*" element={<VisualizerFiles />} />
        </Routes>
      </Router>
    </>
  );

  // const [name, setName] = useState("");
  // const [message, setMessage] = useState("");

  // const handleSubmit = () => {
  //   const request = new HelloRequest();
  //   request.setName(name);

  //   client.sayHello(request, {}, (err, response) => {
  //     if (err) {
  //       console.error(err);
  //       return;
  //     }
  //     setMessage(response.getMessage());
  //   });
  // };

  // return (
  //   <div>
  //     <input
  //       type="text"
  //       value={name}
  //       onChange={(e) => setName(e.target.value)}
  //       placeholder="Enter your name"
  //     />
  //     <button onClick={handleSubmit}>Send</button>
  //     <div>{message}</div>
  //   </div>
  // );
}

export default App;
