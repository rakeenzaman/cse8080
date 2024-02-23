document.getElementById('runCmdButton').addEventListener('click', function() {
    fetch('/run-command')
        .then(response => response.json())
        .then(data => {
            document.getElementById('output').textContent = data.data;
        })
        .catch(error => console.error('Error:', error));
});

