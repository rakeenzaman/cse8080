function getGraphName(dotData) {
    var lines = dotData.split('\n');
    var digraphName = null;
    for (var i = 0; i < lines.length; i++) {
        var line = lines[i].trim();

        if (line.startsWith('digraph')) {
            digraphName = line.split(' ')[1];
            break;
        }
    }
    return digraphName;
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
        map.set(getGraphName(arrayOfGraphs[i]), arrayOfGraphs[i]);
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
