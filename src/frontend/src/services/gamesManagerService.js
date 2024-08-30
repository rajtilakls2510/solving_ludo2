import { Empty } from "google-protobuf/google/protobuf/empty_pb";
import { GamesManagerClient } from "../generated/system_grpc_web_pb";
import { FileName } from "../generated/system_pb";

const client = new GamesManagerClient("http://localhost:8080");

// Function to get all file names
export const getAll = () => {
  const request = new Empty();

  // Return a promise that wraps the callback-based gRPC method
  return new Promise((resolve, reject) => {
    client.getAll(request, {}, (err, response) => {
      if (err) {
        // Reject the promise with an error object
        reject({ status: err.code, message: err.message });
      } else {
        // Resolve the promise with the status and data
        resolve({ status: 0, data: response });
      }
    });
  });
};

// Function to get game file contents given a filename
export const getGame = (filename) => {
  const request = new FileName();
  request.setFile(filename);

  return new Promise((resolve, reject) => {
    client.get(request, {}, (err, response) => {
      if (err) {
        reject({ status: err.code, message: err.message });
      } else {
        resolve({ status: 0, data: response });
      }
    });
  });
};
