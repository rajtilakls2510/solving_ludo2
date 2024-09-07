import { Empty } from "google-protobuf/google/protobuf/empty_pb";
import { LiveplayManagerClient } from "../generated/system_grpc_web_pb";
import {
  NewGameConfig,
  PlayerConfig,
  PlayerMode,
  MoveRequest,
} from "../generated/system_pb";

const live_client = new LiveplayManagerClient("http://localhost:8080");

export const checkRunningGame = () => {
  const request = new Empty();

  return new Promise((resolve, reject) => {
    live_client.checkRunningGame(request, {}, (err, response) => {
      if (err) reject({ status: err.code, message: err.message });
      else resolve({ status: 0, data: response });
    });
  });
};

export const resetGame = () => {
  const request = new Empty();
  return new Promise((resolve, reject) => {
    live_client.reset(request, {}, (err, response) => {
      if (err) reject({ status: err.code, message: err.message });
      else resolve({ status: 0, data: response });
    });
  });
};

export const createNewGame = (players_config) => {
  // [{mode: "AI", colours: ["red", "yellow"]}, {mode: "Human", colours: ["blue", "green"]}]

  const new_game_config = new NewGameConfig();

  // Helper function to map string mode to protobuf PlayerMode enum
  function getPlayerMode(mode) {
    if (mode === "AI") {
      return PlayerMode.AI;
    } else if (mode === "Human") {
      return PlayerMode.HUMAN;
    }
    throw new Error("Invalid player mode");
  }

  // Create player configurations based on input
  const playerConfigs = players_config.map((player) => {
    const playerConfig = new PlayerConfig();
    playerConfig.setMode(getPlayerMode(player.mode));
    playerConfig.setColoursList(player.colours);
    return playerConfig;
  });

  // Set player configurations in the NewGameConfig object
  new_game_config.setPlayerConfigsList(playerConfigs);

  return new Promise((resolve, reject) => {
    live_client.createNewGame(new_game_config, {}, (err, response) => {
      if (err) reject({ status: err.code, message: err.message });
      else resolve({ status: 0, data: response });
    });
  });
};

export const getCurrentState = () => {
  const request = new Empty();
  return new Promise((resolve, reject) => {
    live_client.getCurrentState(request, {}, (err, response) => {
      if (err) reject({ status: err.code, message: err.message });
      else resolve({ status: 0, data: response });
    });
  });
};

export const takeMove = (move, moveid) => {
  const request = new MoveRequest();
  request.setMove(move);
  request.setMoveId(moveid);

  return new Promise((resolve, reject) => {
    live_client.takeMove(request, {}, (err, response) => {
      if (err) reject({ status: err.code, message: err.message });
      else resolve({ status: 0, data: response });
    });
  });
};
