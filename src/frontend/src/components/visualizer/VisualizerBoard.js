import React, { useState, useEffect } from "react";
import { useParams } from "react-router-dom";
import PContainer from "../PContainer";
import {
  FaFastForward,
  FaFastBackward,
  FaForward,
  FaBackward,
  FaStepForward,
  FaStepBackward,
} from "react-icons/fa";
import { IconContext } from "react-icons";
import Pos from "../Pos";
import Base from "../Base";
import { getGame } from "../../services/gamesManagerService";

const VisualizerBoard = () => {
  // Info state contains the whole game. An example of the game format is given as initialization
  const [info, setInfo] = useState({
    config: {
      players: [
        { name: "Player 1", colours: ["red", "yellow"] },
        { name: "Player 2", colours: ["green", "blue"] },
      ],
    },
    game: [
      {
        game_state: {
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
          last_move_id: 0,
          current_player: 0,
          dice_roll: [6, 6, 2],
          blocks: [
            { pawn_ids: ["R3", "R4"], rigid: true },
            { pawn_ids: ["R2", "Y3"], rigid: false },
            { pawn_ids: ["B2", "B3"], rigid: true },
          ],
          game_over: false,
          num_more_moves: 0,
        },
        move_id: 0,
        move: [["B4", "P16", "P18"]],
        top_moves: [],
      },
    ],
    player_won: 2,
  });
  const { file } = useParams();

  // The current board state that is being displayed
  const [boardState, setBoardState] = useState({
    game_state: {
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
      last_move_id: 0,
      current_player: 0,
      dice_roll: [6, 6, 2],
      blocks: [
        { pawn_ids: ["R3", "R4"], rigid: true },
        { pawn_ids: ["R2", "Y3"], rigid: false },
        { pawn_ids: ["B2", "B3"], rigid: true },
      ],
      game_over: false,
      num_more_moves: 0,
      selected_pawns: [],
      available_pos: [],
    },
    move_id: 0,
    move: [["B4", "P16", "P18"]],
    top_moves: [],
  });

  const convertStateToRepr = (mappings, state_proto) => {
    // Converts a state_proto to object representation
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
    // Converts a move_proto to a move object representation
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
  const convertGameToRepr = (game_proto) => {
    // Converts a game_proto to a game object representation
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

    // Parsing config
    let config = { players: [] };
    // const n_players = game_proto.getConfig().getNPlayers();
    const colour_player = game_proto.getConfig().getColourPlayerList();
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
    // console.log("config", config);

    // Parsing game
    let game = [];
    for (let i = 0; i < game_proto.getMovesList().length; i++) {
      game.push({
        game_state: convertStateToRepr(mappings, game_proto.getStatesList()[i]),
        move_id: i,
        move: convertMoveToRepr(mappings, game_proto.getMovesList()[i]),
        top_moves: [],
      });
    }
    game.push({
      game_state: convertStateToRepr(
        mappings,
        game_proto.getStatesList()[game_proto.getMovesList().length]
      ),
      move_id: game_proto.getMovesList().length,
      move: [],
      top_moves: [],
    });
    // console.log("game", game);
    return { config, game, player_won: game_proto.getWinner() };
  };

  useEffect(() => {
    // Initial useEffect to fetch the game data using the provided file
    const fetchData = async () => {
      try {
        const { status, data } = await getGame(file);
        // console.log("Game", data);
        let game_repr = convertGameToRepr(data);
        setInfo(game_repr);
        setBoardState(game_repr.game[0]);
      } catch (err) {
        console.error(`Error ${err.status}: ${err.message}`);
      }
    };

    fetchData();
  }, []);

  const handleStep = (steps) => {
    // Handles stepping through the game by displaying the correct board state
    let new_move_id = boardState.move_id + steps;
    if (new_move_id < 0) new_move_id = 0;
    else if (new_move_id >= info.game.length)
      new_move_id = info.game.length - 1;
    setBoardState(info.game[new_move_id]);
  };

  return (
    <section className="board-section">
      <div className="board-info-container">
        <div className="board-container">
          <div className="colour-container red-container">
            <Base
              colour="red"
              id="R"
              boardState={boardState.game_state}
              playerState={{ available_pos: [], selected_pawns: [] }}
              handlePawnClick={() => {}}
              handlePosClick={() => {}}
            />
          </div>
          <PContainer
            orientation={"vert"}
            colours={["", "", "", "", "", ""]}
            ids={["P12", "P11", "P10", "P9", "P8", "P7"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["", "green", "green", "green", "green", "green"]}
            ids={["P13", "GH1", "GH2", "GH3", "GH4", "GH5"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["", "green", "", "", "", ""]}
            ids={["P14", "P15", "P16", "P17", "P18", "P19"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <div className="colour-container green-container">
            <Base
              colour="green"
              id="G"
              boardState={boardState.game_state}
              playerState={{ available_pos: [], selected_pawns: [] }}
              handlePawnClick={() => {}}
              handlePosClick={() => {}}
            />
          </div>
          <PContainer
            orientation={"hor"}
            colours={["", "red", "", "", "", ""]}
            ids={["P1", "P2", "P3", "P4", "P5", "P6"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <Pos
            container_colour="green"
            classes="middle-winner"
            id="GH6"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <PContainer
            orientation={"hor"}
            colours={["", "", "", "", "", ""]}
            ids={["P20", "P21", "P22", "P23", "P24", "P25"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <PContainer
            orientation={"hor"}
            colours={["", "red", "red", "red", "red", "red"]}
            ids={["P52", "RH1", "RH2", "RH3", "RH4", "RH5"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <Pos
            container_colour="red"
            id="RH6"
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <Pos
            container_colour="yellow"
            id="YH6"
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <PContainer
            orientation={"hor"}
            colours={["yellow", "yellow", "yellow", "yellow", "yellow", ""]}
            ids={["YH5", "YH4", "YH3", "YH2", "YH1", "P26"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <PContainer
            orientation={"hor"}
            colours={["", "", "", "", "", ""]}
            ids={["P51", "P50", "P49", "P48", "P47", "P46"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <Pos
            container_colour="blue"
            id="BH6"
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <Pos
            container_colour="disabled"
            id=""
            classes="middle-winner"
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></Pos>
          <PContainer
            orientation={"hor"}
            colours={["", "", "", "", "yellow", ""]}
            ids={["P32", "P31", "P30", "P29", "P28", "P27"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <div className="colour-container blue-container">
            <Base
              colour="blue"
              id="B"
              boardState={boardState.game_state}
              playerState={{ available_pos: [], selected_pawns: [] }}
              handlePawnClick={() => {}}
              handlePosClick={() => {}}
            />
          </div>
          <PContainer
            orientation={"vert"}
            colours={["", "", "", "", "blue", ""]}
            ids={["P45", "P44", "P43", "P42", "P41", "P40"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["blue", "blue", "blue", "blue", "blue", ""]}
            ids={["BH5", "BH4", "BH3", "BH2", "BH1", "P39"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <PContainer
            orientation={"vert"}
            colours={["", "", "", "", "", ""]}
            ids={["P33", "P34", "P35", "P36", "P37", "P38"]}
            boardState={boardState.game_state}
            playerState={{ available_pos: [], selected_pawns: [] }}
            handlePawnClick={() => {}}
            handlePosClick={() => {}}
          ></PContainer>
          <div className="colour-container yellow-container">
            <Base
              colour="yellow"
              id="Y"
              boardState={boardState.game_state}
              playerState={{ available_pos: [], selected_pawns: [] }}
              handlePawnClick={() => {}}
              handlePosClick={() => {}}
            />
          </div>
        </div>
        <div className="info-container">
          <div className="step-container">
            <IconContext.Provider value={{ className: "step-icons", size: 30 }}>
              <>
                <FaFastBackward onClick={() => handleStep(-info.game.length)} />
                <FaBackward onClick={() => handleStep(-10)} />
                <FaStepBackward onClick={() => handleStep(-1)} />
                <span>Move: {boardState.move_id}</span>
                <FaStepForward onClick={() => handleStep(1)} />
                <FaForward onClick={() => handleStep(10)} />
                <FaFastForward onClick={() => handleStep(info.game.length)} />
              </>
            </IconContext.Provider>
          </div>
          <div className="players-container">
            {info.config.players.map((player, index) => {
              return (
                <div
                  key={player.name}
                  className={`player-container ${
                    index === boardState.game_state.current_player
                      ? "current-player"
                      : ""
                  } ${
                    boardState.move_id + 1 === info.game.length &&
                    index === info.player_won
                      ? "winner-player"
                      : ""
                  }`}
                >
                  {player.colours.map((colour, index) => {
                    return (
                      <button
                        key={index}
                        className={`btn pawn btn-${colour}`}
                      ></button>
                    );
                  })}
                  <span>{player.name}</span>
                </div>
              );
            })}
          </div>
          <div className="roll-num-container">
            <div>Roll: {JSON.stringify(boardState.game_state.dice_roll)}</div>
            <div>No. of Moves Left: {boardState.game_state.num_more_moves}</div>
          </div>
          <div className="move-taken-container">
            <span> Move Taken: </span>
            <span> {JSON.stringify(boardState.move)}</span>
          </div>
          <div className="top-moves-container">
            <h3>Top Moves</h3>
            <h3>Probability</h3>
            <h3>Value</h3>
            {boardState.top_moves &&
              boardState.top_moves.map((elem, index) => {
                return (
                  <>
                    <span>{JSON.stringify(elem.move)}</span>
                    <span>{elem.prob}</span>
                    <span>{elem.value}</span>
                  </>
                );
              })}
          </div>
        </div>
      </div>
    </section>
  );
};

export default VisualizerBoard;
