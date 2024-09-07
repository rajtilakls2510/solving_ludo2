import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import {
  checkRunningGame,
  createNewGame,
} from "../../services/liveplayManagerService";

const ColourChooser = () => {
  const [loader, setLoader] = useState(false);
  const navigate = useNavigate();
  useEffect(() => {
    const fetchRunningGame = async () => {
      try {
        const { state, data } = await checkRunningGame();
        navigate("/live_play");
      } catch (err) {
        console.log(err);
      }
    };
    fetchRunningGame();
  }, []);

  const create_new_game = (colours) => {
    setLoader(true);
    if (colours === "ry") {
      const cNG = async () => {
        try {
          const { status, data } = await createNewGame([
            { mode: "Human", colours: ["green", "blue"] },
            { mode: "Human", colours: ["red", "yellow"] },
          ]);
          navigate("/live_play");
        } catch (err) {
          console.error(`Error ${err.status}: ${err.message}`);
        } finally {
          setLoader(false);
        }
      };
      cNG();
    } else {
      const cNG = async () => {
        try {
          const { status, data } = await createNewGame([
            { mode: "Human", colours: ["red", "yellow"] },
            { mode: "Human", colours: ["blue", "green"] },
          ]);
          navigate("/live_play");
        } catch (err) {
          console.error(`Error ${err.status}: ${err.message}`);
        } finally {
          setLoader(false);
        }
      };
      cNG();
    }
  };

  return (
    <section className="board-section">
      <div className="visualizer-file-container">
        <div className="card run-container">
          <h5>Choose Colour for Doubles (Player 1):</h5>

          <div className="live-double-colour-container">
            <div
              className="player-container colour-combination"
              onClick={() => create_new_game("ry")}
            >
              <button className={`btn pawn btn-red`}></button>
              <button className={`btn pawn btn-yellow`}></button>
              <span>Combination 1</span>
            </div>
            <div
              className="player-container colour-combination"
              onClick={() => create_new_game("gb")}
            >
              <button className={`btn pawn btn-green`}></button>
              <button className={`btn pawn btn-blue`}></button>
              <span>Combination 2</span>
            </div>
          </div>
          {loader ? <div className="loader"></div> : ""}
        </div>
        <div className="card run-container">
          <h5>Choose Game configuration for Singles:</h5>
          <span>NOT IMPLEMENTED YET</span>
        </div>
      </div>
    </section>
  );
};

export default ColourChooser;
