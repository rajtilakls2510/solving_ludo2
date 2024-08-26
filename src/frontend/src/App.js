import React, { useState } from "react";
import { GrpcWebClientBase } from "grpc-web";
import { GreeterClient } from "./generated/service_grpc_web_pb";
import { HelloRequest } from "./generated/service_pb";

const client = new GreeterClient("http://localhost:8080"); // Envoy proxy URL

function App() {
  const [name, setName] = useState("");
  const [message, setMessage] = useState("");

  const handleSubmit = () => {
    const request = new HelloRequest();
    request.setName(name);

    client.sayHello(request, {}, (err, response) => {
      if (err) {
        console.error(err);
        return;
      }
      setMessage(response.getMessage());
    });
  };

  return (
    <div>
      <input
        type="text"
        value={name}
        onChange={(e) => setName(e.target.value)}
        placeholder="Enter your name"
      />
      <button onClick={handleSubmit}>Send</button>
      <div>{message}</div>
    </div>
  );
}

export default App;
