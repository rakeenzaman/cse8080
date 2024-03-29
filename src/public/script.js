var editor;
var map;

document.addEventListener('DOMContentLoaded', (event) => {
    document.getElementById('runCmdButton').addEventListener('click', createCFG);
  });

function createCFG() {
    document.getElementById("error").style.display = "none";
    d3.select("#output").selectAll("svg").style("display", "none");
    document.getElementById("placeholder_text").style.display = "none";
    document.getElementById("spinner-container").style.display = "block";
    localStorage.setItem("initialValueCfg", editor.getValue());
    const queryString = encodeURIComponent(editor.getValue());
    fetch('/run-command', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ data: editor.getValue() })
        })
        .then(response => response.json())
        .then(data => {
            var container = d3.select("#output");
            container.graphviz().renderDot(data.data);
            document.getElementById('functionName').innerHTML = getGraphName(getGraphData(data.data));
            document.getElementById('functionParams').innerHTML = getGraphParams(getGraphData(data.data));
            document.getElementById('functionType').innerHTML = getGraphType(getGraphData(data.data));
            console.log(data);

            var graphs = data.data.split(/(?=digraph )/);

            graphs = graphs.filter(function(graph) {
                return graph.trim().length > 0;
            });
            loading = true;
            document.getElementById("spinner-container").style.display = "none";



            map = createMap(data.data);
            setOptions(map);
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById("error").style.display = "block";
            d3.select("#output").selectAll("svg").style("display", "none");

        });
    };

async function openFileExplorer() {
    let [fileHandle] = await window.showOpenFilePicker({
        types: [
          {
            description: 'C++ Files',
            accept: {'text/cpp': ['.cpp']}
          }
        ]
      });
    let fileData = await fileHandle.getFile();
    let text = await fileData.text();
    editor.setValue(text);
}

function splitData(digraphName) {
    return digraphName.split(";");
}

function getGraphName(digraphName) {
    return splitData(digraphName)[0];
}

function getGraphType(digraphName) {
    return splitData(digraphName)[1];
}

function getGraphParams(digraphName) {
    return splitData(digraphName)[2];
}

function getGraphData(dotData) {
    return dotData.match(/"([^"]*)"/)[1];
}

function splitGraphsIntoArray(data) {
    var graphs = data.split(/(?=digraph )/);
    graphs = graphs.filter(function(graph) {
        return graph.trim().length > 0;
    });
    return graphs;
}

function createMap(data) {
    var arrayOfGraphs = splitGraphsIntoArray(data);
    const map = new Map();

    for (let i = 0; i < arrayOfGraphs.length; i++) {
        map.set(getGraphName(getGraphData(arrayOfGraphs[i])), arrayOfGraphs[i]);
    }
    return map
}

function setOptions(map) {
    var dropdown = document.getElementById("dropdown");
    dropdown.innerHTML = "";

    map.forEach(function(value, key) {
        // Create a new option element
        var option = document.createElement("option");

        // Set the value and text of the option
        option.value = key;
        option.text = key;

        // Append the option to the dropdown
        dropdown.appendChild(option);
    });
}
