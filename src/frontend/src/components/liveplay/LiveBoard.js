import React, { useState, useEffect, useRef } from "react";
import { useNavigate } from "react-router-dom";
import PContainer from "../PContainer";
import Pos from "../Pos";
import Base from "../Base";
import { MoveProto } from "../../generated/ludo_pb";
import {
  resetGame,
  checkRunningGame,
  getCurrentState,
  takeMove,
} from "../../services/liveplayManagerService";

const LiveBoard = () => {
  const navigate = useNavigate();

  // board state contains the current game state along with the configuration of the game and available moves
  const [boardState, setBoardState] = useState({
    config: {
      players: [
        { name: "Player 0", colours: ["red", "yellow"] },
        { name: "Player 1", colours: ["green", "blue"] },
      ],
    },
    modes: ["Human", "Human"],
    pawns: {
      R1: { colour: "red", blocked: false },
      R2: { colour: "red", blocked: true },
      R3: { colour: "red", blocked: true },
      R4: { colour: "red", blocked: true },
      G1: { colour: "green", blocked: false },
      G2: { colour: "green", blocked: false },
      G3: { colour: "green", blocked: false },
      G4: { colour: "green", blocked: false },
      Y1: { colour: "yellow", blocked: false },
      Y2: { colour: "yellow", blocked: false },
      Y3: { colour: "yellow", blocked: true },
      Y4: { colour: "yellow", blocked: false },
      B1: { colour: "blue", blocked: false },
      B2: { colour: "blue", blocked: true },
      B3: { colour: "blue", blocked: true },
      B4: { colour: "blue", blocked: false },
    },
    positions: [
      { pawn_id: "R1", pos_id: "RB1" },
      { pawn_id: "R2", pos_id: "P23" },
      { pawn_id: "R3", pos_id: "P4" },
      { pawn_id: "R4", pos_id: "P4" },
      { pawn_id: "G1", pos_id: "GB1" },
      { pawn_id: "G2", pos_id: "P24" },
      { pawn_id: "G3", pos_id: "P35" },
      { pawn_id: "G4", pos_id: "P41" },
      { pawn_id: "Y1", pos_id: "YB1" },
      { pawn_id: "Y2", pos_id: "P28" },
      { pawn_id: "Y3", pos_id: "P23" },
      { pawn_id: "Y4", pos_id: "YH3" },
      { pawn_id: "B1", pos_id: "BB1" },
      { pawn_id: "B2", pos_id: "P5" },
      { pawn_id: "B3", pos_id: "P5" },
      { pawn_id: "B4", pos_id: "BH2" },
    ],
    last_move_id: 1,
    current_player: 5,
    dice_roll: [6, 6, 2],
    blocks: [
      { pawn_ids: ["R3", "R4"], rigid: true },
      { pawn_ids: ["R2", "Y3"], rigid: false },
      { pawn_ids: ["B2", "B3"], rigid: true },
    ],
    game_over: false,
    moves: [],
  });

  // Player state contains the current player pawn selection
  const [playerState, setPlayerState] = useState({
    current_move: [],
    selected_pawns: [],
    available_pos: [],
  });

  useEffect(() => {
    // Checks whether a game is already running or not. If running, display the board state, otherwise redirect to colour chooser page
    const fetchRunningGame = async () => {
      try {
        const { state, data } = await checkRunningGame();
      } catch (err) {
        navigate("/choose_colour_live_play");
      }
    };
    fetchRunningGame();
  }, []);
  useInterval(() => {
    // Periodically fetch the current board state in case playing with AI and the AI has already taken the move
    const fetchCurrentBoard = async () => {
      try {
        const { status, data } = await getCurrentState();
        applyBoardState(data);
      } catch (err) {
        console.error(`Board Fetch Error ${err.status}: ${err.message}`);
      }
    };
    fetchCurrentBoard();
  }, 2000);

  const convertConfigToRepr = (mappings, config_proto) => {
    // Parsing config
    let config = { players: [] };
    // const n_players = game_proto.getConfig().getNPlayers();
    const colour_player = config_proto.getColourPlayerList();
    let players = {};
    for (let colour = 1; colour < colour_player.length; colour++) {
      if (!(`Player ${colour_player[colour]}` in players))
        players[`Player ${colour_player[colour]}`] = [];
      players[`Player ${colour_player[colour]}`].push(mappings.colours[colour]);
    }

    // Get and sort the keys based on the player index
    const sortedPlayerNames = Object.keys(players).sort((a, b) => {
      // Extract the number part of the string
      const indexA = parseInt(a.split(" ")[1], 10);
      const indexB = parseInt(b.split(" ")[1], 10);

      // Sort based on the number part
      return indexA - indexB;
    });
    sortedPlayerNames.forEach((player_name) => {
      config.players.push({ name: player_name, colours: players[player_name] });
    });
    return config;
  };

  const convertStateToRepr = (mappings, state_proto) => {
    let pb = new Array(17).fill(false);
    let blocks = [];
    for (let i = 0; i < state_proto.getNumBlocks(); i++) {
      let block_pawns = [];
      let p = state_proto.getBlocksList()[i].getPawns();
      while (p !== 0) {
        pb[p % 17] = true;
        block_pawns.push(mappings["pawn"][p % 17]);
        p = Math.floor(p / 17);
      }
      blocks.push({
        pawn_ids: block_pawns,
        rigid: state_proto.getBlocksList()[i].getRigid(),
      });
    }

    let pawns = {};
    let positions = [];
    for (let player = 0; player < state_proto.getNPlayers(); player++) {
      for (let j = 0; j < 93; j++) {
        let p = state_proto.getPlayerPosPawnList()[player * 93 + j];
        while (p !== 0) {
          let pawnIndex = p % 17;
          pawns[mappings["pawn"][pawnIndex]] = {
            colour: mappings["colours"][Math.floor((pawnIndex - 1) / 4) + 1],
            blocked: pb[pawnIndex],
          };
          positions.push({
            pawn_id: mappings["pawn"][pawnIndex],
            pos_id: mappings["pos"][j],
          });
          p = Math.floor(p / 17);
        }
      }
    }
    // console.log("pawns", pawns);
    // console.log("positions", positions);
    let dice_roll = [];
    let roll = state_proto.getDiceRoll();
    while (roll >= 6) {
      dice_roll.push(6);
      roll -= 6;
    }
    if (roll !== 0) dice_roll.push(roll);

    let state = {
      game_over: state_proto.getGameOver(),
      pawns,
      positions,
      current_player: state_proto.getCurrentPlayer(),
      last_move_id: state_proto.getLastMoveId(),
      num_more_moves: state_proto.getNumMoreThrows(),
      blocks,
      dice_roll,
    };
    return state;
  };
  const convertMoveToRepr = (mappings, move_proto) => {
    let move = [];
    let pawn = [];
    if (move_proto.getPawn() !== 0) {
      if (move_proto.getPawn() > 16) {
        let p = move_proto.getPawn();
        while (p !== 0) {
          pawn.push(mappings["pawn"][p % 17]);
          p = Math.floor(p / 17);
        }
      } else pawn = mappings["pawn"][move_proto.getPawn()];
      move.push(pawn);
      move.push(mappings["pos"][move_proto.getCurrentPos()]);
      move.push(mappings["pos"][move_proto.getDestination()]);
      // console.log("move", move);
    }
    return move;
  };

  const mappings = {
    pawn: [
      "",
      "R1",
      "R2",
      "R3",
      "R4",
      "G1",
      "G2",
      "G3",
      "G4",
      "Y1",
      "Y2",
      "Y3",
      "Y4",
      "B1",
      "B2",
      "B3",
      "B4",
    ],
    pos: [
      "",
      "RB1",
      "RB2",
      "RB3",
      "RB4",
      "GB1",
      "GB2",
      "GB3",
      "GB4",
      "YB1",
      "YB2",
      "YB3",
      "YB4",
      "BB1",
      "BB2",
      "BB3",
      "BB4",
    ]
      .concat([...Array(52).keys()].map((i) => `P${i + 1}`))
      .concat([
        "RH1",
        "RH2",
        "RH3",
        "RH4",
        "RH5",
        "RH6",
        "GH1",
        "GH2",
        "GH3",
        "GH4",
        "GH5",
        "GH6",
        "YH1",
        "YH2",
        "YH3",
        "YH4",
        "YH5",
        "YH6",
        "BH1",
        "BH2",
        "BH3",
        "BH4",
        "BH5",
        "BH6",
      ]),
    colours: ["", "red", "green", "yellow", "blue"],
  };

  const applyBoardState = (data) => {
    // Parses the current game state from protobuffer and converts to object format
    let state_repr = convertStateToRepr(mappings, data.getState());
    let config_repr = convertConfigToRepr(mappings, data.getConfig());
    let modes = data.getModesList().map((mode) => {
      return mode === 0 ? "AI" : "Human";
    });
    let moves = data.getMovesList().map((move) => {
      return convertMoveToRepr(mappings, move);
    });

    if (state_repr.last_move_id !== boardState.last_move_id) {
      setPlayerState({
        current_move: [],
        selected_pawns: [],
        available_pos: [],
      });
    }
    setBoardState({ config: config_repr, ...state_repr, modes, moves });
  };

  const checkContainsInArrayUsingPos = (move1, move) => {
    if (!Array.isArray(move[0])) {
      if (
        !Array.isArray(move1[0]) &&
        move1[0] === move[0] &&
        move1[2] === move[1]
      )
        return true;
    } else {
      if (
        Array.isArray(move1[0]) &&
        move1[0].every((elem) => move[0].includes(elem)) &&
        move1[2] === move[1]
      )
        return true;
    }
    return false;
  };

  const convertMoveToProto = (mappings, move) => {
    let move_proto = new MoveProto();
    if (move.length > 0) {
      // Generate inverse mappings
      const inverseMappings = {
        pawn: {},
        pos: {},
      };

      // Fill inverse mappings with key-value pairs
      mappings.pawn.forEach((value, index) => {
        if (value) inverseMappings.pawn[value] = index;
      });

      mappings.pos.forEach((value, index) => {
        if (value) inverseMappings.pos[value] = index;
      });

      const pawn = move[0]; // This can be a string or array of strings
      const currentPos = move[1]; // This is a string
      const destination = move[2]; // This is a string

      // Calculate pawn index
      let pawnIndex = 0;
      if (Array.isArray(pawn)) {
        for (let i = 0; i < pawn.length; i++) {
          pawnIndex = pawnIndex * 17 + inverseMappings.pawn[pawn[i]];
        }
      } else {
        pawnIndex = inverseMappings.pawn[pawn];
      }

      // Set pawn index in move_proto
      move_proto.setPawn(pawnIndex);

      // Set currentPos and destination in move_proto
      move_proto.setCurrentPos(inverseMappings.pos[currentPos]);
      move_proto.setDestination(inverseMappings.pos[destination]);
    }
    return move_proto;
  };

  useEffect(() => {
    // Monitors the current move selection of the player and takes the move
    let available_moves = [...boardState.moves];
    available_moves = available_moves.filter((elem) =>
      checkContainsInArrayUsingPos(elem, playerState.current_move)
    );
    console.log("available moves", available_moves);

    if (!boardState.game_over) {
      if (available_moves.length > 0) {
        const takeTheMove = async () => {
          try {
            const { status, data } = await takeMove(
              convertMoveToProto(mappings, available_moves[0]),
              boardState.last_move_id + 1
            );
            applyBoardState(data);
          } catch (err) {
            console.error(`Board Fetch Error ${err.status}: ${err.message}`);
          }
        };
        takeTheMove();
      } else if (
        boardState.moves.length === 0 &&
        boardState.moves[boardState.current_player] !== "AI"
      ) {
        console.log("no valid move found, skipping turn");
        const takePassMove = async () => {
          try {
            await new Promise((resolve) => setTimeout(resolve, 2000));
            const { status, data } = await takeMove(
              convertMoveToProto(mappings, []),
              boardState.last_move_id + 1
            );
            applyBoardState(data);
          } catch (err) {
            console.error(`Board Fetch Error ${err.status}: ${err.message}`);
          }
        };
        takePassMove();
      }
    } else {
      console.log("GAME OVER");
    }
  }, [playerState.current_move]);

  useEffect(() => {
    // Monitors the currently selected pawns of the player and filters the applicable moves
    let available_pos = [];
    if (playerState.selected_pawns.length === 1) {
      for (let i = 0; i < boardState.moves.length; i++) {
        if (
          !Array.isArray(boardState.moves[i][0]) &&
          boardState.moves[i][0] === playerState.selected_pawns[0] &&
          !available_pos.includes(boardState.moves[i][2])
        )
          available_pos.push(boardState.moves[i][2]);
      }
    } else if (playerState.selected_pawns.length > 1) {
      for (let i = 0; i < boardState.moves.length; i++) {
        if (
          Array.isArray(boardState.moves[i][0]) &&
          boardState.moves[i][0].every((elem) =>
            playerState.selected_pawns.includes(elem)
          ) &&
          !available_pos.includes(boardState.moves[i][2])
        )
          available_pos.push(boardState.moves[i][2]);
      }
    }
    setPlayerState({ ...playerState, available_pos: available_pos });
  }, [playerState.selected_pawns]);

  const handlePawnClick = (pawn_id) => {
    if (boardState.modes[boardState.current_player] !== "AI") {
      let selected_pawns = playerState.selected_pawns;
      if (playerState.selected_pawns.includes(pawn_id)) {
        // If pawn is already selected, remove it from selected list and any other rigid block pawns
        selected_pawns = selected_pawns.filter((elem) => elem !== pawn_id);
        let block = boardState.blocks.filter((elem) =>
          elem.pawn_ids.includes(pawn_id)
        );

        if (block.length === 1 && block[0].rigid)
          selected_pawns = selected_pawns.filter(
            (elem) => !block[0].pawn_ids.includes(elem)
          );
      } else {
        // If pawn is not selected, add it to selected list and any other rigid block pawns
        let block = boardState.blocks.filter((elem) =>
          elem.pawn_ids.includes(pawn_id)
        );

        if (block.length === 1 && block[0].rigid)
          selected_pawns = [...selected_pawns, ...block[0].pawn_ids];
        else selected_pawns = [...selected_pawns, pawn_id];
      }
      setPlayerState({
        ...playerState,
        selected_pawns: selected_pawns,
      });
    }
  };
  const handlePosClick = (pos_id) => {
    let selected_pawns = playerState.selected_pawns;
    if (playerState.available_pos.includes(pos_id)) {
      if (selected_pawns.length === 1) selected_pawns = selected_pawns[0];
      setPlayerState({
        ...playerState,
        current_move: [selected_pawns, pos_id],
        selected_pawns: [],
      });
    }
  };

  const handleReset = () => {
    // Reset stops the game and redirects to colour chooser page
    const resetG = async () => {
      try {
        const { status, data } = await resetGame();
        navigate("/choose_colour_live_play");
      } catch (err) {
        console.error(`Reset Error ${err.status}: ${err.message}`);
      }
    };
    resetG();
  };
  return (
    <section className="board-section">
      <div className="board-info-container">
        <div className="board-container">
          <div className="colour-container red-container">
            <Base
              colour="red"
              id="R"
              boardState={boardState}
              playerState={playerState}
              handlePawnClick={handlePawnClick}
              handlePosClick={handlePosClick}
            />
          </div>
          <PContainer
            orientation={"vert"}
            colours={["", "", "", "", "", ""]}
            ids={["P12", "P11", "P10", "P9", "P8", "P7"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["", "green", "green", "green", "green", "green"]}
            ids={["P13", "GH1", "GH2", "GH3", "GH4", "GH5"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["", "green", "", "", "", ""]}
            ids={["P14", "P15", "P16", "P17", "P18", "P19"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <div className="colour-container green-container">
            <Base
              colour="green"
              id="G"
              boardState={boardState}
              playerState={playerState}
              handlePawnClick={handlePawnClick}
              handlePosClick={handlePosClick}
            />
          </div>
          <PContainer
            orientation={"hor"}
            colours={["", "red", "", "", "", ""]}
            ids={["P1", "P2", "P3", "P4", "P5", "P6"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <Pos
            container_colour="green"
            classes="middle-winner"
            id="GH6"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <PContainer
            orientation={"hor"}
            colours={["", "", "", "", "", ""]}
            ids={["P20", "P21", "P22", "P23", "P24", "P25"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <PContainer
            orientation={"hor"}
            colours={["", "red", "red", "red", "red", "red"]}
            ids={["P52", "RH1", "RH2", "RH3", "RH4", "RH5"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <Pos
            container_colour="red"
            id="RH6"
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <Pos
            container_colour="yellow"
            id="YH6"
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <PContainer
            orientation={"hor"}
            colours={["yellow", "yellow", "yellow", "yellow", "yellow", ""]}
            ids={["YH5", "YH4", "YH3", "YH2", "YH1", "P26"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <PContainer
            orientation={"hor"}
            colours={["", "", "", "", "", ""]}
            ids={["P51", "P50", "P49", "P48", "P47", "P46"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <Pos
            container_colour="blue"
            id="BH6"
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></Pos>
          <PContainer
            orientation={"hor"}
            colours={["", "", "", "", "yellow", ""]}
            ids={["P32", "P31", "P30", "P29", "P28", "P27"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <div className="colour-container blue-container">
            <Base
              colour="blue"
              id="B"
              boardState={boardState}
              playerState={playerState}
              handlePawnClick={handlePawnClick}
              handlePosClick={handlePosClick}
            />
          </div>
          <PContainer
            orientation={"vert"}
            colours={["", "", "", "", "blue", ""]}
            ids={["P45", "P44", "P43", "P42", "P41", "P40"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["blue", "blue", "blue", "blue", "blue", ""]}
            ids={["BH5", "BH4", "BH3", "BH2", "BH1", "P39"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["", "", "", "", "", ""]}
            ids={["P33", "P34", "P35", "P36", "P37", "P38"]}
            boardState={boardState}
            playerState={playerState}
            handlePawnClick={handlePawnClick}
            handlePosClick={handlePosClick}
          ></PContainer>
          <div className="colour-container yellow-container">
            <Base
              colour="yellow"
              id="Y"
              boardState={boardState}
              playerState={playerState}
              handlePawnClick={handlePawnClick}
              handlePosClick={handlePosClick}
            />
          </div>
        </div>
        <div className="info-container">
          <div className="players-container">
            {boardState.config.players.map((player, index) => {
              return (
                <div
                  key={player.name}
                  className={`player-container ${
                    index === boardState.current_player ? "current-player" : ""
                  }`}
                >
                  {player.colours.map((colour) => {
                    return (
                      <button className={`btn pawn btn-${colour}`}></button>
                    );
                  })}
                  <span>{player.name}</span>
                </div>
              );
            })}
          </div>
          <div className="roll-num-container">
            <div>Roll: {JSON.stringify(boardState.dice_roll)}</div>
            <div>No. of Moves Left: {boardState.num_more_moves}</div>
          </div>
          <div style={{ margin: "auto" }}>
            <button className="btn btn-red" onClick={handleReset}>
              Reset Game
            </button>
          </div>
        </div>
      </div>
    </section>
  );
};

function useInterval(callback, delay) {
  const savedCallback = useRef();

  // Remember the latest callback.
  useEffect(() => {
    savedCallback.current = callback;
  }, [callback]);

  // Set up the interval.
  useEffect(() => {
    function tick() {
      savedCallback.current();
    }
    if (delay !== null) {
      let id = setInterval(tick, delay);
      return () => clearInterval(id);
    }
  }, [delay]);
}

export default LiveBoard;
