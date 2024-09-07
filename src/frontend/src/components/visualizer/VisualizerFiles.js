import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { getAll, getRandom } from "../../services/gamesManagerService";

const VisualizerFiles = () => {
  const [files, setFiles] = useState([]);
  const navigate = useNavigate();

  useEffect(() => {
    const fetchData = async () => {
      try {
        const { status, data } = await getAll();
        setFiles(data.getFilesList());
      } catch (err) {
        console.error(`Error ${err.status}: ${err.message}`);
      }
    };

    fetchData();
  }, []);

  const handleFileClick = (filename) => {
    navigate(`/vis/${filename}`);
  };

  return (
    <section className="board-section">
      <div className="visualizer-file-container">
        <div className="card run-container">
          <h5>Available Games:</h5>
          <div className="runs">
            {files.map((elem, index) => (
              <div
                key={index}
                className="btn btn-green"
                onClick={() => handleFileClick(elem)}
              >
                <h6>{elem}</h6>
              </div>
            ))}
          </div>
        </div>
      </div>
    </section>
  );
};

export default VisualizerFiles;
