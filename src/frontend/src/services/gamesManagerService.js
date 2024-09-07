import { Empty } from "google-protobuf/google/protobuf/empty_pb";
import {
  GamesManagerClient,
  PlayerManagerClient,
} from "../generated/system_grpc_web_pb";
import { FileName } from "../generated/system_pb";

const games_client = new GamesManagerClient("http://localhost:8080");
// const players_client = new PlayerManagerClient("http://localhost:8080");
// Function to get all file names
export const getAll = () => {
  const request = new Empty();

  // Return a promise that wraps the callback-based gRPC method
  return new Promise((resolve, reject) => {
    games_client.getAll(request, {}, (err, response) => {
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
    games_client.get(request, {}, (err, response) => {
      if (err) {
        reject({ status: err.code, message: err.message });
      } else {
        resolve({ status: 0, data: response });
      }
    });
  });
};

// export const getLatest = () => {
//   const request = new Empty();
//   return new Promise((resolve, reject) => {
//     players_client.getLatest(request, {}, (err, response) => {
//       if (err) {
//         // Reject the promise with an error object
//         reject({ status: err.code, message: err.message });
//       } else {
//         // Resolve the promise with the status and data
//         resolve({ status: 0, data: response });
//       }
//     });
//   });
// };

// export const getRandom = () => {
//   const request = new Empty();
//   return new Promise((resolve, reject) => {
//     players_client.getRandom(request, {}, (err, response) => {
//       if (err) {
//         // Reject the promise with an error object
//         reject({ status: err.code, message: err.message });
//       } else {
//         // Resolve the promise with the status and data
//         resolve({ status: 0, data: response });
//       }
//     });
//   });
// };
